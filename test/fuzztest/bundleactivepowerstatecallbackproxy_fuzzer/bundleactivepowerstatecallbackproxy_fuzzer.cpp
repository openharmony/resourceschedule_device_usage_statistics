/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "bundleactivepowerstatecallbackproxy_fuzzer.h"
#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "bundle_active_power_state_callback_proxy.h"
#include "power_state_callback_ipc_interface_code.h"
#include "power_state_machine_info.h"
#include "bundle_active_open_callback.h"
#include "bundle_active_service.h"
#include "bundle_active_client.h"
#include "bundle_active_stats_combiner.h"
#include "bundle_active_event_list.h"

namespace OHOS {
namespace DeviceUsageStats {
    static std::string g_defaultBundleName = "com.ohos.camera";
    static std::string g_defaultMoudleName = "defaultmoudlename";
    static std::string g_defaultFormName = "defaultformname";
    static int32_t DEFAULT_DIMENSION = 4;
    static int64_t DEFAULT_FORMID = 1;
    constexpr uint32_t U32_AT_SIZE = 4;
    constexpr uint8_t TWENTYFOUR = 24;
    constexpr uint8_t SIXTEEN = 16;
    constexpr uint8_t EIGHT = 8;
    constexpr uint8_t TWO = 2;
    constexpr uint8_t THREE = 3;

    uint32_t GetU32Data(const char* ptr)
    {
        return (ptr[0] << TWENTYFOUR) | (ptr[1] << SIXTEEN) | (ptr[TWO] << EIGHT) | (ptr[THREE]);
    }

    std::string GetStringFromData(int strlen, const char* ptr)
    {
        if (strlen <= 0) {
            return "";
        }
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = static_cast<char>(GetU32Data(ptr));
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }
    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        uint32_t code = GetU32Data(data);
        PowerMgr::PowerState state = static_cast<PowerMgr::PowerState>(code);
        const sptr<IRemoteObject> tempImpl;
        BundleActivePowerStateCallbackProxy bundleActivePowerStateCallbackProxy(tempImpl);
        bundleActivePowerStateCallbackProxy.OnPowerStateChanged(state);
        return true;
    }

    bool BundleActiveClientFuzzTest(const char* data, size_t size)
    {
        bool result = false;
        int32_t userId = static_cast<int32_t>(GetU32Data(data));
        std::string inputBundleName(data);
        sptr<IAppGroupCallback> appGroupCallback = nullptr;
        int32_t intervalType = static_cast<int32_t>(GetU32Data(data));
        int64_t beginTime = static_cast<int64_t>(GetU32Data(data));
        int64_t endTime = static_cast<int64_t>(GetU32Data(data));

        DelayedSingleton<BundleActiveClient>::GetInstance()->GetBundleActiveProxy();
        DelayedSingleton<BundleActiveClient>::GetInstance()->RegisterAppGroupCallBack(appGroupCallback);
        DelayedSingleton<BundleActiveClient>::GetInstance()->UnRegisterAppGroupCallBack(appGroupCallback);
        DelayedSingleton<BundleActiveClient>::GetInstance()->IsBundleIdle(result, inputBundleName, userId);
        BundleActiveEvent event(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
        DelayedSingleton<BundleActiveClient>::GetInstance()->ReportEvent(event, userId);

        std::vector<BundleActivePackageStats> packageStats;
        DelayedSingleton<BundleActiveClient>::GetInstance()->QueryBundleStatsInfoByInterval(packageStats,
        intervalType, beginTime, endTime, userId);

        std::vector<BundleActiveEvent> bundleActiveEvent;
        DelayedSingleton<BundleActiveClient>::GetInstance()->QueryBundleEvents(bundleActiveEvent,
        beginTime, endTime, userId);

        int32_t newGroup = static_cast<int32_t>(GetU32Data(data));
        std::string bundleName = GetStringFromData(size, data);
        DelayedSingleton<BundleActiveClient>::GetInstance()->SetAppGroup(bundleName, newGroup, userId);

        std::vector<BundleActiveEventStats> eventStats;
        DelayedSingleton<BundleActiveClient>::GetInstance()->QueryDeviceEventStats(beginTime, endTime,
        eventStats, userId);
        DelayedSingleton<BundleActiveClient>::GetInstance()->QueryNotificationEventStats(beginTime, endTime,
        eventStats, userId);
        return true;
    }

    bool BundleActiveEventListFuzzTest(const char* data, size_t size)
    {
        BundleActiveEventList right;
        int64_t resultData = static_cast<int64_t>(GetU32Data(data));
        auto combiner = std::make_shared<BundleActiveEventList>();
        BundleActiveEvent event;
        event.bundleName_ = GetStringFromData(size, data);
        event.continuousTaskAbilityName_ = GetStringFromData(size, data);
        event.timeStamp_ = static_cast<int64_t>(GetU32Data(data));

        combiner->Size();
        combiner->FindBestIndex(resultData);
        combiner->Insert(event);
        combiner->Merge(right);
        combiner->Clear();
        return true;
    }

    bool BundleActiveStatsCombinerFuzzTest(const char* data, size_t size)
    {
        auto combiner = std::make_shared<BundleActiveStatsCombiner<BundleActivePackageStats>>();
        auto stats = std::make_shared<BundleActivePeriodStats>();
        auto packageStat = std::make_shared<BundleActivePackageStats>();
        stats->bundleStats_.emplace("normal", packageStat);
        packageStat = nullptr;
        stats->bundleStats_.emplace("default", packageStat);
        int64_t beginTime = static_cast<int64_t>(GetU32Data(data));
        std::vector<BundleActivePackageStats> accumulatedResult;
        combiner->combine(stats, accumulatedResult, beginTime);

        auto eventCombiner = std::make_shared<BundleActiveStatsCombiner<BundleActiveEvent>>();
        std::vector<BundleActiveEvent> activeEventResult;
        eventCombiner->combine(stats, activeEventResult, beginTime);
        return true;
    }
} // namespace DeviceUsageStats
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::DeviceUsageStats::U32_AT_SIZE) {
        return 0;
    }

    OHOS::DeviceUsageStats::DoSomethingInterestingWithMyAPI(ch, size);
    OHOS::DeviceUsageStats::BundleActiveClientFuzzTest(ch, size);
    OHOS::DeviceUsageStats::BundleActiveEventListFuzzTest(ch, size);
    OHOS::DeviceUsageStats::BundleActiveStatsCombinerFuzzTest(ch, size);
    return 0;
}


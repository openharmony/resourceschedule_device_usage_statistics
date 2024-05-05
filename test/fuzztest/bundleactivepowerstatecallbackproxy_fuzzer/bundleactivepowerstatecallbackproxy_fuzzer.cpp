/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <securec.h>
#include "bundle_active_power_state_callback_proxy.h"
#include "power_state_callback_ipc_interface_code.h"
#include "power_state_machine_info.h"
#include "bundle_active_open_callback.h"
#include "bundle_active_service.h"
#include "bundle_active_client.h"
#include "bundle_active_stats_combiner.h"
#include "bundle_active_event_list.h"

#ifndef errno_t
typedef int errno_t;
#endif

#ifndef EOK
#define EOK 0
#endif

namespace OHOS {
namespace DeviceUsageStats {
    static std::string g_defaultBundleName = "com.ohos.camera";
    static std::string g_defaultMoudleName = "defaultmoudlename";
    static std::string g_defaultFormName = "defaultformname";
    static int32_t DEFAULT_DIMENSION = 4;
    static int64_t DEFAULT_FORMID = 1;
    constexpr uint32_t U32_AT_SIZE = 4;
    
    const uint8_t* g_data = nullptr;
    size_t g_size = 0;
    size_t g_pos;

    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    std::string GetStringFromData(int strlen)
    {
        if (strlen <= 0) {
            return "";
        }
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            char tmp = GetData<char>();
            if (tmp == '\0') {
                tmp = '1';
            }
            cstr[i] = tmp;
        }
        std::string str(cstr);
        return str;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        g_data = data;
        g_size = size;
        g_pos = 0;
        uint32_t code = GetData<uint32_t>();
        PowerMgr::PowerState state = static_cast<PowerMgr::PowerState>(code);
        const sptr<IRemoteObject> tempImpl;
        BundleActivePowerStateCallbackProxy bundleActivePowerStateCallbackProxy(tempImpl);
        bundleActivePowerStateCallbackProxy.OnPowerStateChanged(state);
        return true;
    }

    bool BundleActiveClientFuzzTest(const uint8_t* data, size_t size)
    {
        g_data = data;
        g_size = size;
        g_pos = 0;
        bool result = false;
        int32_t userId = GetData<int32_t>();
        std::string inputBundleName = GetStringFromData(size);
        sptr<IAppGroupCallback> appGroupCallback = nullptr;
        int32_t intervalType = GetData<int32_t>();
        int64_t beginTime = GetData<int64_t>();
        int64_t endTime = GetData<int64_t>();

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

        int32_t newGroup = GetData<int32_t>();
        std::string bundleName = GetStringFromData(size);
        DelayedSingleton<BundleActiveClient>::GetInstance()->SetAppGroup(bundleName, newGroup, userId);

        std::vector<BundleActiveEventStats> eventStats;
        DelayedSingleton<BundleActiveClient>::GetInstance()->QueryDeviceEventStats(beginTime, endTime,
        eventStats, userId);
        DelayedSingleton<BundleActiveClient>::GetInstance()->QueryNotificationEventStats(beginTime, endTime,
        eventStats, userId);
        return true;
    }

    bool BundleActiveEventListFuzzTest(const uint8_t* data, size_t size)
    {
        g_data = data;
        g_size = size;
        g_pos = 0;
        BundleActiveEventList right;
        int64_t resultData = GetData<int64_t>();
        auto combiner = std::make_shared<BundleActiveEventList>();
        BundleActiveEvent event;
        event.bundleName_ = GetStringFromData(size);
        event.continuousTaskAbilityName_ = GetStringFromData(size);
        event.timeStamp_ = GetData<int64_t>();

        combiner->Size();
        combiner->FindBestIndex(resultData);
        combiner->Insert(event);
        combiner->Merge(right);
        combiner->Clear();
        return true;
    }

    bool BundleActiveStatsCombinerFuzzTest(const uint8_t* data, size_t size)
    {
        g_data = data;
        g_size = size;
        g_pos = 0;
        auto combiner = std::make_shared<BundleActiveStatsCombiner<BundleActivePackageStats>>();
        auto stats = std::make_shared<BundleActivePeriodStats>();
        auto packageStat = std::make_shared<BundleActivePackageStats>();
        stats->bundleStats_.emplace("normal", packageStat);
        packageStat = nullptr;
        stats->bundleStats_.emplace("default", packageStat);
        int64_t beginTime = GetData<int64_t>();
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

    OHOS::DeviceUsageStats::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::DeviceUsageStats::BundleActiveClientFuzzTest(data, size);
    OHOS::DeviceUsageStats::BundleActiveEventListFuzzTest(data, size);
    OHOS::DeviceUsageStats::BundleActiveStatsCombinerFuzzTest(data, size);
    return 0;
}
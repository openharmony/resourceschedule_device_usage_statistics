/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "bundleactiveparallelization_fuzzer.h"

#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "bundle_active_service.h"
#include "app_group_callback_stub.h"
#include "bundle_active_event_list.h"

#include "fuzzer/FuzzedDataProvider.h"
#include "token_setproc.h"

auto bundleActiveService = std::make_shared<OHOS::DeviceUsageStats::BundleActiveService>();
namespace OHOS {
namespace DeviceUsageStats {
    static std::string g_defaultBundleName = "com.ohos.camera";
    static std::string g_defaultMoudleName = "defaultmoudlename";
    static std::string g_defaultFormName = "defaultformname";
    static int32_t DEFAULT_DIMENSION = 4;
    static int64_t DEFAULT_FORMID = 1;

    bool ReportEventFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        BundleActiveEvent event(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
            DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
        bundleActiveService->ReportEvent(event, userId);
        return true;
    }

    bool IsBundleIdleFuzzTest(FuzzedDataProvider *provider)
    {
        bool result = false;
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::string inputBundleName = provider->ConsumeRandomLengthString();
        bundleActiveService->IsBundleIdle(result, inputBundleName, userId);
        return true;
    }

    bool IsBundleUsePeriodFuzzTest(FuzzedDataProvider *provider)
    {
        bool result = false;
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::string inputBundleName = provider->ConsumeRandomLengthString();
        bundleActiveService->IsBundleUsePeriod(result, inputBundleName, userId);
        return true;
    }

    bool QueryBundleStatsInfoByIntervalFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t intervalType = provider->ConsumeIntegral<int32_t>();
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::vector<BundleActivePackageStats> packageStat;
        bundleActiveService->QueryBundleStatsInfoByInterval(packageStat,
            intervalType, beginTime, endTime, userId);
        return true;
    }

    bool QueryBundleEventsFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActiveEvent> bundleActiveEvent;
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::vector<BundleActivePackageStats> packageStat;
        bundleActiveService->QueryBundleEvents(bundleActiveEvent,
            beginTime, endTime, userId);
        return true;
    }

    bool QueryBundleStatsInfosFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t intervalType = provider->ConsumeIntegral<int32_t>();
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        std::vector<BundleActivePackageStats> packageStat;
        bundleActiveService->QueryBundleStatsInfos(packageStat,
            intervalType, beginTime, endTime);
        return true;
    }

    bool QueryHighFrequencyUsageBundleInfosFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActivePackageStats> packageStat;
        int32_t maxNum = provider->ConsumeIntegral<int32_t>();
        constexpr int32_t defaultNum = 20;
        maxNum = maxNum > 0 ? maxNum : defaultNum;
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->QueryHighFrequencyUsageBundleInfos(packageStat,
            userId, maxNum);
        return true;
    }

    bool QueryCurrentBundleEventsFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActiveEvent> events;
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        bundleActiveService->QueryCurrentBundleEvents(events, beginTime, endTime);
        return true;
    }

    bool QueryAppGroupFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t appGroup = 0;
        std::string bundleName = provider->ConsumeRandomLengthString();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->QueryAppGroup(appGroup, bundleName, userId);
        return true;
    }

    bool SetAppGroupFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t newGroup = provider->ConsumeIntegral<int32_t>();
        std::string bundleName = provider->ConsumeRandomLengthString();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->SetAppGroup(newGroup, bundleName, userId);
        return true;
    }

    bool QueryModuleUsageRecordsFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t maxNum = provider->ConsumeIntegral<int32_t>();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::vector<BundleActiveModuleRecord> bundleActiveModuleRecord;
        bundleActiveService->QueryModuleUsageRecords(maxNum, bundleActiveModuleRecord, userId);
        return true;
    }

    class TestAppGroupChangeCallback : public AppGroupCallbackStub {
    public:
        ErrCode OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo) override;
    }

    ErrCode OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
    {
        return ERR_OK;
    }

    sptr<IAppGroupCallback> GetMockAppGroupChangeCallback() {
        sptr<TestAppGroupChangeCallback> impl =
            sptr<TestAppGroupChangeCallback>::MakeSptr();
        return impl;
    }

    bool RegisterAppGroupCallBackFuzzTest(FuzzedDataProvider *provider)
    {
        sptr<IAppGroupCallback> appGroupCallback = GetMockAppGroupChangeCallback();
        bundleActiveService->RegisterAppGroupCallBack(appGroupCallback);
        return true;
    }

    bool UnRegisterAppGroupCallBackFuzzTest(FuzzedDataProvider *provider)
    {
        sptr<IAppGroupCallback> appGroupCallback = GetMockAppGroupChangeCallback();
        bundleActiveService->UnRegisterAppGroupCallBack(appGroupCallback);
        return true;
    }

    bool QueryDeviceEventStatsFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActiveEventStats> eventStats;
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);
        return true;
    }

    bool QueryNotificationEventStatsFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActiveEventStats> eventStats;
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);
        return true;
    }

    bool QueryHighFrequencyPeriodBundleFuzzTest(FuzzedDataProvider *provider)
    {
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
        bundleActiveService->QueryHighFrequencyPeriodBundle(appFreqHours, userId);
        return true;
    }

    bool QueryBundleTodayLatestUsedTimeFuzzTest(FuzzedDataProvider *provider)
    {
        int64_t latestUsedTime;
        std::string bundleName = provider->ConsumeRandomLengthString();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
        bundleActiveService->QueryBundleTodayLatestUsedTime(latestUsedTime,
            bundleName, userId);
        return true;
    }
} // namespace DeviceUsageStats
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    if (SetSelfTokenID(718336240ull | (1ull << 32)) < 0) {
        return -1;
    }
    bundleActiveService->OnStart();
    return 0;
}

extern "C" int FuzzIBundleActiveService(FuzzedDataProvider &provider)
{
    static const int fuzzcodes[] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16
    };
    int code = provider.PickValueInArray(fuzzcodes);
    switch (code) {
        case 0: {
            OHOS::DeviceUsageStats::ReportEventFuzzTest(&provider);
            break;
        }
        case 1: {
            OHOS::DeviceUsageStats::IsBundleIdleFuzzTest(&provider);
            break;
        }
        case 2: {
            OHOS::DeviceUsageStats::IsBundleUsePeriodFuzzTest(&provider);
            break;
        }
        case 3: {
            OHOS::DeviceUsageStats::QueryBundleStatsInfoByIntervalFuzzTest(&provider);
            break;
        }
        case 4: {
            OHOS::DeviceUsageStats::QueryBundleEventsFuzzTest(&provider);
            break;
        }
        case 5: {
            OHOS::DeviceUsageStats::QueryBundleStatsInfosFuzzTest(&provider);
            break;
        }
        case 6: {
            OHOS::DeviceUsageStats::QueryHighFrequencyUsageBundleInfosFuzzTest(&provider);
            break;
        }
        case 7: {
            OHOS::DeviceUsageStats::QueryCurrentBundleEventsFuzzTest(&provider);
            break;
        }
        case 8: {
            OHOS::DeviceUsageStats::QueryAppGroupFuzzTest(&provider);
            break;
        }
        case 9: {
            OHOS::DeviceUsageStats::SetAppGroupFuzzTest(&provider);
            break;
        }
        case 10: {
            OHOS::DeviceUsageStats::QueryModuleUsageRecordsFuzzTest(&provider);
            break;
        }
        case 11: {
            OHOS::DeviceUsageStats::RegisterAppGroupCallBackFuzzTest(&provider);
            break;
        }
        case 12: {
            OHOS::DeviceUsageStats::UnRegisterAppGroupCallBackFuzzTest(&provider);
            break;
        }
        case 13: {
            OHOS::DeviceUsageStats::QueryDeviceEventStatsFuzzTest(&provider);
            break;
        }
        case 14: {
            OHOS::DeviceUsageStats::QueryNotificationEventStatsFuzzTest(&provider);
            break;
        }
        case 15: {
            OHOS::DeviceUsageStats::QueryHighFrequencyPeriodBundleFuzzTest(&provider);
            break;
        }
        case 16: {
            OHOS::DeviceUsageStats::QueryBundleTodayLatestUsedTimeFuzzTest(&provider);
            break;
        }
        default:
            break;
    }
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    FuzzIBundleActiveService(fdp);
    return 0;
}

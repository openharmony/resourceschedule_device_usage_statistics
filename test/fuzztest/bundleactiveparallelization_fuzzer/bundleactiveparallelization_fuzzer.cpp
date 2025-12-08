/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
    constexpr int32_t FUZZ_CODE_0 = 0;
    constexpr int32_t FUZZ_CODE_1 = 1;
    constexpr int32_t FUZZ_CODE_2 = 2;
    constexpr int32_t FUZZ_CODE_3 = 3;
    constexpr int32_t FUZZ_CODE_4 = 4;
    constexpr int32_t FUZZ_CODE_5 = 5;
    constexpr int32_t FUZZ_CODE_6 = 6;
    constexpr int32_t FUZZ_CODE_7 = 7;
    constexpr int32_t FUZZ_CODE_8 = 8;
    constexpr int32_t FUZZ_CODE_9 = 9;
    constexpr int32_t FUZZ_CODE_10 = 10;
    constexpr int32_t FUZZ_CODE_11 = 11;
    constexpr int32_t FUZZ_CODE_12 = 12;
    constexpr int32_t FUZZ_CODE_13 = 13;
    constexpr int32_t FUZZ_CODE_14 = 14;
    constexpr int32_t FUZZ_CODE_15 = 15;
    constexpr int32_t FUZZ_CODE_16 = 16;

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
        std::vector<BundleActivePackageStats> packageStats;
        bundleActiveService->QueryBundleStatsInfoByInterval(packageStats,
            intervalType, beginTime, endTime, userId);
        return true;
    }

    bool QueryBundleEventsFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActiveEvent> bundleActiveEvent;
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->QueryBundleEvents(bundleActiveEvent,
            beginTime, endTime, userId);
        return true;
    }

    bool QueryBundleStatsInfosFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActivePackageStats> packageStats;
        int32_t intervalType = provider->ConsumeIntegral<int32_t>();
        int64_t beginTime = provider->ConsumeIntegral<int64_t>();
        int64_t endTime = provider->ConsumeIntegral<int64_t>();
        bundleActiveService->QueryBundleStatsInfos(packageStats,
            intervalType, beginTime, endTime);
        return true;
    }

    bool QueryHighFrequencyUsageBundleInfosFuzzTest(FuzzedDataProvider *provider)
    {
        std::vector<BundleActivePackageStats> packageStats;
        int32_t maxNum = provider->ConsumeIntegral<int32_t>();
        constexpr int32_t defaultNum = 20;
        maxNum = maxNum > 0 ? maxNum : defaultNum;
        int32_t userId = provider->ConsumeIntegral<int32_t>();
        bundleActiveService->QueryHighFrequencyUsageBundleInfos(packageStats,
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
        bundleActiveService->SetAppGroup(bundleName, newGroup, userId);
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
    };

    ErrCode TestAppGroupChangeCallback::OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
    {
        return ERR_OK;
    }

    sptr<IAppGroupCallback> GetMockAppGroupChangeCallback()
    {
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
        bundleActiveService->QueryBundleTodayLatestUsedTime(latestUsedTime,
            bundleName, userId);
        return true;
    }
} // namespace DeviceUsageStats
} // namespace OHOS

extern "C" int FuzzIBundleActiveService(FuzzedDataProvider &provider)
{
    int code = provider.ConsumeIntegralInRange(
        OHOS::DeviceUsageStats::FUZZ_CODE_0, OHOS::DeviceUsageStats::FUZZ_CODE_16);
    switch (code) {
        case OHOS::DeviceUsageStats::FUZZ_CODE_0: {
            OHOS::DeviceUsageStats::ReportEventFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_1: {
            OHOS::DeviceUsageStats::IsBundleIdleFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_2: {
            OHOS::DeviceUsageStats::IsBundleUsePeriodFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_3: {
            OHOS::DeviceUsageStats::QueryBundleStatsInfoByIntervalFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_4: {
            OHOS::DeviceUsageStats::QueryBundleEventsFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_5: {
            OHOS::DeviceUsageStats::QueryBundleStatsInfosFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_6: {
            OHOS::DeviceUsageStats::QueryHighFrequencyUsageBundleInfosFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_7: {
            OHOS::DeviceUsageStats::QueryCurrentBundleEventsFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_8: {
            OHOS::DeviceUsageStats::QueryAppGroupFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_9: {
            OHOS::DeviceUsageStats::SetAppGroupFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_10: {
            OHOS::DeviceUsageStats::QueryModuleUsageRecordsFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_11: {
            OHOS::DeviceUsageStats::RegisterAppGroupCallBackFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_12: {
            OHOS::DeviceUsageStats::UnRegisterAppGroupCallBackFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_13: {
            OHOS::DeviceUsageStats::QueryDeviceEventStatsFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_14: {
            OHOS::DeviceUsageStats::QueryNotificationEventStatsFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_15: {
            OHOS::DeviceUsageStats::QueryHighFrequencyPeriodBundleFuzzTest(&provider);
            break;
        }
        case OHOS::DeviceUsageStats::FUZZ_CODE_16: {
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

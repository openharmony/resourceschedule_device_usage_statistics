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

#include "bundleactivecommon_fuzzer.h"

#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "bundle_active_service.h"
#include "bundle_active_service_proxy.h"
#include "bundle_active_service_stub.h"
#include "bundle_active_client.h"
#include "iapp_group_callback.h"
#include "bundle_active_config_reader.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
namespace DeviceUsageStats {
    using HapModuleInfo = OHOS::AppExecFwk::HapModuleInfo;
    using AbilityInfo = OHOS::AppExecFwk::AbilityInfo;
    using ApplicationInfo = OHOS::AppExecFwk::ApplicationInfo;
    const char* CONFIG_TEST_PATH = "/sys_prod/etc/device_usage_statistics/device_usage_statistics_config.json";
    const char* CONFIG_TEST1_PATH = "/sys_prod/etc/device_usage_statistics/device_usage_statistics_config_test1.json";
    const char* CONFIG_TEST2_PATH = "/sys_prod/etc/device_usage_statistics/device_usage_statistics_config_test2.json";
    const char* CONFIG_TEST3_PATH = "/sys_prod/etc/device_usage_statistics/device_usage_statistics_config_test3.json";
    const char* CONFIG_TEST4_PATH = "/sys_prod/etc/device_usage_statistics/device_usage_statistics_config_test4.json";
    const int32_t MAX_CODE = 20;
    const int32_t INTERVAL_TYPE_COUNT = 5;
    const int32_t APP_TYPE_COUNT = 6;
    const int32_t APP_TYPE_INTERVAL = 10;
    bool BundleActiveConfigReaderTest(FuzzedDataProvider* fdp)
    {
        auto bundleActiveConfigReader = std::make_shared<BundleActiveConfigReader>();
        std::string filePath = fdp->ConsumeRandomLengthString();
        bundleActiveConfigReader->LoadConfigFile(filePath.c_str());
        cJSON *root = nullptr;
        bundleActiveConfigReader->GetJsonFromFile(filePath.c_str(), root);
        bundleActiveConfigReader->GetJsonFromFile(CONFIG_TEST_PATH, root);
        std::string fullPath = "";
        bundleActiveConfigReader->ConvertFullPath(filePath, fullPath);
        bundleActiveConfigReader->ConvertFullPath(CONFIG_TEST_PATH, fullPath);
        return true;
    }

    bool BundleActiveServiceTest(FuzzedDataProvider* fdp)
    {
        auto bundleActiveService = std::make_shared<BundleActiveService>();
        int32_t uid = fdp->ConsumeIntegral<int32_t>();
        bundleActiveService->GetNameAndIndexForUid(uid);
        BundleActiveEvent event;
        int32_t userId = fdp->ConsumeIntegral<int32_t>();
        bundleActiveService->ReportEvent(event, userId);
        bool isBundleIdle = false;
        std::string bundleName = fdp->ConsumeRandomLengthString();
        bundleActiveService->IsBundleIdle(isBundleIdle, bundleName, userId);
        bool isBundleUserPeriod = false;
        bundleActiveService->IsBundleUsePeriod(isBundleUserPeriod, bundleName, userId);
        std::vector<BundleActivePackageStats> packageStats;
        int32_t intervalType = fdp->ConsumeIntegral<int32_t>();
        intervalType = intervalType % INTERVAL_TYPE_COUNT;
        int64_t beginTime = fdp->ConsumeIntegral<int64_t>();
        int64_t endTime = fdp->ConsumeIntegral<int64_t>();
        bundleActiveService->QueryBundleStatsInfoByInterval(packageStats, intervalType, beginTime, endTime, userId);
        BundleActiveEventVecRawData bundleActiveEventVecRawData;
        bundleActiveService->QueryBundleEvents(bundleActiveEventVecRawData, beginTime, endTime, userId);
        std::vector<BundleActiveEventStats> eventStats;
        eventStats.clear();
        int32_t newGroup = fdp->ConsumeIntegral<int32_t>();
        newGroup = (newGroup % APP_TYPE_COUNT + 1) * APP_TYPE_INTERVAL;
        bundleActiveService->SetAppGroup(bundleName, newGroup, userId);
        packageStats.clear();
        bundleActiveService->QueryBundleStatsInfos(packageStats, intervalType, beginTime, endTime);
        bundleActiveService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);
        BundleActiveModuleRecord moduleRecord;
        moduleRecord.bundleName_ = fdp->ConsumeRandomLengthString();
        moduleRecord.userId_ = userId;
        bundleActiveService->QueryModuleRecordInfos(moduleRecord);
        HapModuleInfo hapModuleInfo;
        ApplicationInfo appInfo;
        AbilityInfo abilityInfo;
        bundleActiveService->SerModuleProperties(hapModuleInfo, appInfo, abilityInfo, moduleRecord);
        int32_t fd = fdp->ConsumeIntegral<int32_t>();
        std::vector<std::u16string> args;
        bundleActiveService->Dump(fd, args);
        uint32_t fuzzCode = fdp->ConsumeIntegral<uint32_t>();
        std::string data = fdp->ConsumeRandomLengthString();
        MessageParcel fuzzData;
        fuzzData.WriteInterfaceToken(BundleActiveServiceStub::GetDescriptor());
        fuzzData.WriteBuffer(data.c_str(), data.size());
        fuzzData.RewindRead(0);
        MessageParcel fuzzReply;
        MessageOption fuzzOption;
        bundleActiveService->OnRemoteRequest(fuzzCode % MAX_CODE, fuzzData, fuzzReply, fuzzOption);
        std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
        bundleActiveService->QueryHighFrequencyPeriodBundle(appFreqHours, userId);
        return true;
    }

    bool BundleActiveServiceOtherTest(FuzzedDataProvider* fdp)
    {
        auto bundleActiveService = std::make_shared<BundleActiveService>();
        BundleActiveEvent event;
        int32_t userId = fdp->ConsumeIntegral<int32_t>();
        int32_t maxNum = fdp->ConsumeIntegral<int32_t>();
        constexpr int32_t defaultNum = 20;
        maxNum = maxNum > 0 ? maxNum : defaultNum;
        std::string bundleName = fdp->ConsumeRandomLengthString();
        std::vector<BundleActivePackageStats> packageStats;
        int32_t intervalType = fdp->ConsumeIntegral<int32_t>();
        intervalType = intervalType % INTERVAL_TYPE_COUNT;
        int64_t beginTime = fdp->ConsumeIntegral<int64_t>();
        int64_t endTime = fdp->ConsumeIntegral<int64_t>();
        std::vector<BundleActiveEvent> events;
        std::vector<BundleActiveEventStats> eventStats;
        BundleActiveEventVecRawData bundleActiveEventVecRawData;
        bundleActiveService->QueryCurrentBundleEvents(bundleActiveEventVecRawData, beginTime, endTime);
        bundleActiveEventVecRawData.Unmarshalling(events);
        eventStats.clear();
        int32_t appGroup = 0;
        bundleActiveService->QueryAppGroup(appGroup, bundleName, userId);
        packageStats.clear();
        bundleActiveService->QueryHighFrequencyUsageBundleInfos(packageStats, userId, maxNum);
        packageStats.clear();
        bundleActiveService->QueryBundleStatsInfos(packageStats, intervalType, beginTime, endTime);
        bundleActiveService->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);
        BundleActivePackageStats packageStats1;
        packageStats1.bundleName_ = bundleName;
        BundleActivePackageStats packageStats2;
        packageStats2.bundleName_ = bundleName;
        BundleActivePackageStats packageStats3;
        packageStats3.bundleName_ = fdp->ConsumeRandomLengthString();
        bundleActiveService->MergeSamePackageStats(packageStats1, packageStats2);
        bundleActiveService->MergeSamePackageStats(packageStats1, packageStats3);
        return true;
    }

    bool BundleActiveServiceProxyTest(FuzzedDataProvider* fdp)
    {
        DelayedSingleton<BundleActiveClient>::GetInstance()->GetBundleActiveProxy();
        auto bundleActiveServiceProxy = DelayedSingleton<BundleActiveClient>::GetInstance()->bundleActiveProxy_;
        if (bundleActiveServiceProxy == nullptr) {
            return true;
        }
        std::vector<BundleActivePackageStats> bundleActivePackageStats;
        int32_t intervalType = fdp->ConsumeIntegral<int32_t>();
        intervalType = intervalType % INTERVAL_TYPE_COUNT;
        int64_t beginTime = fdp->ConsumeIntegral<int64_t>();
        int64_t endTime = fdp->ConsumeIntegral<int64_t>();
        bundleActiveServiceProxy->QueryBundleStatsInfos(bundleActivePackageStats, intervalType, beginTime, endTime);
        std::vector<BundleActiveEvent> bundleActiveEvent;
        BundleActiveEventVecRawData bundleActiveEventVecRawData;
        bundleActiveServiceProxy->QueryCurrentBundleEvents(bundleActiveEventVecRawData, beginTime, endTime);
        bundleActiveEventVecRawData.Unmarshalling(bundleActiveEvent);
        int32_t appGroup = fdp->ConsumeIntegral<int32_t>();
        std::string bundleName = fdp->ConsumeRandomLengthString();
        int32_t userId = fdp->ConsumeIntegral<int32_t>();
        bundleActiveServiceProxy->QueryAppGroup(appGroup, bundleName, userId);
        int32_t maxNum = fdp->ConsumeIntegral<int32_t>();
        std::vector<BundleActiveModuleRecord> bundleActiveModuleRecord;
        bundleActiveServiceProxy->QueryModuleUsageRecords(maxNum, bundleActiveModuleRecord, userId);
        return true;
    }
} // namespace DeviceUsageStats
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    OHOS::DeviceUsageStats::BundleActiveConfigReaderTest(&fdp);
    OHOS::DeviceUsageStats::BundleActiveServiceTest(&fdp);
    OHOS::DeviceUsageStats::BundleActiveServiceOtherTest(&fdp);
    OHOS::DeviceUsageStats::BundleActiveServiceProxyTest(&fdp);
    return 0;
}


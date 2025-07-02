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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_IDL_COMMON_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_IDL_COMMON_H

#include "ohos.resourceschedule.usageStatistics.proj.hpp"
#include "ohos.resourceschedule.usageStatistics.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "bundle_active_client.h"
#include <map>
namespace OHOS {
namespace DeviceUsageStats {
class BundleStateIDLCommon {
public:
    static std::shared_ptr<std::map<std::string, BundleActivePackageStats>> QueryBundleStatsInfos(
        int64_t& beginTime, int64_t& endTime, int32_t& errCode);
    static void ParseBundleStatsInfo(const BundleActivePackageStats& bundleActivePackageStats,
        ::ohos::resourceschedule::usageStatistics::BundleStatsInfo& bundleStatsInfo);
    static void ParseBundleEvents(const BundleActiveEvent& bundleActiveEvent,
        ::ohos::resourceschedule::usageStatistics::BundleEvents& bundleEvent);
    static void MergePackageStats(BundleActivePackageStats& left, const BundleActivePackageStats& right);
    static void ParseQueryInfosMap(const taihe::map<taihe::string, taihe::array<double>>& appInfos,
        std::map<std::string, std::vector<int64_t>>& queryInfos);
    static taihe::array<::ohos::resourceschedule::usageStatistics::HapFormInfo> ParseformRecords(
        const std::vector<BundleActiveFormRecord>& formRecords);
    static std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> QueryAppStatsInfos(
        int64_t& beginTime, int64_t& endTime, int32_t& errCode);
    static std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> QueryLastUseTime(
        const std::map<std::string, std::vector<int64_t>>& queryInfos, int32_t& errCode);
    static void MergeAppStatsInfos(const std::vector<BundleActivePackageStats>& packageStats,
            std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>>& mergedPackageStats);
};

}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_IDL_COMMON_H
 
 
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

#include "bundle_active_stats_combiner.h"

namespace OHOS {
namespace DeviceUsageStats {
void BundleActiveStatsCombiner<BundleActivePackageStats>::combine(
    const std::shared_ptr<BundleActivePeriodStats>& stats, std::vector<BundleActivePackageStats>& accumulatedResult,
    int64_t beginTime)
{
    BUNDLE_ACTIVE_LOGI("BundleActivePackageStats combine called");
    for (auto it : stats->bundleStats_) {
        if (it.second != nullptr) {
            accumulatedResult.push_back(*(it.second));
        }
    }
}

void BundleActiveStatsCombiner<BundleActiveEvent>::combine(const std::shared_ptr<BundleActivePeriodStats>& stats,
    std::vector<BundleActiveEvent>& accumulatedResult, int64_t beginTime)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveEvent combine called");
    int32_t startIndex = stats->events_.FindBestIndex(beginTime);
    int32_t size = static_cast<int32_t>(stats->events_.events_.size());
    for (int32_t i = startIndex; i < size; i++) {
        accumulatedResult.push_back(stats->events_.events_[i]);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


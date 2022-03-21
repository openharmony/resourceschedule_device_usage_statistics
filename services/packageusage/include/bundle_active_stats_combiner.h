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

#ifndef BUNDLE_ACTIVE_STATS_COMBINER_H
#define BUNDLE_ACTIVE_STATS_COMBINER_H

#include <vector>

#include "bundle_active_period_stats.h"

namespace OHOS {
namespace DeviceUsageStats {
template <typename T>
class BundleActiveStatsCombiner {
public:
    void combine(const std::shared_ptr<BundleActivePeriodStats>& stats, std::vector<T>& accumulatedResult,
        int64_t beginTime = 0);
};

template <> class BundleActiveStatsCombiner<BundleActivePackageStats> {
public:
    void combine(const std::shared_ptr<BundleActivePeriodStats>& stats,
        std::vector<BundleActivePackageStats>& accumulatedResult, int64_t beginTime = 0);
};

template <> class BundleActiveStatsCombiner<BundleActiveEvent> {
public:
    void combine(const std::shared_ptr<BundleActivePeriodStats>& stats,
        std::vector<BundleActiveEvent>& accumulatedResult, int64_t beginTime = 0);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_STATS_COMBINER_H


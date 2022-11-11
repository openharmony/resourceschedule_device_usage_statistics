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
#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_CONDITION_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_CONDITION_H

namespace OHOS {
namespace DeviceUsageStats {
class BundleStateCondition {
public:
    virtual ~BundleStateCondition() = default;

    enum IntervalType {
        BY_OPTIMIZED = 0,
        BY_DAILY,
        BY_WEEKLY,
        BY_MONTHLY,
        BY_ANNUALLY,
    };

    enum GroupType {
        ACTIVE_GROUP_ALIVE = 10,
        ACTIVE_GROUP_DAILY = 20,
        ACTIVE_GROUP_FIXED = 30,
        ACTIVE_GROUP_RARE = 40,
        ACTIVE_GROUP_LIMIT = 50,
        ACTIVE_GROUP_NEVER = 60,
    };

    enum AppGroupType {
        ALIVE_GROUP = 10,
        DAILY_GROUP = 20,
        FIXED_GROUP = 30,
        RARE_GROUP = 40,
        LIMITED_GROUP = 50,
        NEVER_GROUP = 60,
    };
};
} // namespace DeviceUsageStats
} // namespace OHOS
#endif // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_CONDITION_H


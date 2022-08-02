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

#ifndef BUNDLE_ACTIVE_GROUP_MAP_H
#define BUNDLE_ACTIVE_GROUP_MAP_H
#include "bundle_active_group_common.h"

namespace OHOS {
namespace DeviceUsageStats {
namespace DeviceUsageStatsGroupMap {
    std::map<int32_t, int64_t> groupIntervalMap_ = {
        {DeviceUsageStatsGroupConst::ACTIVE_GROUP_FORCED_SET, 0},
        {DeviceUsageStatsGroupConst::ACTIVE_GROUP_ALIVE, DeviceUsageStatsGroupConst::TWO_HOUR},
        {DeviceUsageStatsGroupConst::ACTIVE_GROUP_DAILY, 2 * DeviceUsageStatsGroupConst::TWO_HOUR},
        {DeviceUsageStatsGroupConst::ACTIVE_GROUP_FIXED, DeviceUsageStatsGroupConst::TWENTY_FOUR_HOUR},
        {DeviceUsageStatsGroupConst::ACTIVE_GROUP_RARE, DeviceUsageStatsGroupConst::FOURTY_EIGHT_HOUR},
    };
}
}
}
#endif  // BUNDLE_ACTIVE_GROUP_MAP_H
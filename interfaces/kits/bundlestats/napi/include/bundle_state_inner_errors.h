/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INNER_ERRORS_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INNER_ERRORS_H

#include "errors.h"

namespace OHOS {
namespace DeviceUsageStats {
/**
 * ErrCode layout
 *
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * | Bit |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |Field|Reserved|        Subsystem      |  Module      |                              Code             |
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 */

// DeviceUsageStats's module const defined.
enum : int32_t {
    DEVICE_USAGE_STATS_MODULE_COMMON = 0x01,
};

// Offset of device usage stats sub-system's errCode base, number : 39911424.
constexpr ErrCode DEVICE_USAGE_STATS_COMMON_ERR_OFFSET =
    ErrCodeOffset(SUBSYS_IAWARE, DEVICE_USAGE_STATS_MODULE_COMMON);
// Device Usage Stats Common Error Code Defined.
enum : int32_t {
    ERR_USAGE_STATS_BUNDLENAME_EMPTY = DEVICE_USAGE_STATS_COMMON_ERR_OFFSET + 1,
    ERR_USAGE_STATS_BUNDLENAME_TYPE,
    ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR,
    ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED,
    ERR_USAGE_STATS_BEGIN_TIME_INVALID,
    ERR_USAGE_STATS_END_TIME_INVALID,
    ERR_USAGE_STATS_TIME_INTERVAL,
    ERR_USAGE_STATS_INTERVAL_TYPE,
    ERR_USAGE_STATS_INTERVAL_NUMBER,
    ERR_USAGE_STATS_GROUP_INVALID,
    ERR_MODULE_STATS_MAXNUM_INVALID,
    ERR_EVENT_TYPE,
    ERR_EVENT_TYPE_NUMBER,
    ERR_SERVICE_FAILED,
    ERR_REPEAT_OPERATION,
    ERR_REGISTER_OBSERVER_IS_NULL,
    ERR_OBSERVER_CALLBACK_IS_INVALID,
    ERR_USAGE_STATS_METHOD_CALLED_FAILED,
    ERR_USAGE_STATS_INVALID_PARAM,
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INNER_ERRORS_H


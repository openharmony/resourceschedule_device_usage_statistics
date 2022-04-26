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

#ifndef BUNDLE_ACTIVE_GROUP_COMMON_H
#define BUNDLE_ACTIVE_GROUP_COMMON_H

namespace OHOS {
namespace DeviceUsageStats {
namespace DeviceUsageStatsGroupConst {
const int32_t ACTIVE_GROUP_UNKNOWN = -1;
const int32_t ACTIVE_GROUP_FORCED_SET = 5;
const int32_t ACTIVE_GROUP_ALIVE = 10;
const int32_t ACTIVE_GROUP_DAILY = 20;
const int32_t ACTIVE_GROUP_FIXED = 30;
const int32_t ACTIVE_GROUP_RARE = 40;
const int32_t ACTIVE_GROUP_LIMIT = 50;
const int32_t ACTIVE_GROUP_NEVER = 60;
const int64_t FIVE_SECOND = 5 * 1000LL;
const int64_t ONE_MINUTE = 60 * 1000LL;
const int64_t TWO_MINUTE = 2 * ONE_MINUTE;
const int64_t THREE_MINUTE = 3 * ONE_MINUTE;
const int64_t FOUR_MINUTE = 4 * ONE_MINUTE;
const int64_t TEN_MINUTE = 10 * ONE_MINUTE;
const int64_t SIXTEEN_MINUTE = 16 * ONE_MINUTE;
const int64_t ONE_HOUR = 60 * ONE_MINUTE;
const int64_t TWO_HOUR = 2 * ONE_HOUR;
const int64_t THREE_HOUR = 3 * ONE_HOUR;
const int64_t TWELVE_HOUR = 12 * ONE_HOUR;
const int64_t TWENTY_FOUR_HOUR = 24 * ONE_HOUR;
const int64_t FOURTY_EIGHT_HOUR = 48 * ONE_HOUR;
const int64_t DEFAULT_EVENT_TIMEOUT = ONE_HOUR;
const int64_t DEFAULT_NOTIFY_EVENT_TIMEOUT = 12 * ONE_HOUR;
const int64_t DEFAULT_SYSTEevent__TIMEOUT = 2 * ONE_HOUR;
const int64_t DEFAULT_LONT_TIME_TASK_START_EVENT_TIMEOUT = 30 * ONE_MINUTE;
const uint32_t GROUP_CONTROL_REASON_MASK = 0xFF00;
const uint32_t GROUP_CONTROL_REASON_DEFAULT = 0x0100;
const uint32_t GROUP_CONTROL_REASON_TIMEOUT = 0x0200;
const uint32_t GROUP_CONTROL_REASON_USAGE = 0x0300;
const uint32_t GROUP_CONTROL_REASON_FORCED = 0x0400;
const uint32_t GROUP_CONTROL_REASON_CALCULATED = 0x0500;
const uint32_t GROUP_EVENT_REASON_MASK = 0x00FF;
const uint32_t GROUP_EVENT_REASON_SYSTEM = 0x0001;
const uint32_t GROUP_EVENT_REASON_NOTIFY_SEEN = 0x0002;
const uint32_t GROUP_EVENT_REASON_USER_INTERACTION = 0x0003;
const uint32_t GROUP_EVENT_REASON_FOREGROUND = 0x0004;
const uint32_t GROUP_EVENT_REASON_BACKGROUND = 0x0005;
const uint32_t GROUP_EVENT_REASON_ALIVE_TIMEOUT = 0x0006;
const uint32_t GROUP_EVENT_REASON_LONG_TIME_TASK_STARTTED = 0x0007;
const uint32_t GROUP_EVENT_REASON_CALCULATED_RESTORED = 0x0008;
const uint32_t GROUP_EVENT_REASON_ALIVE_NOT_TIMEOUT = 0x0009;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_GROUP_COMMON_H


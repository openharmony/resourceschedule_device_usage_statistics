/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <ctime>
#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
const int32_t MILLISECOND_TO_MICROSECOND = 1000;
const int32_t MILLISECOND_TO_SECOND = 1000;
const int32_t SECOND_TO_MILLISECOND = 1000;
const int64_t ONE_DAY_TIME = 24 * 60 * 60 *1000LL;
const int64_t WEEK_OFFSET = 6LL;
const int64_t DAYS_OF_WEEK = 7LL;
const int32_t PERIOD_DAILY = 0;
const int32_t PERIOD_WEEKLY = 1;
const int32_t PERIOD_MONTHLY = 2;
const int32_t PERIOD_YEARLY = 4;
std::string BundleActiveUtil::GetBundleUsageKey(const std::string &bundleName, const int32_t uid)
{
    return bundleName + std::to_string(uid);
}

int64_t BundleActiveUtil::GetFFRTDelayTime(const int64_t& delayTime)
{
    return delayTime * MILLISECOND_TO_MICROSECOND;
}

int64_t BundleActiveUtil::GetIntervalTypeStartTime(const int64_t& timeStamp, const int32_t& intervalType)
{
    time_t time = timeStamp / MILLISECOND_TO_SECOND;
    std::tm* tm_time = std::localtime(&time);
    tm_time->tm_hour = 0;
    tm_time->tm_min = 0;
    tm_time->tm_sec = 0;
    if (intervalType == PERIOD_WEEKLY) {
        int64_t weekday = tm_time->tm_wday;
        time_t startOfDay = mktime(tm_time) * SECOND_TO_MILLISECOND;
        time_t weekDayTime = (weekday + WEEK_OFFSET) % DAYS_OF_WEEK * ONE_DAY_TIME;
        return startOfDay - weekDayTime;
    }
    switch (intervalType) {
        case PERIOD_DAILY:
            break;
        case PERIOD_MONTHLY:
            tm_time->tm_mday = 1;
            break;
        case PERIOD_YEARLY:
            tm_time->tm_mon = 0;
            tm_time->tm_mday = 1;
            break;
        default:
            break;
    }
    return mktime(tm_time) * SECOND_TO_MILLISECOND;
}

int64_t BundleActiveUtil::GetMonthStartTime(const int64_t& timeStamp)
{
    time_t time = timeStamp / MILLISECOND_TO_SECOND;
    std::tm* tm_time = std::localtime(&time);
    tm_time->tm_mday = 1;
    tm_time->tm_hour = 0;
    tm_time->tm_min = 0;
    tm_time->tm_sec = 0;
    return mktime(tm_time) * SECOND_TO_MILLISECOND;
}

int64_t BundleActiveUtil::GetWeekStartTime(const int64_t& timeStamp)
{
    time_t time = timeStamp / MILLISECOND_TO_SECOND;
    std::tm* tm_time = std::localtime(&time);
    int64_t weekday = tm_time->tm_wday;
    tm_time->tm_hour = 0;
    tm_time->tm_min = 0;
    tm_time->tm_sec = 0;
    time_t startOfDay = mktime(tm_time) * SECOND_TO_MILLISECOND;
    time_t weekDayTime = (weekday -1) * ONE_DAY_TIME;
    return startOfDay - weekDayTime;
}

int64_t BundleActiveUtil::GetDayStartTime(const int64_t& timeStamp)
{
    time_t time = timeStamp / MILLISECOND_TO_SECOND;
    std::tm* tm_time = std::localtime(&time);
    tm_time->tm_hour = 0;
    tm_time->tm_min = 0;
    tm_time->tm_sec = 0;
    return mktime(tm_time) * SECOND_TO_MILLISECOND;
}
} // namespace DeviceUsageStats
}  // namespace OHOS


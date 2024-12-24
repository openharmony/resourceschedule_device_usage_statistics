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
#include <limits>
#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
const int32_t MILLISECOND_TO_MICROSECOND = 1000;
const int32_t MILLISECOND_TO_SECOND = 1000;
const int32_t SECOND_TO_MILLISECOND = 1000;
const int64_t ONE_DAY_TIME = 24 * 60 * 60 *1000LL;
const int64_t WEEK_OFFSET = 6LL;
const int64_t DAYS_OF_WEEK = 7LL;
const int64_t HOUR_OF_MIDNIGHT = 0;
const int64_t MIN_OF_MIDNIGHT = 0;
const int64_t SECOND_TO_MIDNIGHT = 0;
const int64_t STATR_DAY_OF_MON = 1;
const int64_t STATR_MON_OF_YEAR = 0;
const int64_t ERROR_TIME = 0;
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
    if (timeStamp <= 0) {
        return ERROR_TIME;
    }
    time_t time = timeStamp / MILLISECOND_TO_SECOND;
    std::tm* tm_time = std::localtime(&time);
    if (tm_time == nullptr) {
        return ERROR_TIME;
    }
    tm_time->tm_hour = HOUR_OF_MIDNIGHT;
    tm_time->tm_min = MIN_OF_MIDNIGHT;
    tm_time->tm_sec = SECOND_TO_MIDNIGHT;
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
            tm_time->tm_mday = STATR_DAY_OF_MON;
            break;
        case PERIOD_YEARLY:
            tm_time->tm_mon = STATR_MON_OF_YEAR;
            tm_time->tm_mday = STATR_DAY_OF_MON;
            break;
        default:
            break;
    }
    return mktime(tm_time) * SECOND_TO_MILLISECOND;
}

int32_t BundleActiveUtil::StringToInt32(const std::string& str)
{
    char* pEnd = nullptr;
    errno = 0;
    int64_t res = std::strtol(str.c_str(), &pEnd, 10);
    if (errno == ERANGE || pEnd == str.c_str() || *pEnd != '\0' ||
        (res < std::numeric_limits<int32_t>::min()) ||
        res > std::numeric_limits<int32_t>::max()) {
        return 0;
    }
    return static_cast<int32_t>(res);
}

int64_t BundleActiveUtil::StringToInt64(const std::string& str)
{
    char* pEnd = nullptr;
    errno = 0;
    int64_t res = std::strtol(str.c_str(), &pEnd, 10);
    if (errno == ERANGE || pEnd == str.c_str() || *pEnd != '\0') {
        return 0;
    }
    return res;
}
} // namespace DeviceUsageStats
}  // namespace OHOS


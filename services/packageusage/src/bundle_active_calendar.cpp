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

#include "bundle_active_calendar.h"
#include "bundle_active_period_stats.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveCalendar::BundleActiveCalendar(const int64_t timeStamp)
{
    time_ = timeStamp;
    dayMilliseconds_ = ONE_DAY_TIME;
    weekMilliseconds_ = ONE_WEEK_TIME;
    monthMilliseconds_ = ONE_MONTH_TIME;
    yearMilliseconds_ = ONE_YEAR_TIME;
}

void BundleActiveCalendar::TruncateToDay()
{
    time_ -= time_ % dayMilliseconds_;
}

void BundleActiveCalendar::TruncateToWeek()
{
    time_ -= time_ % weekMilliseconds_;
}

void BundleActiveCalendar::TruncateToMonth()
{
    time_ -= time_ % monthMilliseconds_;
}

void BundleActiveCalendar::TruncateToYear()
{
    time_ -= time_ % yearMilliseconds_;
}

void BundleActiveCalendar::IncreaseDays(const int64_t val)
{
    time_ += val * dayMilliseconds_;
}

void BundleActiveCalendar::IncreaseWeeks(const int64_t val)
{
    time_ += val* weekMilliseconds_;
}

void BundleActiveCalendar::IncreaseMonths(const int64_t val)
{
    time_ += val * monthMilliseconds_;
}

void BundleActiveCalendar::IncreaseYears(const int64_t val)
{
    time_ += val * yearMilliseconds_;
}

void BundleActiveCalendar::SetMilliseconds(const int64_t timeStamp)
{
    time_ = timeStamp;
}

int64_t BundleActiveCalendar::GetMilliseconds()
{
    return time_;
}

void BundleActiveCalendar::ChangeToDebug()
{
        dayMilliseconds_ = ONE_DAY_TIME_DEBUG;
        weekMilliseconds_ = ONE_WEEK_TIME_DEBUG;
        monthMilliseconds_ = ONE_MONTH_TIME_DEBUG;
        yearMilliseconds_ = ONE_YEAR_TIME_DEBUG;
}

void BundleActiveCalendar::TruncateTo(int32_t intervalType)
{
    switch (intervalType) {
        case BundleActivePeriodStats::PERIOD_DAILY:
            TruncateToDay();
            break;
        case BundleActivePeriodStats::PERIOD_WEEKLY:
            TruncateToWeek();
            break;
        case BundleActivePeriodStats::PERIOD_MONTHLY:
            TruncateToMonth();
            break;
        case BundleActivePeriodStats::PERIOD_YEARLY:
            TruncateToYear();
            break;
        default:
            break;
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


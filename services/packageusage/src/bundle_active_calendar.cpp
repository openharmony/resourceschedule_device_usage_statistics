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
}

void BundleActiveCalendar::TruncateToDay()
{
    time_ -= time_ % DAY_MILLISECONDS;
}

void BundleActiveCalendar::TruncateToWeek()
{
    time_ -= time_ % WEEK_MILLISECONDS;
}

void BundleActiveCalendar::TruncateToMonth()
{
    time_ -= time_ % MONTH_MILLISECONDS;
}

void BundleActiveCalendar::TruncateToYear()
{
    time_ -= time_ % YEAR_MILLISECONDS;
}

void BundleActiveCalendar::IncreaseDays(const int val)
{
    time_ += val * DAY_MILLISECONDS;
}

void BundleActiveCalendar::IncreaseWeeks(const int val)
{
    time_ += val* WEEK_MILLISECONDS;
}

void BundleActiveCalendar::IncreaseMonths(const int val)
{
    time_ += val * MONTH_MILLISECONDS;
}

void BundleActiveCalendar::IncreaseYears(const int val)
{
    time_ += val * YEAR_MILLISECONDS;
}

void BundleActiveCalendar::SetMilliseconds(const int64_t timeStamp)
{
    time_ = timeStamp;
}

int64_t BundleActiveCalendar::GetMilliseconds()
{
    return time_;
}

void BundleActiveCalendar::TruncateTo(int intervalType)
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
}
}
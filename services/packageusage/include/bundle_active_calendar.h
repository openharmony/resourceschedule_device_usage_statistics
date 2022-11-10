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

#ifndef BUNDLE_ACTIVE_CALENDER_H
#define BUNDLE_ACTIVE_CALENDER_H

#include <cstdint>

#include "bundle_active_constant.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveCalendar {
public:
    static const int64_t ONE_SECOND_MILLISECONDS = 1000;
    int64_t dayMilliseconds_;
    int64_t weekMilliseconds_;
    int64_t monthMilliseconds_;
    int64_t yearMilliseconds_;
    BundleActiveCalendar(const int64_t timeStamp);
    BundleActiveCalendar()
    {
        time_ = 0;
        dayMilliseconds_ = ONE_DAY_TIME;
        weekMilliseconds_ = ONE_WEEK_TIME;
        monthMilliseconds_ = ONE_MONTH_TIME;
        yearMilliseconds_ = ONE_YEAR_TIME;
    }
    void ChangeToDebug();
    void TruncateToDay();
    void TruncateToWeek();
    void TruncateToMonth();
    void TruncateToYear();
    void IncreaseDays(const int64_t val);
    void IncreaseWeeks(const int64_t val);
    void IncreaseMonths(const int64_t val);
    void IncreaseYears(const int64_t val);
    void SetMilliseconds(const int64_t timeStamp);
    int64_t GetMilliseconds();
    void TruncateTo(int32_t intervalType);

private:
    int64_t time_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CALENDER_H


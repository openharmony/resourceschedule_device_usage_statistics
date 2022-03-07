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

#ifndef BUNDLE_ACTIVE_USER_SERVICE_H
#define BUNDLE_ACTIVE_USER_SERVICE_H

#include <memory>

#include "ibundle_active_service.h"
#include "bundle_active_event.h"
#include "bundle_active_period_stats.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_calendar.h"
#include "bundle_active_stats_combiner.h"
#include "bundle_active_usage_database.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveCore;

class BundleActiveUserService {
public:
    BundleActiveUserService() = delete;
    BundleActiveUserService(const int userId, BundleActiveCore& listener):listener_(listener)
    {
        for (int i = 0; i < BundleActivePeriodStats::PERIOD_COUNT; i++) {
            currentStats_.push_back(nullptr);
        }
        userId_ = userId;
        dailyExpiryDate_.SetMilliseconds(0);
        statsChanged_ = false;
    }
    void Init(const int64_t timeStamp);
    ~BundleActiveUserService() {};
    void ReportForShutdown(const BundleActiveEvent& event);
    void ReportEvent(const BundleActiveEvent& event);
    void RestoreStats(bool forced);
    void RenewStatsInMemory(const int64_t timeStamp);
    void RenewTableTime(int64_t oldTime, int64_t newTime);
    void OnUserRemoved();
    void DeleteUninstalledBundleStats(const std::string& bundleName);
    int userId_;
    BundleActiveCalendar dailyExpiryDate_;
    std::vector<BundleActivePackageStats> QueryPackageStats(int intervalType, const int64_t beginTime,
        const int64_t endTime, const int userId, const std::string& bundleName);
    std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime, const int userId,
        const std::string& bundleName);
    void LoadActiveStats(const int64_t timeStamp, const bool& force, const bool& timeChanged);

private:
    static const int64_t ONE_SECOND_MILLISECONDS = 1000;
    BundleActiveUsageDatabase database_;
    std::vector<std::shared_ptr<BundleActivePeriodStats>> currentStats_;
    bool statsChanged_;
    std::string lastBackgroundBundle_;
    BundleActiveCore& listener_;
    inline static const std::vector<int64_t> PERIOD_LENGTH = {BundleActiveCalendar::DAY_MILLISECONDS,
        BundleActiveCalendar::WEEK_MILLISECONDS, BundleActiveCalendar::MONTH_MILLISECONDS,
        BundleActiveCalendar::YEAR_MILLISECONDS};
    void NotifyStatsChanged();
    void NotifyNewUpdate();
    void PrintInMemStats();
};
}
}
#endif
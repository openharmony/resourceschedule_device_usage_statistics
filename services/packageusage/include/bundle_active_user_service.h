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
#include "bundle_active_constant.h"
#include "bundle_active_module_record.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveCore;

class BundleActiveUserService {
public:
    BundleActiveUserService() = delete;
    BundleActiveUserService(const int32_t userId, BundleActiveCore& listener, const bool debug):listener_(listener)
    {
        for (int32_t i = 0; i < BundleActivePeriodStats::PERIOD_COUNT; i++) {
            currentStats_.push_back(nullptr);
        }
        userId_ = userId;
        dailyExpiryDate_.SetMilliseconds(0);
        statsChanged_ = false;
        if (debug) {
            dailyExpiryDate_.ChangeToDebug();
            database_.ChangeToDebug();
            debugUserService_ = true;
            periodLength_ = {ONE_DAY_TIME_DEBUG, ONE_WEEK_TIME_DEBUG, ONE_MONTH_TIME_DEBUG, ONE_YEAR_TIME_DEBUG};
        } else {
            debugUserService_ = false;
            periodLength_ = {ONE_DAY_TIME, ONE_WEEK_TIME, ONE_MONTH_TIME, ONE_YEAR_TIME};
        }
    }
    void Init(const int64_t timeStamp);
    ~BundleActiveUserService() {}
    void ReportForShutdown(const BundleActiveEvent& event);
    void ReportEvent(const BundleActiveEvent& event);
    void ReportFormEvent(const BundleActiveEvent& event);
    void ReportModuleEvent(const BundleActiveEvent& event);
    void RestoreStats(bool forced);
    void RenewStatsInMemory(const int64_t timeStamp);
    void RenewTableTime(int64_t oldTime, int64_t newTime);
    void OnUserRemoved();
    void DeleteUninstalledBundleStats(const std::string& bundleName);
    int32_t userId_;
    BundleActiveCalendar dailyExpiryDate_;
    ErrCode QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& PackageStats, int32_t intervalType,
        const int64_t beginTime, const int64_t endTime, const int32_t userId, const std::string& bundleName);
    ErrCode QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvent, const int64_t beginTime,
        const int64_t endTime, const int32_t userId, const std::string& bundleName);
    int32_t QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results);
    int32_t QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId);
    int32_t QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId);
    void LoadActiveStats(const int64_t timeStamp, const bool& force, const bool& timeChanged);
    void LoadModuleAndFormStats();

private:
    static const int64_t ONE_SECOND_MILLISECONDS = 1000;
    BundleActiveUsageDatabase database_;
    std::vector<std::shared_ptr<BundleActivePeriodStats>> currentStats_;
    std::map<std::string, std::shared_ptr<BundleActiveModuleRecord>> moduleRecords_;
    bool statsChanged_;
    bool debugUserService_;
    std::string lastForegroundBundle_;
    BundleActiveCore& listener_;
    std::vector<int64_t> periodLength_ = {0, 0, 0, 0};
    void NotifyStatsChanged();
    void NotifyNewUpdate();
    std::shared_ptr<BundleActiveModuleRecord> GetOrCreateModuleRecord(const BundleActiveEvent& event);
    void PrintInMemPackageStats(const int32_t idx, const bool debug);
    void PrintInMemEventStats(const bool debug);
    void PrintInMemFormStats(const bool debug, const bool printform);
    void GetCachedSystemEvents(std::shared_ptr<BundleActivePeriodStats> currentStats, int64_t beginTime,
        int64_t endTime, std::map<std::string, BundleActiveEventStats>& systemEventStats);
    void GetCachedNotificationEvents(std::shared_ptr<BundleActivePeriodStats> currentStats, int64_t beginTime,
        int64_t endTime, std::map<std::string, BundleActiveEventStats>& notificationEventStats);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_USER_SERVICE_H


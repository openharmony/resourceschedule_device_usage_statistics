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

#ifndef BUNDLE_ACTIVE_USAGE_DATABASE_H
#define BUNDLE_ACTIVE_USAGE_DATABASE_H

#include <vector>
#include <string>
#include <mutex>

#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_open_callback.h"
#include "rdb_store.h"
#include "rdb_store_config.h"
#include "result_set.h"
#include "values_bucket.h"

#include "bundle_active_period_stats.h"
#include "bundle_active_calendar.h"
#include "bundle_active_package_history.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveUsageDatabase {
public:
    BundleActiveUsageDatabase();
    ~BundleActiveUsageDatabase();
    void InitDatabaseTableInfo(int64_t currentTime);
    void InitUsageGroupInfo(int32_t databaseType);
    void UpdateUsageData(int32_t databaseType, BundleActivePeriodStats &stats);
    std::shared_ptr<BundleActivePeriodStats> GetCurrentUsageData(int32_t databaseType, int userId);
    void RenewTableTime(int64_t timeDiffMillis);
    int32_t GetOptimalIntervalType(int64_t beginTime, int64_t endTime);
    void RemoveOldData(int64_t currentTime);
    std::vector<BundleActivePackageStats> QueryDatabaseUsageStats(int32_t databaseType, int64_t beginTime,
        int64_t endTime, int userId);
    std::vector<BundleActiveEvent> QueryDatabaseEvents(int64_t beginTime, int64_t endTime, int userId,
        std::string bundleName);
    void PutDurationData(int64_t bootBasedDuration, int64_t screenOnDuration);
    std::pair<int64_t, int64_t> GetDurationData();
    void PutBundleHistoryData(int userId, std::shared_ptr<std::map<std::string,
        std::shared_ptr<BundleActivePackageHistory>>> userHistory);
    std::shared_ptr<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>>
        GetBundleHistoryData(int userId);
    void OnPackageUninstalled(const int userId, const std::string& bundleName);

private:
    void CheckDatabaseVersion();
    void HandleTableInfo(unsigned int databaseType);
    std::shared_ptr<NativeRdb::RdbStore> GetBundleActiveRdbStore(unsigned int databaseType);
    int64_t ParseStartTime(const std::string &tableName);
    int32_t NearIndexOnOrAfterCurrentTime(int64_t currentTime, std::vector<int64_t> &sortedTableArray);
    int32_t NearIndexOnOrBeforeCurrentTime(int64_t currentTime, std::vector<int64_t> &sortedTableArray);
    int32_t RenameTableName(unsigned int databaseType, int64_t tableOldTime, int64_t tableNewTime);
    std::string GetTableIndexSql(unsigned int databaseType, int64_t tableTime, bool createFlag);
    void FlushPackageInfo(unsigned int databaseType, const BundleActivePeriodStats &stats);
    void FlushEventInfo(unsigned int databaseType, BundleActivePeriodStats &stats);
    void DeleteExcessiveTableData(unsigned int databaseType);
    int32_t DeleteInvalidTable(unsigned int databaseType, int64_t tableTimeMillis);
    std::unique_ptr<std::vector<int64_t>> GetOverdueTableCreateTime(unsigned int databaseType,
        int64_t currentTimeMillis);
    int32_t CreatePackageLogTable(unsigned int databaseType, int64_t currentTimeMillis);
    int32_t CreateEventLogTable(unsigned int databaseType, int64_t currentTimeMillis);
    int32_t CreateDurationTable(unsigned int databaseType);
    int32_t CreateBundleHistoryTable(unsigned int databaseType);
    std::unique_ptr<NativeRdb::ResultSet> QueryStatsInfoByStep(unsigned int databaseType,
        const std::string &sql, const std::vector<std::string> &selectionArgs);
    void DeleteUninstalledInfo(const int userId, const std::string& bundleName, const std::string& tableName,
        unsigned int databaseType);
    int32_t CreateDatabasePath();

private:
    std::vector<std::string> databaseFiles_;
    std::vector<std::vector<int64_t>> sortedTableArray_;
    std::map<std::string, std::shared_ptr<NativeRdb::RdbStore>> bundleActiveRdbStoreCache_;
    std::shared_ptr<BundleActiveCalendar> calendar_;
    std::string eventTableName_;
    std::string durationTableName_;
    std::string bundleHistoryTableName_;
    std::string versionDirectoryPath_;
    std::string versionFile_;
    uint32_t currentVersion_;
    std::mutex databaseMutex_;
    std::int64_t eventBeginTime_;
};
}
}
#endif
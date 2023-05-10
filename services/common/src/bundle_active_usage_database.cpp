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

#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <map>
#include <limits>
#include <cmath>

#include "time_service_client.h"

#include "bundle_active_constant.h"
#include "bundle_active_open_callback.h"
#include "bundle_active_log.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_binary_search.h"
#include "bundle_active_period_stats.h"
#include "bundle_active_usage_database.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace OHOS::NativeRdb;
using namespace std;

BundleActiveUsageDatabase::BundleActiveUsageDatabase()
{
    currentVersion_ = BUNDLE_ACTIVE_CURRENT_VERSION;
    versionDirectoryPath_ = BUNDLE_ACTIVE_DATABASE_DIR + BUNDLE_ACTIVE_VERSION_FILE;
    for (uint32_t i = 0; i < sizeof(DATABASE_TYPE)/sizeof(DATABASE_TYPE[0]); i++) {
        databaseFiles_.push_back(DATABASE_TYPE[i] + SUFFIX_TYPE[0]);
    }
    eventTableName_ = UNKNOWN_TABLE_NAME;
    durationTableName_ = UNKNOWN_TABLE_NAME;
    bundleHistoryTableName_ = UNKNOWN_TABLE_NAME;
    moduleRecordsTableName_ = UNKNOWN_TABLE_NAME;
    formRecordsTableName_ = UNKNOWN_TABLE_NAME;
    sortedTableArray_ = vector<vector<int64_t>>(SORTED_TABLE_ARRAY_NUMBER);
    calendar_ = make_shared<BundleActiveCalendar>();
    eventBeginTime_ = EVENT_BEGIN_TIME_INITIAL_VALUE;
    debugDatabase_ = false;
}

BundleActiveUsageDatabase::~BundleActiveUsageDatabase()
{
    RdbHelper::ClearCache();
}

void BundleActiveUsageDatabase::ChangeToDebug()
{
    calendar_->ChangeToDebug();
    debugDatabase_ = true;
}

void BundleActiveUsageDatabase::InitUsageGroupDatabase(const int32_t databaseType, const bool forModuleRecords)
{
    lock_guard<mutex> lock(databaseMutex_);
    if (CreateDatabasePath() == BUNDLE_ACTIVE_FAIL) {
        BUNDLE_ACTIVE_LOGE("database path is not exist");
        return;
    }
    if (databaseType != APP_GROUP_DATABASE_INDEX) {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid, databaseType = %{public}d", databaseType);
        return;
    }
    string queryDatabaseTableNames = "select * from sqlite_master where type = ?";
    vector<string> queryCondition;
    queryCondition.push_back(DATABASE_FILE_TABLE_NAME);
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(databaseType,
        queryDatabaseTableNames, queryCondition);
    if (bundleActiveResult == nullptr) {
        BUNDLE_ACTIVE_LOGE("bundleActiveResult is invalid");
        return;
    }
    int32_t tableNumber;
    bundleActiveResult->GetRowCount(tableNumber);
    if (tableNumber == TABLE_NOT_EXIST) {
        BUNDLE_ACTIVE_LOGE("table not exist");
        return;
    }
    int32_t tableNameIndex;
    bundleActiveResult->GetColumnIndex(SQLITE_MASTER_NAME, tableNameIndex);
    string tableName;
    for (int32_t i = 0; i < tableNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetString(tableNameIndex, tableName);
        if (!forModuleRecords) {
            if (DURATION_LOG_TABLE == tableName) {
                durationTableName_ = DURATION_LOG_TABLE;
            } else if (BUNDLE_HISTORY_LOG_TABLE == tableName) {
                bundleHistoryTableName_ = BUNDLE_HISTORY_LOG_TABLE;
            }
        } else {
            if (tableName.find(MODULE_RECORD_LOG_TABLE.c_str()) != tableName.npos) {
                moduleRecordsTableName_ = tableName;
            } else if (tableName.find(FORM_RECORD_LOG_TABLE.c_str()) != tableName.npos) {
                formRecordsTableName_ = tableName;
            }
        }
    }
}

int32_t BundleActiveUsageDatabase::CreateDatabasePath()
{
    if (access(BUNDLE_ACTIVE_DATABASE_DIR.c_str(), F_OK) != 0) {
        int32_t createDir = mkdir(BUNDLE_ACTIVE_DATABASE_DIR.c_str(), S_IRWXU);
        if (createDir != 0) {
            BUNDLE_ACTIVE_LOGE("failed to create directory %{public}s", BUNDLE_ACTIVE_DATABASE_DIR.c_str());
            return BUNDLE_ACTIVE_FAIL;
        }
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

void BundleActiveUsageDatabase::InitDatabaseTableInfo(int64_t currentTime)
{
    lock_guard<mutex> lock(databaseMutex_);
    if (CreateDatabasePath() == BUNDLE_ACTIVE_FAIL) {
        BUNDLE_ACTIVE_LOGE("database path is not exist");
        return;
    }
    CheckDatabaseVersion();
    for (uint32_t i = 0; i < databaseFiles_.size(); i++) {
        HandleTableInfo(i);
        DeleteExcessiveTableData(i);
    }
    for (uint32_t i = 0; i < sortedTableArray_.size(); i++) {
        int32_t startIndex = NearIndexOnOrAfterCurrentTime(currentTime, sortedTableArray_.at(i));
        if (startIndex < BUNDLE_ACTIVE_SUCCESS) {
            continue;
        }
        int32_t tableNumber = static_cast<int32_t>(sortedTableArray_.at(i).size());
        for (int32_t j = startIndex; j < tableNumber; j++) {
            DeleteInvalidTable(i, sortedTableArray_.at(i).at(startIndex));
            sortedTableArray_.at(i).erase(sortedTableArray_.at(i).begin() + startIndex);
        }
    }
    if (eventTableName_ != UNKNOWN_TABLE_NAME) {
        int64_t eventTableTime = ParseStartTime(eventTableName_);
        if (currentTime < eventTableTime) {
            DeleteInvalidTable(EVENT_DATABASE_INDEX, eventTableTime);
        }
    }
}

int32_t BundleActiveUsageDatabase::NearIndexOnOrAfterCurrentTime(int64_t currentTime,
    vector<int64_t> &sortedTableArray)
{
    int32_t low = 0;
    int32_t high = static_cast<int32_t>(sortedTableArray.size() - 1);
    int32_t mid = -1;
    int64_t tableTime = -1;
    int32_t divisor = 2;
    while (low <= high) {
        mid = (high + low) / divisor;
        tableTime = sortedTableArray.at(mid);
        if (currentTime > tableTime) {
            low = mid + 1;
        } else if (currentTime < tableTime) {
            high = mid - 1;
        } else {
            return mid;
        }
    }
    if (currentTime < tableTime) {
        return mid;
    } else if (currentTime > tableTime && low < static_cast<int32_t>(sortedTableArray.size())) {
        return low;
    } else {
        return BUNDLE_ACTIVE_FAIL;
    }
}

int32_t BundleActiveUsageDatabase::NearIndexOnOrBeforeCurrentTime(int64_t currentTime,
    vector<int64_t> &sortedTableArray)
{
    int32_t index = NearIndexOnOrAfterCurrentTime(currentTime, sortedTableArray);
    if (index < 0) {
        return sortedTableArray.size() - 1;
    }
    if (sortedTableArray.at(index) == currentTime) {
        return index;
    }
    return index - 1;
}

unique_ptr<NativeRdb::ResultSet> BundleActiveUsageDatabase::QueryStatsInfoByStep(uint32_t databaseType,
    const string &sql, const vector<string> &selectionArgs)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return nullptr;
    }
    unique_ptr<NativeRdb::ResultSet> result;
    if (selectionArgs.empty()) {
        result = rdbStore->QueryByStep(sql);
    } else {
        result = rdbStore->QueryByStep(sql, selectionArgs);
    }
    return result;
}

void BundleActiveUsageDatabase::HandleTableInfo(uint32_t databaseType)
{
    string queryDatabaseTableNames = "select * from sqlite_master where type = ?";
    vector<string> queryCondition;
    queryCondition.push_back(DATABASE_FILE_TABLE_NAME);
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(databaseType,
        queryDatabaseTableNames, queryCondition);
    if (bundleActiveResult == nullptr) {
        BUNDLE_ACTIVE_LOGE("bundleActiveResult is invalid");
        return;
    }
    int32_t tableNumber;
    bundleActiveResult->GetRowCount(tableNumber);
    if (tableNumber == TABLE_NOT_EXIST) {
        BUNDLE_ACTIVE_LOGE("table not exist");
        return;
    }
    int32_t tableNameIndex;
    bundleActiveResult->GetColumnIndex(SQLITE_MASTER_NAME, tableNameIndex);
    if (databaseType >= 0 && databaseType < sortedTableArray_.size()) {
        if (!sortedTableArray_.at(databaseType).empty()) {
            sortedTableArray_.at(databaseType).clear();
        }
        for (int32_t i = 0; i < tableNumber; i++) {
            string tableName;
            bundleActiveResult->GoToRow(i);
            bundleActiveResult->GetString(tableNameIndex, tableName);
            sortedTableArray_.at(databaseType).push_back(ParseStartTime(tableName));
        }
        if (!sortedTableArray_.at(databaseType).empty()) {
            sort(sortedTableArray_.at(databaseType).begin(), sortedTableArray_.at(databaseType).end());
        }
        if ((databaseType == DAILY_DATABASE_INDEX) && !sortedTableArray_.at(databaseType).empty()) {
            size_t lastTableIndex = sortedTableArray_.at(databaseType).size() - 1;
            eventBeginTime_ = sortedTableArray_.at(databaseType).at(lastTableIndex);
        }
    } else if (databaseType == EVENT_DATABASE_INDEX) {
        if (tableNumber == EVENT_TABLE_NUMBER) {
            bundleActiveResult->GoToRow(tableNumber - EVENT_TABLE_NUMBER);
            bundleActiveResult->GetString(tableNameIndex, eventTableName_);
        }
    }
}

void BundleActiveUsageDatabase::DeleteExcessiveTableData(uint32_t databaseType)
{
    if (databaseType >= 0 && databaseType < sortedTableArray_.size()) {
        if (sortedTableArray_.at(databaseType).empty()) {
            BUNDLE_ACTIVE_LOGE("database table not exist");
            return;
        }
        int32_t existingNumber = static_cast<int32_t>(sortedTableArray_.at(databaseType).size());
        int32_t deleteNumber = existingNumber - MAX_FILES_EVERY_INTERVAL_TYPE[databaseType];
        if (deleteNumber > 0) {
            for (int32_t i = 0; i < deleteNumber; i++) {
                // 删除多余文件
                DeleteInvalidTable(databaseType, sortedTableArray_.at(databaseType).at(0));
                sortedTableArray_.at(databaseType).erase(sortedTableArray_.at(databaseType).begin());
            }
            BUNDLE_ACTIVE_LOGD("BundleActiveUsageDatabase DeleteExcessiveTableData Deleted %{public}d tables from "
                "database type %{public}d", deleteNumber, databaseType);
        }
    } else if (databaseType == EVENT_DATABASE_INDEX) {
        // 删除多余数据
        if ((eventTableName_ == UNKNOWN_TABLE_NAME) || (eventBeginTime_ == EVENT_BEGIN_TIME_INITIAL_VALUE)) {
            return;
        }
        int64_t eventTableTime = ParseStartTime(eventTableName_);
        int64_t deleteTimePoint = 0;
        if (debugDatabase_) {
            deleteTimePoint = eventBeginTime_ - SIX_DAY_IN_MILLIS_MAX_DEBUG - eventTableTime;
        } else {
            deleteTimePoint = eventBeginTime_ - SIX_DAY_IN_MILLIS_MAX - eventTableTime;
        }
        if (deleteTimePoint <= 0) {
            return;
        }
        shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
        if (rdbStore == nullptr) {
            BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
            return;
        }
        string deleteEventDataSql = "delete from " + eventTableName_ + " where timeStamp <= " +
            to_string(deleteTimePoint);
        int32_t deleteResult = rdbStore->ExecuteSql(deleteEventDataSql);
        if (deleteResult != NativeRdb::E_OK) {
            BUNDLE_ACTIVE_LOGE("delete event data failed, rdb error number: %{public}d", deleteResult);
        }
    } else if (databaseType == APP_GROUP_DATABASE_INDEX) {
        // 无数据删除
    } else {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid, databaseType = %{public}u", databaseType);
    }
}

std::unique_ptr<std::vector<int64_t>> BundleActiveUsageDatabase::GetOverdueTableCreateTime(uint32_t databaseType,
    int64_t currentTimeMillis)
{
    std::unique_ptr<std::vector<int64_t>> overdueTableCreateTime = std::make_unique<std::vector<int64_t>>();
    if (databaseType >= sortedTableArray_.size()) {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid, databaseType = %{public}u", databaseType);
        return nullptr;
    }
    string queryDatabaseTableNames = "select * from sqlite_master where type = ?";
    vector<string> queryCondition;
    queryCondition.push_back(DATABASE_FILE_TABLE_NAME);
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(databaseType,
        queryDatabaseTableNames, queryCondition);
    if (bundleActiveResult == nullptr) {
        BUNDLE_ACTIVE_LOGE("bundleActiveResult is invalid");
        return nullptr;
    }
    int32_t tableNumber;
    bundleActiveResult->GetRowCount(tableNumber);
    if (tableNumber == 0) {
        BUNDLE_ACTIVE_LOGE("table does not exist");
        return nullptr;
    }
    int32_t tableNameIndex;
    bundleActiveResult->GetColumnIndex(SQLITE_MASTER_NAME, tableNameIndex);
    string tableName;
    for (int32_t i = 0; i < tableNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetString(tableNameIndex, tableName);
        if (ParseStartTime(tableName) < currentTimeMillis) {
            overdueTableCreateTime->push_back(ParseStartTime(tableName));
        }
    }
    return overdueTableCreateTime;
}

int32_t BundleActiveUsageDatabase::DeleteInvalidTable(uint32_t databaseType, int64_t tableTimeMillis)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    if (databaseType >= 0 && databaseType < sortedTableArray_.size()) {
        string packageTable = PACKAGE_LOG_TABLE + to_string(tableTimeMillis);
        string deletePackageTableSql = "drop table " + packageTable;
        int32_t deletePackageTable = rdbStore->ExecuteSql(deletePackageTableSql);
        if (deletePackageTable != NativeRdb::E_OK) {
            BUNDLE_ACTIVE_LOGE("delete package table failed, rdb error number: %{public}d", deletePackageTable);
            return BUNDLE_ACTIVE_FAIL;
        }
    } else if (databaseType == EVENT_DATABASE_INDEX) {
        string eventTable = EVENT_LOG_TABLE + to_string(tableTimeMillis);
        string deleteEventTableSql = "drop table " + eventTable;
        int32_t deleteEventTable = rdbStore->ExecuteSql(deleteEventTableSql);
        if (deleteEventTable != NativeRdb::E_OK) {
            BUNDLE_ACTIVE_LOGE("delete event table failed, rdb error number: %{public}d", deleteEventTable);
            return BUNDLE_ACTIVE_FAIL;
        }
    } else if (databaseType == APP_GROUP_DATABASE_INDEX) {
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int64_t BundleActiveUsageDatabase::ParseStartTime(const string &tableName)
{
    if (tableName.empty()) {
        int64_t invalidStartTime(BUNDLE_ACTIVE_FAIL);
        return invalidStartTime;
    }
    string tableTime = tableName;
    for (uint32_t i = 0; i < tableTime.length(); i++) {
        if (tableTime[i] >= '0' && tableTime[i] <= '9') {
            tableTime = tableTime.substr(i);
            break;
        }
    }
    return atoll(tableTime.c_str());
}

void BundleActiveUsageDatabase::CheckDatabaseVersion()
{
    if (access(BUNDLE_ACTIVE_DATABASE_DIR.c_str(), F_OK) == 0) {
        ofstream openVersionFile;
        openVersionFile.open(versionDirectoryPath_, ios::out);
        if (openVersionFile) {
            openVersionFile << "version : " << BUNDLE_ACTIVE_CURRENT_VERSION;
        }
        openVersionFile.close();
    }
}

shared_ptr<NativeRdb::RdbStore> BundleActiveUsageDatabase::GetBundleActiveRdbStore(uint32_t databaseType)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore;
    string file = databaseFiles_.at(databaseType);
    if (bundleActiveRdbStoreCache_.find(file) == bundleActiveRdbStoreCache_.end()) {
        int32_t errCode(BUNDLE_ACTIVE_FAIL);
        string currDatabaseFileConfig = BUNDLE_ACTIVE_DATABASE_DIR + databaseFiles_.at(databaseType);
        RdbStoreConfig config(currDatabaseFileConfig);
        BundleActiveOpenCallback rdbDataCallBack;
        config.SetJournalMode(NativeRdb::JournalMode::MODE_OFF);
        config.SetSecurityLevel(NativeRdb::SecurityLevel::S1);
        rdbStore = RdbHelper::GetRdbStore(config, BUNDLE_ACTIVE_RDB_VERSION, rdbDataCallBack, errCode);
        if ((rdbStore == nullptr)) {
            BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
            return nullptr;
        }
        bundleActiveRdbStoreCache_.insert(pair {file, rdbStore});
    } else {
        rdbStore = bundleActiveRdbStoreCache_[file];
    }
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return nullptr;
    }
    return rdbStore;
}

void BundleActiveUsageDatabase::CheckDatabaseFile(uint32_t databaseType)
{
    std::string databaseFileName = databaseFiles_.at(databaseType);
    for (uint32_t i = 0; i < sizeof(SUFFIX_TYPE) / sizeof(SUFFIX_TYPE[0]); i++) {
        std::string dbFile = BUNDLE_ACTIVE_DATABASE_DIR + DATABASE_TYPE[databaseType] + SUFFIX_TYPE[i];
        if ((access(dbFile.c_str(), F_OK) != 0)
            && (bundleActiveRdbStoreCache_.find(databaseFileName) != bundleActiveRdbStoreCache_.end())) {
            bundleActiveRdbStoreCache_.erase(databaseFileName);
            std::string rdbStorePath = BUNDLE_ACTIVE_DATABASE_DIR + DATABASE_TYPE[databaseType] + SUFFIX_TYPE[0];
            RdbHelper::DeleteRdbStore(rdbStorePath);
            if (databaseType >= 0 && databaseType < sortedTableArray_.size()) {
                sortedTableArray_.at(databaseType).clear();
            } else if (databaseType == EVENT_DATABASE_INDEX) {
                eventTableName_ = UNKNOWN_TABLE_NAME;
            } else if (databaseType == APP_GROUP_DATABASE_INDEX) {
                durationTableName_ = UNKNOWN_TABLE_NAME;
                bundleHistoryTableName_ = UNKNOWN_TABLE_NAME;
                moduleRecordsTableName_ = UNKNOWN_TABLE_NAME;
                formRecordsTableName_ = UNKNOWN_TABLE_NAME;
            }
            return;
        }
    }
}

int32_t BundleActiveUsageDatabase::CreateEventLogTable(uint32_t databaseType, int64_t currentTimeMillis)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    string eventTable = EVENT_LOG_TABLE + to_string(currentTimeMillis);
    eventTableName_ = eventTable;
    const string createEventTableSql = "CREATE TABLE IF NOT EXISTS "
                                           + eventTable
                                           + " ("
                                           + BUNDLE_ACTIVE_DB_USER_ID + " INTEGER NOT NULL, "
                                           + BUNDLE_ACTIVE_DB_BUNDLE_NAME + " TEXT NOT NULL, "
                                           + BUNDLE_ACTIVE_DB_EVENT_ID + " INTEGER NOT NULL, "
                                           + BUNDLE_ACTIVE_DB_TIME_STAMP + " INTEGER NOT NULL, "
                                           + BUNDLE_ACTIVE_DB_ABILITY_ID + " TEXT NOT NULL);";
    int32_t createEventTable = rdbStore->ExecuteSql(createEventTableSql);
    if (createEventTable != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create event table failed, rdb error number: %{public}d", createEventTable);
        return createEventTable;
    }
    string createEventTableIndex = GetTableIndexSql(EVENT_DATABASE_INDEX, currentTimeMillis, true);
    int32_t createResult = rdbStore->ExecuteSql(createEventTableIndex);
    if (createResult != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create event table index failed, rdb error number: %{public}d", createResult);
        return BUNDLE_ACTIVE_FAIL;
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::CreatePackageLogTable(uint32_t databaseType, int64_t currentTimeMillis)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    string packageTable = PACKAGE_LOG_TABLE + to_string(currentTimeMillis);
    string createPackageTableSql = "CREATE TABLE IF NOT EXISTS "
                                        + packageTable
                                        + " ("
                                        + BUNDLE_ACTIVE_DB_USER_ID + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_STARTED_COUNT + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_LAST_TIME + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_LAST_TIME_CONTINUOUS_TASK + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_TOTAL_TIME + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_TOTAL_TIME_CONTINUOUS_TASK + " INTEGER NOT NULL);";
    int32_t createPackageTable = rdbStore->ExecuteSql(createPackageTableSql);
    if (createPackageTable != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create packageLog table failed, rdb error number: %{public}d", createPackageTable);
        return BUNDLE_ACTIVE_FAIL;
    }
    string createPackageTableIndex = GetTableIndexSql(databaseType, currentTimeMillis, true);
    int32_t createResult = rdbStore->ExecuteSql(createPackageTableIndex);
    if (createResult != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create package table index failed, rdb error number: %{public}d", createResult);
        return BUNDLE_ACTIVE_FAIL;
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::CreateModuleRecordTable(uint32_t databaseType, int64_t timeStamp)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    string moduleRecord = MODULE_RECORD_LOG_TABLE + to_string(timeStamp);
    moduleRecordsTableName_ = moduleRecord;
    string createModuleRecordTableSql = "CREATE TABLE IF NOT EXISTS "
                                        + moduleRecord
                                        + " ("
                                        + BUNDLE_ACTIVE_DB_USER_ID + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_MODULE_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_MODULE_LAUNCHED_COUNT + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_LAST_TIME + " INTEGER NOT NULL);";
    int32_t createModuleRecordTable = rdbStore->ExecuteSql(createModuleRecordTableSql);
    if (createModuleRecordTable != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create ModuleRecord table failed, rdb error number: %{public}d", createModuleRecordTable);
        return BUNDLE_ACTIVE_FAIL;
    }
    string createModuleTableIndex = GetTableIndexSql(databaseType, timeStamp, true, BUNDLE_ACTIVE_DB_INDEX_MODULE);
    int32_t createResult = rdbStore->ExecuteSql(createModuleTableIndex);
    if (createResult != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create module table index failed, rdb error number: %{public}d", createResult);
        return BUNDLE_ACTIVE_FAIL;
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::CreateFormRecordTable(uint32_t databaseType, int64_t timeStamp)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    string formRecord = FORM_RECORD_LOG_TABLE + to_string(timeStamp);
    formRecordsTableName_ = formRecord;
    string createFormRecordTableSql = "CREATE TABLE IF NOT EXISTS "
                                        + formRecord
                                        + " ("
                                        + BUNDLE_ACTIVE_DB_USER_ID + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_MODULE_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_FORM_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_FORM_DIMENSION + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_FORM_ID + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_FORM_TOUCH_COUNT + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_LAST_TIME + " INTEGER NOT NULL);";
    int32_t createFormRecordTable = rdbStore->ExecuteSql(createFormRecordTableSql);
    if (createFormRecordTable != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create ModuleRecord table failed, rdb error number: %{public}d", createFormRecordTable);
        return BUNDLE_ACTIVE_FAIL;
    }
    string createFormTableIndex = GetTableIndexSql(databaseType, timeStamp, true, BUNDLE_ACTIVE_DB_INDEX_FORM);
    int32_t createResult = rdbStore->ExecuteSql(createFormTableIndex);
    if (createResult != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create module table index failed, rdb error number: %{public}d", createResult);
        return BUNDLE_ACTIVE_FAIL;
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::CreateDurationTable(uint32_t databaseType)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    string createDurationTableSql = "CREATE TABLE IF NOT EXISTS "
                                        + DURATION_LOG_TABLE
                                        + " ("
                                        + BUNDLE_ACTIVE_DB_BOOT_BASED_DURATION + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_SCREEN_ON_DURATION + " INTEGER NOT NULL);";
    int32_t createDurationTable = rdbStore->ExecuteSql(createDurationTableSql);
    if (createDurationTable != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create duration table failed, rdb error number: %{public}d", createDurationTable);
        return BUNDLE_ACTIVE_FAIL;
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::CreateBundleHistoryTable(uint32_t databaseType)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return BUNDLE_ACTIVE_FAIL;
    }
    string createBundleHistoryTableSql = "CREATE TABLE IF NOT EXISTS "
                                        + BUNDLE_HISTORY_LOG_TABLE
                                        + " ("
                                        + BUNDLE_ACTIVE_DB_USER_ID + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_NAME + " TEXT NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_LAST_BOOT_FROM_USED_TIME + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_LAST_SCREEN_USED_TIME + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_CURRENT_GROUP + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_REASON_IN_GROUP + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_ALIVE_TIMEOUT_TIME + " INTEGER NOT NULL, "
                                        + BUNDLE_ACTIVE_DB_BUNDLE_DAILY_TIMEOUT_TIME + " INTEGER NOT NULL);";
    int32_t createBundleHistoryTable = rdbStore->ExecuteSql(createBundleHistoryTableSql);
    if (createBundleHistoryTable != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create bundleHistory table failed, rdb error number: %{public}d", createBundleHistoryTable);
        return createBundleHistoryTable;
    }
    int32_t time = 0;
    string createBundleHistoryTableIndex = GetTableIndexSql(databaseType, time, true);
    int32_t createResult = rdbStore->ExecuteSql(createBundleHistoryTableIndex);
    if (createResult != NativeRdb::E_OK) {
        BUNDLE_ACTIVE_LOGE("create bundleHistory table index failed, rdb error number: %{public}d", createResult);
        return BUNDLE_ACTIVE_FAIL;
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

void BundleActiveUsageDatabase::PutBundleHistoryData(int32_t userId,
    shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> userHistory)
{
    lock_guard<mutex> lock(databaseMutex_);
    if (userHistory == nullptr) {
        BUNDLE_ACTIVE_LOGE("userHistory is nullptr");
        return;
    }
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(APP_GROUP_DATABASE_INDEX);
    if (rdbStore == nullptr) {
        return;
    }
    CheckDatabaseFile(APP_GROUP_DATABASE_INDEX);
    if (bundleHistoryTableName_ == UNKNOWN_TABLE_NAME) {
        CreateBundleHistoryTable(APP_GROUP_DATABASE_INDEX);
        bundleHistoryTableName_ = BUNDLE_HISTORY_LOG_TABLE;
    }
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    int64_t outRowId = BUNDLE_ACTIVE_FAIL;
    NativeRdb::ValuesBucket valuesBucket;
    vector<string> queryCondition;
    int32_t updatedcount = 0;
    int32_t unupdatedcount = 0;
    for (auto iter = userHistory->begin(); iter != userHistory->end(); iter++) {
        if (iter->second == nullptr || !iter->second->isChanged_) {
            unupdatedcount++;
            continue;
        }
        queryCondition.push_back(to_string(userId));
        queryCondition.push_back(iter->first);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_LAST_BOOT_FROM_USED_TIME, iter->second->lastBootFromUsedTimeStamp_);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_LAST_SCREEN_USED_TIME, iter->second->lastScreenUsedTimeStamp_);
        valuesBucket.PutInt(BUNDLE_ACTIVE_DB_CURRENT_GROUP, iter->second->currentGroup_);
        valuesBucket.PutInt(BUNDLE_ACTIVE_DB_REASON_IN_GROUP, static_cast<int32_t>(iter->second->reasonInGroup_));
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_BUNDLE_ALIVE_TIMEOUT_TIME, iter->second->bundleAliveTimeoutTimeStamp_);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_BUNDLE_DAILY_TIMEOUT_TIME, iter->second->bundleDailyTimeoutTimeStamp_);
        rdbStore->Update(changeRow, BUNDLE_HISTORY_LOG_TABLE, valuesBucket, "userId = ? and bundleName = ?",
            queryCondition);
        if (changeRow == NO_UPDATE_ROW) {
            valuesBucket.PutString(BUNDLE_ACTIVE_DB_BUNDLE_NAME, iter->first);
            valuesBucket.PutInt(BUNDLE_ACTIVE_DB_USER_ID, userId);
            rdbStore->Insert(outRowId, BUNDLE_HISTORY_LOG_TABLE, valuesBucket);
            outRowId = BUNDLE_ACTIVE_FAIL;
        } else {
            changeRow = BUNDLE_ACTIVE_FAIL;
        }
        valuesBucket.Clear();
        queryCondition.clear();
        iter->second->isChanged_ = false;
        updatedcount++;
    }
    BUNDLE_ACTIVE_LOGI("PutBundleHistoryData, update %{public}d bundles, keep %{public}d bundles group",
        updatedcount, unupdatedcount);
}

shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> BundleActiveUsageDatabase::GetBundleHistoryData(
    int32_t userId)
{
    lock_guard<mutex> lock(databaseMutex_);
    if (bundleHistoryTableName_ == UNKNOWN_TABLE_NAME) {
        return nullptr;
    }
    string queryHistoryDataSql = "select * from " + BUNDLE_HISTORY_LOG_TABLE + " where userId = ?";
    vector<string> queryCondition;
    queryCondition.push_back(to_string(userId));
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(APP_GROUP_DATABASE_INDEX,
        queryHistoryDataSql, queryCondition);
    if (bundleActiveResult == nullptr) {
        return nullptr;
    }
    int32_t tableRowNumber;
    bundleActiveResult->GetRowCount(tableRowNumber);
    if (tableRowNumber == TABLE_ROW_ZERO) {
        return nullptr;
    }
    string bundleName;
    shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> userUsageHistory =
        make_shared<map<string, shared_ptr<BundleActivePackageHistory>>>();
    int32_t currentBundleGroupReason = 0;
    for (int32_t i = 0; i < tableRowNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, bundleName);
        shared_ptr<BundleActivePackageHistory> usageHistory = make_shared<BundleActivePackageHistory>();
        bundleActiveResult->GetLong(LAST_BOOT_FROM_USED_TIME_COLUMN_INDEX, usageHistory->lastBootFromUsedTimeStamp_);
        bundleActiveResult->GetLong(LAST_SCREEN_USED_TIME_COLUMN_INDEX, usageHistory->lastScreenUsedTimeStamp_);
        bundleActiveResult->GetInt(CURRENT_GROUP_COLUMN_INDEX, usageHistory->currentGroup_);
        bundleActiveResult->GetInt(REASON_IN_GROUP_COLUMN_INDEX, currentBundleGroupReason);
        usageHistory->reasonInGroup_ = static_cast<uint32_t>(currentBundleGroupReason);
        bundleActiveResult->GetLong(BUNDLE_ALIVE_TIMEOUT_TIME_COLUMN_INDEX,
            usageHistory->bundleAliveTimeoutTimeStamp_);
        bundleActiveResult->GetLong(BUNDLE_DAILY_TIMEOUT_TIME_COLUMN_INDEX,
            usageHistory->bundleDailyTimeoutTimeStamp_);
        userUsageHistory->insert(pair<string, shared_ptr<BundleActivePackageHistory>>(bundleName,
            usageHistory));
    }
    return userUsageHistory;
}

void BundleActiveUsageDatabase::PutDurationData(int64_t bootBasedDuration, int64_t screenOnDuration)
{
    lock_guard<mutex> lock(databaseMutex_);
    CheckDatabaseFile(APP_GROUP_DATABASE_INDEX);
    if (durationTableName_ == UNKNOWN_TABLE_NAME) {
        CreateDurationTable(APP_GROUP_DATABASE_INDEX);
        durationTableName_ = DURATION_LOG_TABLE;
    }
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(APP_GROUP_DATABASE_INDEX);
    if (rdbStore == nullptr) {
        return;
    }
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    int64_t outRowId = BUNDLE_ACTIVE_FAIL;
    NativeRdb::ValuesBucket valuesBucket;
    valuesBucket.PutLong(BUNDLE_ACTIVE_DB_BOOT_BASED_DURATION, bootBasedDuration);
    valuesBucket.PutLong(BUNDLE_ACTIVE_DB_SCREEN_ON_DURATION, screenOnDuration);
    rdbStore->Update(changeRow, DURATION_LOG_TABLE, valuesBucket);
    if (changeRow == NO_UPDATE_ROW) {
        rdbStore->Insert(outRowId, DURATION_LOG_TABLE, valuesBucket);
    }
}

pair<int64_t, int64_t> BundleActiveUsageDatabase::GetDurationData()
{
    lock_guard<mutex> lock(databaseMutex_);
    pair<int64_t, int64_t> durationData;
    if (durationTableName_ == UNKNOWN_TABLE_NAME) {
        return durationData;
    }
    string queryDurationDataSql = "select * from " + DURATION_LOG_TABLE;
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(APP_GROUP_DATABASE_INDEX,
        queryDurationDataSql,
        vector<string> {});
    if (bundleActiveResult == nullptr) {
        return durationData;
    }
    int32_t tableRowNumber;
    bundleActiveResult->GetRowCount(tableRowNumber);
    if (tableRowNumber == DURATION_TABLE_ROW_NUMBER) {
        bundleActiveResult->GoToRow(tableRowNumber - DURATION_TABLE_ROW_NUMBER);
        bundleActiveResult->GetLong(BOOT_BASED_DURATION_COLUMN_INDEX, durationData.first);
        bundleActiveResult->GetLong(SCREEN_ON_DURATION_COLUMN_INDEX, durationData.second);
    }
    return durationData;
}

void BundleActiveUsageDatabase::FlushPackageInfo(uint32_t databaseType, const BundleActivePeriodStats &stats)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return;
    }
    string tableName = PACKAGE_LOG_TABLE + to_string(stats.beginTime_);
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    int64_t outRowId = BUNDLE_ACTIVE_FAIL;
    NativeRdb::ValuesBucket valuesBucket;
    vector<string> queryCondition;
    for (auto iter = stats.bundleStats_.begin(); iter != stats.bundleStats_.end(); iter++) {
        if (iter->second == nullptr || (iter->second->totalInFrontTime_ == 0 &&
            iter->second->totalContiniousTaskUsedTime_ == 0)) {
            continue;
        }
        queryCondition.push_back(to_string(stats.userId_));
        queryCondition.push_back(iter->first);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_BUNDLE_STARTED_COUNT, iter->second->bundleStartedCount_);
        int64_t lastTimeUsedAdjusted = iter->second->lastTimeUsed_ == -1 ?
            iter->second->lastTimeUsed_ : iter->second->lastTimeUsed_ - stats.beginTime_;
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_LAST_TIME, lastTimeUsedAdjusted);
        int64_t lastContinuousTaskUsedAdjusted = iter->second->lastContiniousTaskUsed_ == -1 ?
            iter->second->lastContiniousTaskUsed_ : iter->second->lastContiniousTaskUsed_ - stats.beginTime_;
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_LAST_TIME_CONTINUOUS_TASK, lastContinuousTaskUsedAdjusted);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_TOTAL_TIME, iter->second->totalInFrontTime_);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_TOTAL_TIME_CONTINUOUS_TASK, iter->second->totalContiniousTaskUsedTime_);
        rdbStore->Update(changeRow, tableName, valuesBucket, "userId = ? and bundleName = ?", queryCondition);
        if (changeRow == NO_UPDATE_ROW) {
            valuesBucket.PutString(BUNDLE_ACTIVE_DB_BUNDLE_NAME, iter->second->bundleName_);
            valuesBucket.PutInt(BUNDLE_ACTIVE_DB_USER_ID, stats.userId_);
            rdbStore->Insert(outRowId, tableName, valuesBucket);
            outRowId = BUNDLE_ACTIVE_FAIL;
        } else {
            changeRow = BUNDLE_ACTIVE_FAIL;
        }
        valuesBucket.Clear();
        queryCondition.clear();
    }
}

shared_ptr<BundleActivePeriodStats> BundleActiveUsageDatabase::GetCurrentUsageData(int32_t databaseType,
    int32_t userId)
{
    lock_guard<mutex> lock(databaseMutex_);
    if (databaseType < 0 || databaseType >= static_cast<int32_t>(sortedTableArray_.size())) {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid, databaseType = %{public}d", databaseType);
        return nullptr;
    }

    int32_t tableNumber = static_cast<int32_t>(sortedTableArray_.at(databaseType).size());
    if (tableNumber == TABLE_NOT_EXIST) {
        return nullptr;
    }
    shared_ptr<BundleActivePeriodStats> intervalStats = make_shared<BundleActivePeriodStats>();
    intervalStats->userId_ = userId;
    int64_t currentPackageTime = sortedTableArray_.at(databaseType).at(tableNumber - 1);
    intervalStats->beginTime_ = currentPackageTime;
    string packageTableName = PACKAGE_LOG_TABLE + to_string(currentPackageTime);
    string queryPackageSql = "select * from " + packageTableName + " where userId = ?";
    vector<string> queryCondition;
    queryCondition.push_back(to_string(userId));
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(databaseType, queryPackageSql,
        queryCondition);
    if (bundleActiveResult == nullptr) {
        return nullptr;
    }
    int32_t tableRowNumber;
    bundleActiveResult->GetRowCount(tableRowNumber);
    map<string, shared_ptr<BundleActivePackageStats>> bundleStats;
    int64_t relativeLastTimeUsed;
    int64_t relativeLastTimeFrontServiceUsed;
    for (int32_t i = 0; i < tableRowNumber; i++) {
        shared_ptr<BundleActivePackageStats> usageStats = make_shared<BundleActivePackageStats>();
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetInt(USER_ID_COLUMN_INDEX, intervalStats->userId_);
        bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, usageStats->bundleName_);
        bundleActiveResult->GetInt(BUNDLE_STARTED_COUNT_COLUMN_INDEX, usageStats->bundleStartedCount_);
        bundleActiveResult->GetLong(LAST_TIME_COLUMN_INDEX, relativeLastTimeUsed);
        usageStats->lastTimeUsed_ = relativeLastTimeUsed == -1 ? -1 :
            relativeLastTimeUsed + currentPackageTime;
        bundleActiveResult->GetLong(LAST_TIME_CONTINUOUS_TASK_COLUMN_INDEX, relativeLastTimeFrontServiceUsed);
        usageStats->lastContiniousTaskUsed_ = relativeLastTimeFrontServiceUsed == -1 ? -1 :
            relativeLastTimeFrontServiceUsed + currentPackageTime;
        bundleActiveResult->GetLong(TOTAL_TIME_COLUMN_INDEX, usageStats->totalInFrontTime_);
        bundleActiveResult->GetLong(TOTAL_TIME_CONTINUOUS_TASK_COLUMN_INDEX, usageStats->totalContiniousTaskUsedTime_);
        bundleStats.insert(pair<string, shared_ptr<BundleActivePackageStats>>(usageStats->bundleName_,
            usageStats));
    }
    intervalStats->bundleStats_ = bundleStats;
    if (databaseType == DAILY_DATABASE_INDEX) {
        // 加载event信息
        eventBeginTime_ = currentPackageTime;
    }
    int64_t systemTime = GetSystemTimeMs();
    intervalStats->lastTimeSaved_ = systemTime;
    return intervalStats;
}

void BundleActiveUsageDatabase::FlushEventInfo(uint32_t databaseType, BundleActivePeriodStats &stats)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return;
    }
    if (eventTableName_ == UNKNOWN_TABLE_NAME) {
        CreateEventLogTable(databaseType, stats.beginTime_);
    }
    int64_t eventTableTime = ParseStartTime(eventTableName_);
    int64_t outRowId = BUNDLE_ACTIVE_FAIL;
    NativeRdb::ValuesBucket valuesBucket;
    for (int32_t i = 0; i < stats.events_.Size(); i++) {
        valuesBucket.PutInt(BUNDLE_ACTIVE_DB_USER_ID, stats.userId_);
        valuesBucket.PutString(BUNDLE_ACTIVE_DB_BUNDLE_NAME, stats.events_.events_.at(i).bundleName_);
        valuesBucket.PutInt(BUNDLE_ACTIVE_DB_EVENT_ID, stats.events_.events_.at(i).eventId_);
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_TIME_STAMP, stats.events_.events_.at(i).timeStamp_ - eventTableTime);
        valuesBucket.PutString(BUNDLE_ACTIVE_DB_ABILITY_ID, stats.events_.events_.at(i).abilityId_);
        rdbStore->Insert(outRowId, eventTableName_, valuesBucket);
        valuesBucket.Clear();
    }
}

string BundleActiveUsageDatabase::GetTableIndexSql(uint32_t databaseType, int64_t tableTime, bool createFlag,
    int32_t indexFlag)
{
    string tableIndexSql;
    if (databaseType >= 0 && databaseType < sortedTableArray_.size()) {
        string packageTableIndex = PACKAGE_LOG_TABLE_INDEX_PREFIX + to_string(tableTime);
        string PackageTableName = PACKAGE_LOG_TABLE + to_string(tableTime);
        if (createFlag) {
            tableIndexSql = "CREATE INDEX " + packageTableIndex + " ON "
                + PackageTableName + " (userId, lastTime, bundleName);";
        } else {
            tableIndexSql = "DROP INDEX " + packageTableIndex;
        }
    } else if (databaseType == EVENT_DATABASE_INDEX) {
        string eventTableIndex = EVENT_LOG_TABLE_INDEX_PREFIX + to_string(tableTime);
        string eventTableName = EVENT_LOG_TABLE + to_string(tableTime);
        if (createFlag) {
            tableIndexSql = "CREATE INDEX " + eventTableIndex + " ON " + eventTableName +
                " (timeStamp, userId, bundleName);";
        } else {
            tableIndexSql = "DROP INDEX " + eventTableIndex;
        }
    } else if (databaseType == APP_GROUP_DATABASE_INDEX) {
        if (createFlag) {
            if (indexFlag == BUNDLE_ACTIVE_DB_INDEX_NORMAL) {
                tableIndexSql = "CREATE INDEX " + BUNDLE_HISTORY_LOG_TABLE_INDEX_PREFIX
                    + " ON " + BUNDLE_HISTORY_LOG_TABLE + " (userId, bundleName);";
            } else if (indexFlag == BUNDLE_ACTIVE_DB_INDEX_MODULE) {
                tableIndexSql = "CREATE INDEX " + MODULE_RECORD_LOG_TABLE_INDEX_PREFIX
                    + " ON " + MODULE_RECORD_LOG_TABLE + to_string(tableTime) + " (userId, bundleName, moduleName);";
            } else if (indexFlag == BUNDLE_ACTIVE_DB_INDEX_FORM) {
                tableIndexSql = "CREATE INDEX " + FORM_RECORD_LOG_TABLE_INDEX_PREFIX
                    + " ON " + FORM_RECORD_LOG_TABLE + to_string(tableTime) +
                    " (userId, moduleName, formName, formDimension, formId);";
            }
        } else {
            if (indexFlag == BUNDLE_ACTIVE_DB_INDEX_NORMAL) {
                tableIndexSql = "DROP INDEX " + BUNDLE_HISTORY_LOG_TABLE_INDEX_PREFIX;
            } else if (indexFlag == BUNDLE_ACTIVE_DB_INDEX_MODULE) {
                tableIndexSql = "DROP INDEX " + MODULE_RECORD_LOG_TABLE_INDEX_PREFIX;
            } else if (indexFlag == BUNDLE_ACTIVE_DB_INDEX_FORM) {
                tableIndexSql = "DROP INDEX " + FORM_RECORD_LOG_TABLE_INDEX_PREFIX;
            }
        }
    } else {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid, databaseType = %{public}d", databaseType);
    }
    return tableIndexSql;
}

int32_t BundleActiveUsageDatabase::SetNewIndexWhenTimeChanged(uint32_t databaseType, int64_t tableOldTime,
    int64_t tableNewTime, std::shared_ptr<NativeRdb::RdbStore> rdbStore)
{
    if (rdbStore == nullptr) {
        return BUNDLE_ACTIVE_FAIL;
    }
    if (databaseType == APP_GROUP_DATABASE_INDEX) {
        string oldModuleTableIndex = GetTableIndexSql(databaseType, tableOldTime, false,
            BUNDLE_ACTIVE_DB_INDEX_MODULE);
        int32_t deleteResult = rdbStore->ExecuteSql(oldModuleTableIndex);
        if (deleteResult != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        string newModuleTableIndex = GetTableIndexSql(databaseType, tableNewTime, true,
            BUNDLE_ACTIVE_DB_INDEX_MODULE);
        int32_t createResult = rdbStore->ExecuteSql(newModuleTableIndex);
        if (createResult != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        string oldFormTableIndex = GetTableIndexSql(databaseType, tableOldTime, false,
            BUNDLE_ACTIVE_DB_INDEX_FORM);
        deleteResult = rdbStore->ExecuteSql(oldFormTableIndex);
        if (deleteResult != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        string newFormTableIndex = GetTableIndexSql(databaseType, tableNewTime, true,
            BUNDLE_ACTIVE_DB_INDEX_FORM);
        createResult = rdbStore->ExecuteSql(newFormTableIndex);
        if (createResult != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
    } else {
        string oldTableIndex = GetTableIndexSql(databaseType, tableOldTime, false);
        int32_t deleteResult = rdbStore->ExecuteSql(oldTableIndex);
        if (deleteResult != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        string newTableIndex = GetTableIndexSql(databaseType, tableNewTime, true);
        int32_t createResult = rdbStore->ExecuteSql(newTableIndex);
        if (createResult != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::RenameTableName(uint32_t databaseType, int64_t tableOldTime,
    int64_t tableNewTime)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        return BUNDLE_ACTIVE_FAIL;
    }
    if (databaseType >= 0 && databaseType < sortedTableArray_.size()) {
        string oldPackageTableName = PACKAGE_LOG_TABLE + to_string(tableOldTime);
        string newPackageTableName = PACKAGE_LOG_TABLE + to_string(tableNewTime);
        string renamePackageTableNameSql = "alter table " + oldPackageTableName + " rename to " +
            newPackageTableName;
        int32_t renamePackageTableName = rdbStore->ExecuteSql(renamePackageTableNameSql);
        if (renamePackageTableName != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        int32_t setResult = SetNewIndexWhenTimeChanged(databaseType, tableOldTime, tableNewTime, rdbStore);
        if (setResult != BUNDLE_ACTIVE_SUCCESS) {
            return BUNDLE_ACTIVE_FAIL;
        }
    } else if (databaseType == EVENT_DATABASE_INDEX) {
        string oldEventTableName = EVENT_LOG_TABLE + to_string(tableOldTime);
        string newEventTableName = EVENT_LOG_TABLE + to_string(tableNewTime);
        string renameEventTableNameSql = "alter table " + oldEventTableName + " rename to " + newEventTableName;
        int32_t renameEventTableName = rdbStore->ExecuteSql(renameEventTableNameSql);
        if (renameEventTableName != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        int32_t setResult = SetNewIndexWhenTimeChanged(databaseType, tableOldTime, tableNewTime, rdbStore);
        if (setResult != BUNDLE_ACTIVE_SUCCESS) {
            return BUNDLE_ACTIVE_FAIL;
        }
    } else if (databaseType == APP_GROUP_DATABASE_INDEX) {
        string oldModuleTableName = MODULE_RECORD_LOG_TABLE + to_string(tableOldTime);
        string newModuleTableName = MODULE_RECORD_LOG_TABLE + to_string(tableNewTime);
        string renameModuleTableNameSql = "alter table " + oldModuleTableName + " rename to " + newModuleTableName;
        int32_t renameModuleTableName = rdbStore->ExecuteSql(renameModuleTableNameSql);
        if (renameModuleTableName != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        string oldFormTableName = FORM_RECORD_LOG_TABLE + to_string(tableOldTime);
        string newFormTableName = FORM_RECORD_LOG_TABLE + to_string(tableNewTime);
        string renameFormTableNameSql = "alter table " + oldFormTableName + " rename to " + newFormTableName;
        int32_t renameFormTableName = rdbStore->ExecuteSql(renameFormTableNameSql);
        if (renameFormTableName != NativeRdb::E_OK) {
            return BUNDLE_ACTIVE_FAIL;
        }
        int32_t setResult = SetNewIndexWhenTimeChanged(databaseType, tableOldTime, tableNewTime, rdbStore);
        if (setResult != BUNDLE_ACTIVE_SUCCESS) {
            return BUNDLE_ACTIVE_FAIL;
        }
    }
    return BUNDLE_ACTIVE_SUCCESS;
}

int32_t BundleActiveUsageDatabase::GetOptimalIntervalType(int64_t beginTime, int64_t endTime)
{
    lock_guard<mutex> lock(databaseMutex_);
    int32_t optimalIntervalType = -1;
    int64_t leastTimeDiff = numeric_limits<int64_t>::max();
    for (int32_t i = static_cast<int32_t>(sortedTableArray_.size() - 1); i >= 0; i--) {
        int32_t index = NearIndexOnOrBeforeCurrentTime(beginTime, sortedTableArray_.at(i));
        int32_t size = static_cast<int32_t>(sortedTableArray_.at(i).size());
        if (index >= 0 && index < size) {
            int64_t diff = abs(sortedTableArray_.at(i).at(index) - beginTime);
            if (diff < leastTimeDiff) {
                leastTimeDiff = diff;
                optimalIntervalType = i;
            }
        }
    }
    BUNDLE_ACTIVE_LOGI("optimalIntervalType is %{public}d", optimalIntervalType);
    return optimalIntervalType;
}

void BundleActiveUsageDatabase::RemoveOldData(int64_t currentTime)
{
    lock_guard<mutex> lock(databaseMutex_);
    calendar_->SetMilliseconds(currentTime);
    calendar_->IncreaseYears(-1 * MAX_FILES_EVERY_INTERVAL_TYPE[YEARLY_DATABASE_INDEX]);
    std::unique_ptr<std::vector<int64_t>> overdueYearsTableCreateTime = GetOverdueTableCreateTime(YEARLY_DATABASE_INDEX,
        calendar_->GetMilliseconds());
    if (overdueYearsTableCreateTime != nullptr) {
        for (uint32_t i = 0; i < overdueYearsTableCreateTime->size(); i++) {
            DeleteInvalidTable(YEARLY_DATABASE_INDEX, overdueYearsTableCreateTime->at(i));
        }
    }
    calendar_->SetMilliseconds(currentTime);
    calendar_->IncreaseMonths(-1 * MAX_FILES_EVERY_INTERVAL_TYPE[MONTHLY_DATABASE_INDEX]);
    std::unique_ptr<std::vector<int64_t>> overdueMonthsTableCreateTime
        = GetOverdueTableCreateTime(MONTHLY_DATABASE_INDEX, calendar_->GetMilliseconds());
    if (overdueMonthsTableCreateTime != nullptr) {
        for (uint32_t i = 0; i < overdueMonthsTableCreateTime->size(); i++) {
            DeleteInvalidTable(MONTHLY_DATABASE_INDEX, overdueMonthsTableCreateTime->at(i));
        }
    }
    calendar_->SetMilliseconds(currentTime);
    calendar_->IncreaseWeeks(-1 * MAX_FILES_EVERY_INTERVAL_TYPE[WEEKLY_DATABASE_INDEX]);
    std::unique_ptr<std::vector<int64_t>> overdueWeeksTableCreateTime = GetOverdueTableCreateTime(WEEKLY_DATABASE_INDEX,
        calendar_->GetMilliseconds());
    if (overdueWeeksTableCreateTime != nullptr) {
        for (uint32_t i = 0; i < overdueWeeksTableCreateTime->size(); i++) {
            DeleteInvalidTable(WEEKLY_DATABASE_INDEX, overdueWeeksTableCreateTime->at(i));
        }
    }
    calendar_->SetMilliseconds(currentTime);
    calendar_->IncreaseDays(-1 * MAX_FILES_EVERY_INTERVAL_TYPE[DAILY_DATABASE_INDEX]);
    std::unique_ptr<std::vector<int64_t>> overdueDaysTableCreateTime = GetOverdueTableCreateTime(DAILY_DATABASE_INDEX,
        calendar_->GetMilliseconds());
    if (overdueDaysTableCreateTime != nullptr) {
        for (uint32_t i = 0; i < overdueDaysTableCreateTime->size(); i++) {
            DeleteInvalidTable(DAILY_DATABASE_INDEX, overdueDaysTableCreateTime->at(i));
        }
    }
    for (uint32_t i = 0; i < sortedTableArray_.size(); i++) {
        HandleTableInfo(i);
        DeleteExcessiveTableData(i);
    }
}

void BundleActiveUsageDatabase::RenewTableTime(int64_t timeDiffMillis)
{
    lock_guard<mutex> lock(databaseMutex_);
    for (uint32_t i = 0; i < sortedTableArray_.size(); i++) {
        if (sortedTableArray_.at(i).empty()) {
            continue;
        }
        vector<int64_t> tableArray = sortedTableArray_.at(i);
        for (uint32_t j = 0; j < tableArray.size(); j++) {
            int64_t newTime = tableArray.at(j) + timeDiffMillis;
            BUNDLE_ACTIVE_LOGI("new table time is %{public}lld", (long long)newTime);
            if (newTime < 0) {
                DeleteInvalidTable(i, tableArray.at(j));
            } else {
                RenameTableName(i, tableArray.at(j), newTime);
            }
        }
        sortedTableArray_.at(i).clear();
        HandleTableInfo(i);
        DeleteExcessiveTableData(i);
    }
    if (eventTableName_ != UNKNOWN_TABLE_NAME) {
        int64_t oldTime = ParseStartTime(eventTableName_);
        int64_t newTime = oldTime + timeDiffMillis;
        if (newTime < 0) {
            int32_t deletedResult = DeleteInvalidTable(EVENT_DATABASE_INDEX, oldTime);
            if (deletedResult == BUNDLE_ACTIVE_SUCCESS) {
                eventTableName_ = UNKNOWN_TABLE_NAME;
            }
        } else {
            int32_t renamedResult = RenameTableName(EVENT_DATABASE_INDEX, oldTime, newTime);
            if (renamedResult == BUNDLE_ACTIVE_SUCCESS) {
                eventTableName_ = EVENT_LOG_TABLE + to_string(newTime);
            }
        }
    }
    if (formRecordsTableName_ != UNKNOWN_TABLE_NAME && moduleRecordsTableName_ != UNKNOWN_TABLE_NAME) {
        int64_t oldTime = ParseStartTime(moduleRecordsTableName_);
        int64_t newTime = oldTime + timeDiffMillis;
        int32_t renamedResult = RenameTableName(APP_GROUP_DATABASE_INDEX, oldTime, newTime);
        if (renamedResult == BUNDLE_ACTIVE_SUCCESS) {
            moduleRecordsTableName_ = MODULE_RECORD_LOG_TABLE + to_string(newTime);
            formRecordsTableName_ = FORM_RECORD_LOG_TABLE + to_string(newTime);
        }
    }
}

void BundleActiveUsageDatabase::UpdateEventData(int32_t databaseType, BundleActivePeriodStats &stats)
{
    lock_guard<mutex> lock(databaseMutex_);
    CheckDatabaseFile(databaseType);
    if (databaseType != DAILY_DATABASE_INDEX) {
        return;
    }
    if (stats.events_.Size() != 0) {
        CheckDatabaseFile(EVENT_DATABASE_INDEX);
        FlushEventInfo(EVENT_DATABASE_INDEX, stats);
    }
}

void BundleActiveUsageDatabase::UpdateBundleUsageData(int32_t databaseType, BundleActivePeriodStats &stats)
{
    lock_guard<mutex> lock(databaseMutex_);
    if (databaseType < 0 || databaseType >= EVENT_DATABASE_INDEX) {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid : %{public}d", databaseType);
        return;
    }
    CheckDatabaseFile(databaseType);
    int32_t packageTableIndex = BundleActiveBinarySearch::GetInstance()->BinarySearch(
        sortedTableArray_.at(databaseType), stats.beginTime_);
    if (packageTableIndex < 0) {
        CreatePackageLogTable(databaseType, stats.beginTime_);
        if (databaseType == DAILY_DATABASE_INDEX) {
            eventBeginTime_ = stats.beginTime_;
            DeleteExcessiveTableData(EVENT_DATABASE_INDEX);
        }
        sortedTableArray_.at(databaseType).push_back(stats.beginTime_);
        sort(sortedTableArray_.at(databaseType).begin(), sortedTableArray_.at(databaseType).end());
        DeleteExcessiveTableData(databaseType);
    }
    FlushPackageInfo(databaseType, stats);
    int64_t systemTime = GetSystemTimeMs();
    stats.lastTimeSaved_ = systemTime;
}

vector<BundleActivePackageStats> BundleActiveUsageDatabase::QueryDatabaseUsageStats(int32_t databaseType,
    int64_t beginTime, int64_t endTime, int32_t userId)
{
    lock_guard<mutex> lock(databaseMutex_);
    vector<BundleActivePackageStats> databaseUsageStats;
    if (databaseType < 0 || databaseType >= static_cast<int32_t>(sortedTableArray_.size())) {
        BUNDLE_ACTIVE_LOGE("databaseType is invalid, databaseType = %{public}d", databaseType);
        return databaseUsageStats;
    }
    if (endTime <= beginTime) {
        BUNDLE_ACTIVE_LOGE("endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)endTime, (long long)beginTime);
        return databaseUsageStats;
    }
    int32_t startIndex = NearIndexOnOrBeforeCurrentTime(beginTime, sortedTableArray_.at(databaseType));
    if (startIndex < 0) {
        startIndex = 0;
    }
    int32_t endIndex = NearIndexOnOrBeforeCurrentTime(endTime, sortedTableArray_.at(databaseType));
    if (endIndex < 0) {
        return databaseUsageStats;
    }
    if (sortedTableArray_.at(databaseType).at(endIndex) == endTime) {
        endIndex--;
        if (endIndex < 0) {
        return databaseUsageStats;
        }
    }
    for (int32_t i = startIndex; i <= endIndex; i++) {
        int64_t packageTableTime;
        string packageTableName;
        string queryPackageSql;
        vector<string> queryCondition;
        packageTableTime = sortedTableArray_.at(databaseType).at(i);
        packageTableName = PACKAGE_LOG_TABLE + to_string(packageTableTime);
        queryCondition.push_back(to_string(userId));
        if (startIndex == endIndex) {
            int64_t diff = beginTime - packageTableTime;
            if (diff >= 0) {
                queryCondition.push_back(to_string(diff));
            } else {
                queryCondition.push_back(to_string(LAST_TIME_IN_MILLIS_MIN));
            }
            queryCondition.push_back(to_string(endTime - packageTableTime));
            queryPackageSql = "select * from " + packageTableName +
                " where userId = ? and lastTime >= ? and lastTime <= ?";
        } else {
            if (i == startIndex) {
                int64_t diff = beginTime - packageTableTime;
                if (diff >= 0) {
                    queryCondition.push_back(to_string(diff));
                } else {
                    queryCondition.push_back(to_string(LAST_TIME_IN_MILLIS_MIN));
                }
                queryPackageSql = "select * from " + packageTableName + " where userId = ? and lastTime >= ?";
            } else if (i == endIndex) {
                queryCondition.push_back(to_string(endTime - packageTableTime));
                queryPackageSql = "select * from " + packageTableName + " where userId = ? and lastTime <= ?";
            } else {
                queryPackageSql = "select * from " + packageTableName + " where userId = ?";
            }
        }
        unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(databaseType, queryPackageSql,
            queryCondition);
        if (bundleActiveResult == nullptr) {
            return databaseUsageStats;
        }
        int32_t tableRowNumber;
        bundleActiveResult->GetRowCount(tableRowNumber);
        BundleActivePackageStats usageStats;
        int64_t relativeLastTimeUsed;
        int64_t relativeLastTimeFrontServiceUsed;
        for (int32_t j = 0; j < tableRowNumber; j++) {
            bundleActiveResult->GoToRow(j);
            bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, usageStats.bundleName_);
            bundleActiveResult->GetInt(BUNDLE_STARTED_COUNT_COLUMN_INDEX, usageStats.bundleStartedCount_);
            bundleActiveResult->GetLong(LAST_TIME_COLUMN_INDEX, usageStats.lastTimeUsed_);
            bundleActiveResult->GetLong(LAST_TIME_COLUMN_INDEX, relativeLastTimeUsed);
            usageStats.lastTimeUsed_ = relativeLastTimeUsed == -1 ? -1 :
                relativeLastTimeUsed + packageTableTime;
            bundleActiveResult->GetLong(LAST_TIME_CONTINUOUS_TASK_COLUMN_INDEX, relativeLastTimeFrontServiceUsed);
            usageStats.lastContiniousTaskUsed_ = relativeLastTimeFrontServiceUsed == -1 ? -1 :
                relativeLastTimeFrontServiceUsed + packageTableTime;
            bundleActiveResult->GetLong(TOTAL_TIME_COLUMN_INDEX, usageStats.totalInFrontTime_);
            bundleActiveResult->GetLong(TOTAL_TIME_CONTINUOUS_TASK_COLUMN_INDEX,
                usageStats.totalContiniousTaskUsedTime_);
            databaseUsageStats.push_back(usageStats);
        }
        queryCondition.clear();
    }
    return databaseUsageStats;
}

vector<BundleActiveEvent> BundleActiveUsageDatabase::QueryDatabaseEvents(int64_t beginTime, int64_t endTime,
    int32_t userId, string bundleName)
{
    lock_guard<mutex> lock(databaseMutex_);
    vector<BundleActiveEvent> databaseEvents;
    int64_t eventTableTime = ParseStartTime(eventTableName_);
    if (JudgeQueryCondition(beginTime, endTime, eventTableTime) == QUERY_CONDITION_INVALID) {
        return databaseEvents;
    }
    vector<string> queryCondition;
    int64_t diff = beginTime - eventTableTime;
    if (diff >= 0) {
        queryCondition.push_back(to_string(diff));
    } else {
        queryCondition.push_back(to_string(EVENT_TIME_IN_MILLIS_MIN));
    }
    queryCondition.push_back(to_string(endTime - eventTableTime));
    queryCondition.push_back(to_string(userId));
    string queryEventSql;
    if (bundleName.empty()) {
        queryEventSql = "select * from " + eventTableName_ + " where timeStamp >= ? and timeStamp <= ? and userId = ?";
    } else {
        queryCondition.push_back(bundleName);
        queryEventSql = "select * from " + eventTableName_ +
            " where timeStamp >= ? and timeStamp <= ? and userId = ? and bundleName = ?";
    }
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(EVENT_DATABASE_INDEX,
        queryEventSql, queryCondition);
    if (bundleActiveResult == nullptr) {
        return databaseEvents;
    }
    int32_t tableRowNumber;
    bundleActiveResult->GetRowCount(tableRowNumber);
    BundleActiveEvent event;
    string relativeTimeStamp;
    for (int32_t i = 0; i < tableRowNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, event.bundleName_);
        bundleActiveResult->GetInt(EVENT_ID_COLUMN_INDEX, event.eventId_);
        bundleActiveResult->GetString(TIME_STAMP_COLUMN_INDEX, relativeTimeStamp);
        event.timeStamp_ = atoll(relativeTimeStamp.c_str()) + eventTableTime;
        bundleActiveResult->GetString(ABILITY_ID_COLUMN_INDEX, event.abilityId_);
        databaseEvents.push_back(event);
    }
    return databaseEvents;
}

void BundleActiveUsageDatabase::OnPackageUninstalled(const int32_t userId, const string& bundleName)
{
    lock_guard<mutex> lock(databaseMutex_);
    for (uint32_t i = 0; i < sortedTableArray_.size(); i++) {
        if (sortedTableArray_.at(i).empty()) {
            continue;
        }
        for (uint32_t j = 0; j < sortedTableArray_.at(i).size(); j++) {
            string packageTableName = PACKAGE_LOG_TABLE + to_string(sortedTableArray_.at(i).at(j));
            DeleteUninstalledInfo(userId, bundleName, packageTableName, i);
        }
    }
    if (eventTableName_ != UNKNOWN_TABLE_NAME) {
        DeleteUninstalledInfo(userId, bundleName, eventTableName_, EVENT_DATABASE_INDEX);
    }
    if (bundleHistoryTableName_ != UNKNOWN_TABLE_NAME) {
        DeleteUninstalledInfo(userId, bundleName, bundleHistoryTableName_, APP_GROUP_DATABASE_INDEX);
    }
    if (moduleRecordsTableName_ != UNKNOWN_TABLE_NAME) {
        DeleteUninstalledInfo(userId, bundleName, moduleRecordsTableName_, APP_GROUP_DATABASE_INDEX);
    }
    if (formRecordsTableName_ != UNKNOWN_TABLE_NAME) {
        DeleteUninstalledInfo(userId, bundleName, formRecordsTableName_, APP_GROUP_DATABASE_INDEX);
    }
}

void BundleActiveUsageDatabase::DeleteUninstalledInfo(const int32_t userId, const string& bundleName,
    const string& tableName, uint32_t databaseType)
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(databaseType);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return;
    }
    int32_t deletedRows = BUNDLE_ACTIVE_FAIL;
    vector<string> queryCondition;
    queryCondition.push_back(to_string(userId));
    if (bundleName.empty()) {
        rdbStore->Delete(deletedRows, tableName, "userId = ?", queryCondition);
    } else {
        queryCondition.push_back(bundleName);
        rdbStore->Delete(deletedRows, tableName, "userId = ? and bundleName = ?", queryCondition);
    }
}

int64_t BundleActiveUsageDatabase::GetSystemTimeMs()
{
    time_t now;
    (void)time(&now);  // unit is seconds.
    if (static_cast<int64_t>(now) < 0) {
        BUNDLE_ACTIVE_LOGE("Get now time error");
        return 0;
    }
    auto tarEndTimePoint = std::chrono::system_clock::from_time_t(now);
    auto tarDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tarEndTimePoint.time_since_epoch());
    int64_t tarDate = tarDuration.count();
    if (tarDate < 0) {
        BUNDLE_ACTIVE_LOGE("tarDuration is less than 0.");
        return -1;
    }
    return static_cast<int64_t>(tarDate);
}

void BundleActiveUsageDatabase::UpdateModuleData(const int32_t userId,
    std::map<std::string, std::shared_ptr<BundleActiveModuleRecord>>& moduleRecords, const int64_t timeStamp)
{
    lock_guard<mutex> lock(databaseMutex_);
    CheckDatabaseFile(APP_GROUP_DATABASE_INDEX);
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(APP_GROUP_DATABASE_INDEX);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return;
    }
    if (moduleRecordsTableName_ == UNKNOWN_TABLE_NAME) {
        CreateModuleRecordTable(APP_GROUP_DATABASE_INDEX, timeStamp);
    }
    if (formRecordsTableName_ == UNKNOWN_TABLE_NAME) {
        CreateFormRecordTable(APP_GROUP_DATABASE_INDEX, timeStamp);
    }
    int64_t moduleTableTime = ParseStartTime(moduleRecordsTableName_);
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    int64_t outRowId = BUNDLE_ACTIVE_FAIL;
    NativeRdb::ValuesBucket moduleValuesBucket;
    vector<string> queryCondition;
    for (const auto& oneModuleRecord : moduleRecords) {
        if (oneModuleRecord.second) {
            queryCondition.emplace_back(to_string(oneModuleRecord.second->userId_));
            queryCondition.emplace_back(oneModuleRecord.second->bundleName_);
            queryCondition.emplace_back(oneModuleRecord.second->moduleName_);
            moduleValuesBucket.PutInt(BUNDLE_ACTIVE_DB_MODULE_LAUNCHED_COUNT, oneModuleRecord.second->launchedCount_);
            int64_t adjustLastTime = oneModuleRecord.second->lastModuleUsedTime_ != -1 ?
                oneModuleRecord.second->lastModuleUsedTime_ - moduleTableTime : -1;
            moduleValuesBucket.PutLong(BUNDLE_ACTIVE_DB_LAST_TIME, adjustLastTime);
            rdbStore->Update(changeRow, moduleRecordsTableName_, moduleValuesBucket,
                "userId = ? and bundleName = ? and moduleName = ?", queryCondition);
            if (changeRow == NO_UPDATE_ROW) {
                moduleValuesBucket.PutInt(BUNDLE_ACTIVE_DB_USER_ID, oneModuleRecord.second->userId_);
                moduleValuesBucket.PutString(BUNDLE_ACTIVE_DB_BUNDLE_NAME, oneModuleRecord.second->bundleName_);
                moduleValuesBucket.PutString(BUNDLE_ACTIVE_DB_MODULE_NAME, oneModuleRecord.second->moduleName_);
                rdbStore->Insert(outRowId, moduleRecordsTableName_, moduleValuesBucket);
                outRowId = BUNDLE_ACTIVE_FAIL;
                changeRow = BUNDLE_ACTIVE_FAIL;
            } else {
                changeRow = BUNDLE_ACTIVE_FAIL;
            }
            moduleValuesBucket.Clear();
            queryCondition.clear();
            for (const auto& oneFormRecord : oneModuleRecord.second->formRecords_) {
                UpdateFormData(oneModuleRecord.second->userId_, oneModuleRecord.second->bundleName_,
                    oneModuleRecord.second->moduleName_, oneFormRecord, rdbStore);
            }
        }
    }
}

void BundleActiveUsageDatabase::UpdateFormData(const int32_t userId, const std::string bundleName,
    const string moduleName, const BundleActiveFormRecord& formRecord,
    std::shared_ptr<NativeRdb::RdbStore> rdbStore)
{
    if (rdbStore == nullptr) {
        return;
    }
    int64_t formRecordsTableTime = ParseStartTime(formRecordsTableName_);
    NativeRdb::ValuesBucket formValueBucket;
    vector<string> queryCondition;
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    int64_t outRowId = BUNDLE_ACTIVE_FAIL;
    queryCondition.emplace_back(to_string(userId));
    queryCondition.emplace_back(bundleName);
    queryCondition.emplace_back(moduleName);
    queryCondition.emplace_back(formRecord.formName_);
    queryCondition.emplace_back(to_string(formRecord.formDimension_));
    queryCondition.emplace_back(to_string(formRecord.formId_));
    formValueBucket.PutInt(BUNDLE_ACTIVE_DB_FORM_TOUCH_COUNT, formRecord.count_);
    int64_t adjustLastTime = formRecord.formLastUsedTime_ != -1 ? formRecord.formLastUsedTime_ -
        formRecordsTableTime : -1;
    formValueBucket.PutLong(BUNDLE_ACTIVE_DB_LAST_TIME, adjustLastTime);
    rdbStore->Update(changeRow, formRecordsTableName_, formValueBucket,
        "userId = ? and bundleName = ? and moduleName = ? and formName = ? and formDimension = ? "
        "and formId = ?",
        queryCondition);
    if (changeRow == NO_UPDATE_ROW) {
        formValueBucket.PutInt(BUNDLE_ACTIVE_DB_USER_ID, userId);
        formValueBucket.PutString(BUNDLE_ACTIVE_DB_BUNDLE_NAME, bundleName);
        formValueBucket.PutString(BUNDLE_ACTIVE_DB_MODULE_NAME, moduleName);
        formValueBucket.PutString(BUNDLE_ACTIVE_DB_FORM_NAME, formRecord.formName_);
        formValueBucket.PutInt(BUNDLE_ACTIVE_DB_FORM_DIMENSION, formRecord.formDimension_);
        formValueBucket.PutInt(BUNDLE_ACTIVE_DB_FORM_ID, formRecord.formId_);
        rdbStore->Insert(outRowId, formRecordsTableName_, formValueBucket);
    }
}

void BundleActiveUsageDatabase::RemoveFormData(const int32_t userId, const std::string bundleName,
    const std::string moduleName, const std::string formName, const int32_t formDimension,
    const int64_t formId)
{
    lock_guard<mutex> lock(databaseMutex_);
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(APP_GROUP_DATABASE_INDEX);
    if (rdbStore == nullptr) {
        BUNDLE_ACTIVE_LOGE("rdbStore is nullptr");
        return;
    }
    int32_t deletedRows = BUNDLE_ACTIVE_FAIL;
    if (formRecordsTableName_ != UNKNOWN_TABLE_NAME) {
        vector<string> queryCondition;
        queryCondition.emplace_back(to_string(userId));
        queryCondition.emplace_back(bundleName);
        queryCondition.emplace_back(moduleName);
        queryCondition.emplace_back(formName);
        queryCondition.emplace_back(to_string(formDimension));
        queryCondition.emplace_back(to_string(formId));
        int32_t ret = rdbStore->Delete(deletedRows, formRecordsTableName_,
            "userId = ? and bundleName = ? and moduleName = ? and formName = ? and formDimension = ? "
            "and formId = ?",
            queryCondition);
        if (ret != NativeRdb::E_OK) {
            BUNDLE_ACTIVE_LOGE("delete event data failed, rdb error number: %{public}d", ret);
        }
    }
}

void BundleActiveUsageDatabase::LoadModuleData(const int32_t userId, std::map<std::string,
    std::shared_ptr<BundleActiveModuleRecord>>& moduleRecords)
{
    lock_guard<mutex> lock(databaseMutex_);
    string queryModuleSql = "select * from " + moduleRecordsTableName_ + " where userId = ?";
    vector<string> queryCondition;
    queryCondition.emplace_back(to_string(userId));
    unique_ptr<NativeRdb::ResultSet> moduleRecordResult = QueryStatsInfoByStep(APP_GROUP_DATABASE_INDEX, queryModuleSql,
        queryCondition);
    if (!moduleRecordResult) {
        return;
    }
    int64_t baseTime = ParseStartTime(moduleRecordsTableName_);
    int32_t numOfModuleRecord = 0;
    moduleRecordResult->GetRowCount(numOfModuleRecord);
    for (int32_t i = 0; i < numOfModuleRecord; i++) {
        shared_ptr<BundleActiveModuleRecord> oneModuleRecord = make_shared<BundleActiveModuleRecord>();
        moduleRecordResult->GoToRow(i);
        moduleRecordResult->GetInt(USER_ID_COLUMN_INDEX, oneModuleRecord->userId_);
        moduleRecordResult->GetString(BUNDLE_NAME_COLUMN_INDEX, oneModuleRecord->bundleName_);
        moduleRecordResult->GetString(MODULE_NAME_COLUMN_INDEX, oneModuleRecord->moduleName_);
        moduleRecordResult->GetInt(MODULE_USED_COUNT_COLUMN_INDEX, oneModuleRecord->launchedCount_);
        int64_t relativeLastTime = 0;
        moduleRecordResult->GetLong(MODULE_LAST_TIME_COLUMN_INDEX, relativeLastTime);
        oneModuleRecord->lastModuleUsedTime_ =  relativeLastTime != -1 ? relativeLastTime + baseTime : -1;
        string combinedInfo = oneModuleRecord->bundleName_ + " " + oneModuleRecord->moduleName_;
        moduleRecords[combinedInfo] = oneModuleRecord;
    }
}

void BundleActiveUsageDatabase::LoadFormData(const int32_t userId, std::map<std::string,
    std::shared_ptr<BundleActiveModuleRecord>>& moduleRecords)
{
    lock_guard<mutex> lock(databaseMutex_);
    string queryFormSql = "select * from " + formRecordsTableName_ + " where userId = ?";
    vector<string> queryCondition;
    queryCondition.emplace_back(to_string(userId));
    unique_ptr<NativeRdb::ResultSet> formRecordResult = QueryStatsInfoByStep(APP_GROUP_DATABASE_INDEX, queryFormSql,
        queryCondition);
    if (!formRecordResult) {
        return;
    }
    int32_t numOfFormRecord = 0;
    int64_t baseTime = ParseStartTime(formRecordsTableName_);
    formRecordResult->GetRowCount(numOfFormRecord);
    for (int32_t i = 0; i < numOfFormRecord; i++) {
        BundleActiveFormRecord oneFormRecord;
        string moduleName = "";
        string bundleName = "";
        formRecordResult->GoToRow(i);
        formRecordResult->GetInt(USER_ID_COLUMN_INDEX, oneFormRecord.userId_);
        formRecordResult->GetString(BUNDLE_NAME_COLUMN_INDEX, bundleName);
        formRecordResult->GetString(MODULE_NAME_COLUMN_INDEX, moduleName);
        formRecordResult->GetString(FORM_NAME_COLUMN_INDEX, oneFormRecord.formName_);
        formRecordResult->GetInt(FORM_DIMENSION_COLUMN_INDEX, oneFormRecord.formDimension_);
        formRecordResult->GetLong(FORM_ID_COLUMN_INDEX, oneFormRecord.formId_);
        formRecordResult->GetInt(FORM_COUNT_COLUMN_INDEX, oneFormRecord.count_);
        int64_t relativeLastTime = 0;
        formRecordResult->GetLong(FORM_LAST_TIME_COLUMN_INDEX, relativeLastTime);
        oneFormRecord.formLastUsedTime_ = relativeLastTime != -1 ? relativeLastTime + baseTime : -1;
        auto it = moduleRecords.find(bundleName + " " + moduleName);
        if (it != moduleRecords.end() && it->second) {
            it->second->formRecords_.emplace_back(oneFormRecord);
        }
    }
}

void BundleActiveUsageDatabase::QueryDeviceEventStats(int32_t eventId, int64_t beginTime,
    int64_t endTime, std::map<std::string, BundleActiveEventStats>& eventStats, int32_t userId)
{
    lock_guard<mutex> lock(databaseMutex_);
    int64_t eventTableTime = ParseStartTime(eventTableName_);
    if (JudgeQueryCondition(beginTime, endTime, eventTableTime) == QUERY_CONDITION_INVALID) {
        return;
    }
    vector<string> queryCondition;
    int64_t diff = beginTime - eventTableTime;
    if (diff >= 0) {
        queryCondition.push_back(to_string(diff));
    } else {
        queryCondition.push_back(to_string(EVENT_TIME_IN_MILLIS_MIN));
    }
    queryCondition.push_back(to_string(endTime - eventTableTime));
    queryCondition.push_back(to_string(userId));
    queryCondition.push_back(to_string(eventId));
    string queryEventSql = "select * from " + eventTableName_ +
            " where timeStamp >= ? and timeStamp <= ? and userId = ? and eventId = ?";
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(EVENT_DATABASE_INDEX,
        queryEventSql, queryCondition);
    if (bundleActiveResult == nullptr) {
        return;
    }
    int32_t tableRowNumber;
    bundleActiveResult->GetRowCount(tableRowNumber);
    if (tableRowNumber == 0) {
        return;
    }
    BundleActiveEventStats event;
    event.name_= GetSystemEventName(eventId);
    event.count_ = tableRowNumber;
    event.eventId_ = eventId;
    eventStats.insert(std::pair<std::string, BundleActiveEventStats>(event.name_, event));
}

std::string BundleActiveUsageDatabase::GetSystemEventName(const int32_t userId)
{
    std::string systemEventName = "";
    switch (userId) {
        case BundleActiveEvent::SYSTEM_LOCK:
            systemEventName = OPERATION_SYSTEM_LOCK;
            break;
        case BundleActiveEvent::SYSTEM_UNLOCK:
            systemEventName = OPERATION_SYSTEM_UNLOCK;
            break;
        case BundleActiveEvent::SYSTEM_SLEEP:
            systemEventName = OPERATION_SYSTEM_SLEEP;
            break;
        case BundleActiveEvent::SYSTEM_WAKEUP:
            systemEventName = OPERATION_SYSTEM_WAKEUP;
            break;
        default:
            break;
    }
    return systemEventName;
}

void BundleActiveUsageDatabase::QueryNotificationEventStats(int32_t eventId, int64_t beginTime,
    int64_t endTime, std::map<std::string, BundleActiveEventStats>& notificationEventStats, int32_t userId)
{
    lock_guard<mutex> lock(databaseMutex_);
    int64_t eventTableTime = ParseStartTime(eventTableName_);
    if (JudgeQueryCondition(beginTime, endTime, eventTableTime) == QUERY_CONDITION_INVALID) {
        return;
    }
    vector<string> queryCondition;
    int64_t diff = beginTime - eventTableTime;
    if (diff >= 0) {
        queryCondition.push_back(to_string(diff));
    } else {
        queryCondition.push_back(to_string(EVENT_TIME_IN_MILLIS_MIN));
    }
    queryCondition.push_back(to_string(endTime - eventTableTime));
    queryCondition.push_back(to_string(userId));
    queryCondition.push_back(to_string(eventId));
    string queryEventSql = "select * from " + eventTableName_ +
            " where timeStamp >= ? and timeStamp <= ? and userId = ? and eventId = ?";
    unique_ptr<NativeRdb::ResultSet> bundleActiveResult = QueryStatsInfoByStep(EVENT_DATABASE_INDEX,
        queryEventSql, queryCondition);
    if (bundleActiveResult == nullptr) {
        return;
    }
    int32_t tableRowNumber;
    bundleActiveResult->GetRowCount(tableRowNumber);
    if (tableRowNumber == 0) {
        return;
    }
    BundleActiveEventStats event;
    std::map<std::string, BundleActiveEventStats>::iterator iter;
    for (int32_t i = 0; i < tableRowNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, event.name_);
        bundleActiveResult->GetInt(EVENT_ID_COLUMN_INDEX, event.eventId_);
        iter = notificationEventStats.find(event.name_);
        if (iter != notificationEventStats.end()) {
            iter->second.count_++;
        } else {
            event.count_ = 1;
            notificationEventStats.insert(std::pair<std::string, BundleActiveEventStats>(event.name_, event));
        }
    }
}

int32_t BundleActiveUsageDatabase::JudgeQueryCondition(const int64_t beginTime,
    const int64_t endTime, const int64_t eventTableTime)
{
    if (eventTableName_ == UNKNOWN_TABLE_NAME) {
        BUNDLE_ACTIVE_LOGE("eventTable does not exist");
        return QUERY_CONDITION_INVALID;
    }
    if (endTime <= beginTime) {
        BUNDLE_ACTIVE_LOGE("endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)endTime, (long long)beginTime);
        return QUERY_CONDITION_INVALID;
    }
    if (endTime < eventTableTime) {
        BUNDLE_ACTIVE_LOGE("endTime(%{public}lld) <= eventTableTime(%{public}lld)",
            (long long)endTime, (long long)eventTableTime);
        return QUERY_CONDITION_INVALID;
    }
    return QUERY_CONDITION_VALID;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


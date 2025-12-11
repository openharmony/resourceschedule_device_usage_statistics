/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <unistd.h>
#include <cstdlib>
#include <map>
#include <limits>
#include "bundle_active_constant.h"
#include "bundle_active_log.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_period_stats.h"
#include "bundle_active_usage_database.h"
#include "bundle_active_account_helper.h"
#include "bundle_active_bundle_mgr_helper.h"
#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace OHOS::NativeRdb;
using namespace std;
namespace {
    const int64_t MIN_START_TIME = 0;
}
void BundleActiveUsageDatabase::UpgradeDatabase(const int32_t oldVersion, const int32_t curVersion)
{
    BUNDLE_ACTIVE_LOGI("upgradle database oldVersion: %{public}d, curVersion: %{public}d", oldVersion, curVersion);
    if (oldVersion < curVersion && curVersion == BUNDLE_ACTIVE_CURRENT_VERSION) {
        if (oldVersion == BUNDLE_ACTIVE_VERSION_V1) {
            SupportAppTwin();
            SupportFirstUseTime();
        }
        if (oldVersion == BUNDLE_ACTIVE_VERSION_V2) {
            SupportFirstUseTime();
        }
        currentVersion_ = curVersion;
    }
}

void BundleActiveUsageDatabase::SupportAppTwin()
{
    vector<vector<string>> allTableName = vector<vector<string>>(ALL_TABLE_ARRAY_NUMBER);
    for (uint32_t i = 0; i <databaseFiles_.size(); i++) {
        HandleAllTableName(i, allTableName);
    }

    map<string, int32_t> bundleNameUidMap;
    vector<int32_t> activatedOsAccountIds;
    BundleActiveAccountHelper::GetActiveUserId(activatedOsAccountIds);
    for (uint32_t i = 0; i < allTableName.size(); i++) {
        auto tableNames = allTableName.at(i);
        shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(i);
        if (!rdbStore) {
            BUNDLE_ACTIVE_LOGI("get RdbStore fail, databaseType: %{public}u", i);
            continue;
        }
        for (string tableName: tableNames) {
            if (DURATION_LOG_TABLE == tableName) {
                continue;
            }
            AddRdbColumn(rdbStore, tableName, BUNDLE_ACTIVE_DB_UID, RDB_STORE_COLUMN_TYPE_INT);
            for (auto userId: activatedOsAccountIds) {
                UpdateOldDataUid(rdbStore, tableName, userId, bundleNameUidMap);
            }
        }
    }
}

void BundleActiveUsageDatabase::SupportFirstUseTime()
{
    shared_ptr<NativeRdb::RdbStore> rdbStore = GetBundleActiveRdbStore(APP_GROUP_DATABASE_INDEX);
    if (!rdbStore) {
        BUNDLE_ACTIVE_LOGE("get RdbStore fail, databaseType: %{public}u", APP_GROUP_DATABASE_INDEX);
        return;
    }
    vector<vector<string>> allTableName = vector<vector<string>>(ALL_TABLE_ARRAY_NUMBER);
    {
        lock_guard<ffrt::mutex> lock(databaseMutex_);
        for (uint32_t i = 0; i < databaseFiles_.size(); i++) {
            HandleTableInfo(i);
        }
        HandleAllTableName(APP_GROUP_DATABASE_INDEX, allTableName);
    }
    auto it = std::find(allTableName.at(APP_GROUP_DATABASE_INDEX).begin(),
        allTableName.at(APP_GROUP_DATABASE_INDEX).end(), BUNDLE_HISTORY_LOG_TABLE);
    if (it == allTableName.at(APP_GROUP_DATABASE_INDEX).end()) {
        BUNDLE_ACTIVE_LOGE("not have bundle history table");
        return;
    }
    vector<int32_t> activatedOsAccountIds;
    BundleActiveAccountHelper::GetActiveUserId(activatedOsAccountIds);
    {
        lock_guard<ffrt::mutex> lock(databaseMutex_);
        AddRdbColumn(rdbStore, BUNDLE_HISTORY_LOG_TABLE, BUNDLE_ACTIVE_DB_FIRST_USE_TIME, RDB_STORE_COLUMN_TYPE_INT);
    }
    for (auto userId: activatedOsAccountIds) {
        UpdateFirstUseTime(rdbStore, BUNDLE_HISTORY_LOG_TABLE, userId);
    }
}

void BundleActiveUsageDatabase::AddRdbColumn(const shared_ptr<NativeRdb::RdbStore> store,
    const string& tableName, const string& columnName, const string& columnType)
{
    if (columnType != RDB_STORE_COLUMN_TYPE_INT) {
        return;
    }
    string sqlStr = "";
    sqlStr = "ALTER TABLE " + tableName + " ADD " + columnName + " " + columnType;
    if (columnName == BUNDLE_ACTIVE_DB_UID) {
        sqlStr += " NOT NULL DEFAULT -1";
    }
    if (columnName == BUNDLE_ACTIVE_DB_FIRST_USE_TIME) {
        sqlStr += " NOT NULL DEFAULT " + std::to_string(MAX_END_TIME);
    }
    auto ret = store->ExecuteSql(sqlStr);
    if (ret != E_OK) {
        BUNDLE_ACTIVE_LOGE("add column failed columnName %{public}s", columnName.c_str());
    }
}

void BundleActiveUsageDatabase::UpdateOldDataUid(const shared_ptr<NativeRdb::RdbStore> store,
    const string& tableName, const int32_t userId, map<string, int32_t>& bundleNameUidMap)
{
    vector<string> queryCondition;
    string querySql = "select * from " + tableName;
    shared_ptr<NativeRdb::ResultSet> bundleActiveResult;
    bundleActiveResult = store->QueryByStep(querySql);
    if (!bundleActiveResult) {
        BUNDLE_ACTIVE_LOGE("query by step failed");
        return;
    }
    int32_t tableRowNumber = 0;
    bundleActiveResult->GetRowCount(tableRowNumber);
    string bundleName;
    int32_t uid;
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    NativeRdb::ValuesBucket valuesBucket;
    for (int32_t i = 0; i < tableRowNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, bundleName);
        AppExecFwk::ApplicationInfo appInfo;
        string bundleNameUserIdKey = bundleName + to_string(userId);
        auto it = bundleNameUidMap.find(bundleNameUserIdKey);
        if (it == bundleNameUidMap.end()) {
            BundleActiveBundleMgrHelper::GetInstance()->GetApplicationInfo(bundleName,
                AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, appInfo);
            uid = appInfo.uid;
            bundleNameUidMap[bundleNameUserIdKey] = uid;
        } else {
            uid = it->second;
        }
        queryCondition.push_back(to_string(userId));
        queryCondition.push_back(bundleName);
        valuesBucket.PutInt(BUNDLE_ACTIVE_DB_UID, uid);
        store->Update(changeRow, tableName, valuesBucket, "userId = ? and bundleName = ?", queryCondition);
        queryCondition.clear();
        valuesBucket.Clear();
        changeRow = BUNDLE_ACTIVE_FAIL;
    }
    bundleActiveResult->Close();
}

void BundleActiveUsageDatabase::UpdateFirstUseTime(const shared_ptr<NativeRdb::RdbStore> store,
    const string& tableName, const int32_t userId)
{
    map<string, int64_t> allBundleFirstUseTime = GetAllBundleFirstUseTime(userId);
    lock_guard<ffrt::mutex> lock(databaseMutex_);
    vector<string> queryCondition;
    string querySql = "select * from " + tableName;
    shared_ptr<NativeRdb::ResultSet> bundleActiveResult;
    bundleActiveResult = store->QueryByStep(querySql);
    if (!bundleActiveResult) {
        BUNDLE_ACTIVE_LOGE("query by step failed");
        return;
    }
    int32_t tableRowNumber = 0;
    bundleActiveResult->GetRowCount(tableRowNumber);
    int32_t uid;
    string bundleName;
    int32_t changeRow = BUNDLE_ACTIVE_FAIL;
    int64_t firstUseTime = MAX_END_TIME;
    NativeRdb::ValuesBucket valuesBucket;
    for (int32_t i = 0; i < tableRowNumber; i++) {
        bundleActiveResult->GoToRow(i);
        bundleActiveResult->GetInt(BUNDLE_HISTORY_LOG_UID_COLUMN_INDEX, uid);
        bundleActiveResult->GetString(BUNDLE_NAME_COLUMN_INDEX, bundleName);
        string bundleUidKey = BundleActiveUtil::GetBundleUsageKey(bundleName, uid);
        auto it = allBundleFirstUseTime.find(bundleUidKey);
        if (it != allBundleFirstUseTime.end()) {
            firstUseTime = it->second;
        }
        queryCondition.push_back(to_string(userId));
        queryCondition.push_back(bundleName);
        queryCondition.push_back(to_string(uid));
        valuesBucket.PutLong(BUNDLE_ACTIVE_DB_FIRST_USE_TIME, firstUseTime);
        store->Update(changeRow, tableName, valuesBucket, "userId = ? and bundleName = ? and uid = ?", queryCondition);
        queryCondition.clear();
        valuesBucket.Clear();
        changeRow = BUNDLE_ACTIVE_FAIL;
    }
    bundleActiveResult->Close();
}

map<string, int64_t> BundleActiveUsageDatabase::GetAllBundleFirstUseTime(const int32_t userId)
{
    vector<BundleActiveEvent> events = QueryDatabaseEvents(MIN_START_TIME, MAX_END_TIME, userId, "");
    map<string, int64_t> allBundleFirstUseTime;
    for (auto event : events) {
        string bundleUidKey = BundleActiveUtil::GetBundleUsageKey(event.bundleName_, event.uid_);
        auto it = allBundleFirstUseTime.find(bundleUidKey);
        if (it == allBundleFirstUseTime.end()) {
            allBundleFirstUseTime[bundleUidKey] = event.timeStamp_;
            continue;
        }
        allBundleFirstUseTime[bundleUidKey] = std::min(allBundleFirstUseTime[bundleUidKey], event.timeStamp_);
    }

    vector<BundleActivePackageStats> packageStatsVector = QueryDatabaseUsageStats(YEARLY_DATABASE_INDEX,
        MIN_START_TIME, MAX_END_TIME, userId, "");
    for (auto packageStats : packageStatsVector) {
        string bundleUidKey = BundleActiveUtil::GetBundleUsageKey(packageStats.bundleName_, packageStats.uid_);
        auto it = allBundleFirstUseTime.find(bundleUidKey);
        if (it == allBundleFirstUseTime.end()) {
            allBundleFirstUseTime[bundleUidKey] = packageStats.lastTimeUsed_;
            continue;
        }
        allBundleFirstUseTime[bundleUidKey] = std::min(allBundleFirstUseTime[bundleUidKey], packageStats.lastTimeUsed_);
    }
    return allBundleFirstUseTime;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
 
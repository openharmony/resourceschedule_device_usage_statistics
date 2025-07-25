/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include <string>

#include <gtest/gtest.h>
#include "system_ability_definition.h"

#include "bundle_active_usage_database.h"
#include "bundle_active_util.h"
#include "bundle_active_constant.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveUsageDatabaseTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BundleActiveUsageDatabaseTest::SetUpTestCase(void)
{
}

void BundleActiveUsageDatabaseTest::TearDownTestCase(void)
{
}

void BundleActiveUsageDatabaseTest::SetUp(void)
{
}

void BundleActiveUsageDatabaseTest::TearDown(void)
{
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_OnPackageUninstalled_001
 * @tc.desc: OnPackageUninstalled
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_OnPackageUninstalled_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t userId = 0;
    std::string bundleName = "defaultBundleName";
    database->sortedTableArray_ = {{1}};
    database->eventTableName_ = "defaultBundleName";
    database->bundleHistoryTableName_ = "defaultBundleName";
    database->moduleRecordsTableName_ = "defaultBundleName";
    database->formRecordsTableName_ = "defaultBundleName";

    database->OnPackageUninstalled(userId, bundleName, 0, 0);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_GetOptimalIntervalType_001
 * @tc.desc: GetOptimalIntervalType
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_GetOptimalIntervalType_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int64_t beginTime = 0;
    int64_t endTime = 2000000000000000;
    database->sortedTableArray_ = {{0}, {1}};
    database->GetOptimalIntervalType(beginTime, endTime);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_NearIndexOnOrBeforeCurrentTime_001
 * @tc.desc: NearIndexOnOrBeforeCurrentTime
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_NearIndexOnOrBeforeCurrentTime_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int64_t currentTime = 0;
    vector<int64_t> sortedTableArray = {0};
    database->NearIndexOnOrBeforeCurrentTime(currentTime, sortedTableArray);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_CheckDatabaseVersion_001
 * @tc.desc: CheckDatabaseVersion
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_CheckDatabaseVersion_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->CheckDatabaseVersion();
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_GetOldDbVersion_001
 * @tc.desc: GetOldDbVersion
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_GetOldDbVersion_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->GetOldDbVersion();
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_GetVersionByFileInput_001
 * @tc.desc: GetVersionByFileInput
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_GetVersionByFileInput_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    std::string fileInput = "";
    database->GetVersionByFileInput(fileInput);
    fileInput = "aa123";
    int32_t result = database->GetVersionByFileInput(fileInput);
    EXPECT_EQ(result, 123);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_UpgradeDatabase_001
 * @tc.desc: UpgradeDatabase
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_UpgradeDatabase_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->currentVersion_ = 0;
    int32_t oldVersion = 1;
    int32_t curVersion = 2;
    database->UpgradeDatabase(oldVersion, curVersion);
    
    oldVersion = 2;
    curVersion = 3;
    database->UpgradeDatabase(oldVersion, curVersion);
    EXPECT_EQ(database->currentVersion_, curVersion);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_UpgradeDatabase_001
 * @tc.desc: UpgradeDatabase
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_UpgradeDatabase_002,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->currentVersion_ = 0;
    int32_t oldVersion = 1;
    int32_t curVersion = 3;
    database->UpgradeDatabase(oldVersion, curVersion);
    EXPECT_EQ(database->currentVersion_, curVersion);
    database->currentVersion_ = 0;
    oldVersion = 2;
    curVersion = 3;
    database->UpgradeDatabase(oldVersion, curVersion);
    EXPECT_EQ(database->currentVersion_, curVersion);
    oldVersion = 3;
    database->UpgradeDatabase(oldVersion, curVersion);
    EXPECT_EQ(database->currentVersion_, curVersion);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_AddRdbColumn_001
 * @tc.desc: AddRdbColumn
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_AddRdbColumn_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    std::shared_ptr<NativeRdb::RdbStore> store = database->GetBundleActiveRdbStore(0);
    database->AddRdbColumn(store, "test", "testColumn", "INTERGER");
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_UpdateOldDataUid_001
 * @tc.desc: UpdateOldDataUid
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_UpdateOldDataUid_001,
    Function | MediumTest | TestSize.Level0)
{
    std::map<std::string, int32_t> bundleNameUidMap;
    bundleNameUidMap["111"] = 111;
    bundleNameUidMap["222"] = 222;
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    std::shared_ptr<NativeRdb::RdbStore> store = database->GetBundleActiveRdbStore(0);
    database->UpdateOldDataUid(store, "test", 100, bundleNameUidMap);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_CreateRecordTable_001
 * @tc.desc: CreateRecordTable
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_CreateRecordTable_001,
    Function | MediumTest | TestSize.Level0)
{
    int64_t timeStamp = 10000;
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->moduleRecordsTableName_ = "unknownTableName";
    database->formRecordsTableName_ = "unknownTableName";
    database->CreateRecordTable(timeStamp);;
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_GetQuerySqlCommand_001
 * @tc.desc: GetQuerySqlCommand
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
 HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_GetQuerySqlCommand_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<std::string> queryCondition;
    std::string queryPackageSql;
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->sortedTableArray_ = {{0}, {1}, {2}};
    database->GetQuerySqlCommand(0, 1, 2, 0, 0, 1, 10, queryCondition, queryPackageSql);
    EXPECT_TRUE(!queryPackageSql.empty());
}

/*
 * @tc.name: BundleActiveUtil_001
 * @tc.desc: CreateRecordTable
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
 HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUtil_001,
    Function | MediumTest | TestSize.Level0)
{
    int32_t result = BundleActiveUtil::StringToInt32("11");
    EXPECT_EQ(result, 11);
    int64_t resultInt64 = BundleActiveUtil::StringToInt64("11");
    EXPECT_EQ(resultInt64, 11);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_SupportFirstUseTime_001
 * @tc.desc: SupportFirstUseTime
 * @tc.type: FUNC
 * @tc.require: issuesICCZ27
 */
HWTEST_F(BundleActiveUsageDatabaseTest, SupportFirstUseTime_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    
    database->SupportFirstUseTime();
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_UpdateFirstUseTime_002
 * @tc.desc: UpdateFirstUseTime
 * @tc.type: FUNC
 * @tc.require: issuesICCZ27
 */
HWTEST_F(BundleActiveUsageDatabaseTest, UpdateFirstUseTime_002,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    auto rdb = database->GetBundleActiveRdbStore(APP_GROUP_DATABASE_INDEX);
    database->UpdateFirstUseTime(rdb, BUNDLE_HISTORY_LOG_TABLE, 100);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_DeleteExcessiveEventTableData_001
 * @tc.desc: DeleteExcessiveEventTableData
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_DeleteExcessiveEventTableData_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->DeleteExcessiveEventTableData(0);
    database->eventTableName_ = "a123";
    database->DeleteExcessiveEventTableData(0);
    database->eventBeginTime_  = 0;
    database->DeleteExcessiveEventTableData(0);
    database->eventBeginTime_  = MAX_END_TIME;
    database->DeleteExcessiveEventTableData(0);
    database->InitDatabaseTableInfo(100);
    database->DeleteExcessiveEventTableData(0);
    EXPECT_EQ(database->eventTableName_, "a123");
}

/*
 * @tc.name: GetSteadyTime_001
 * @tc.desc: BundleActiveUtil Test
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveUsageDatabaseTest, GetSteadyTime_001,
    Function | MediumTest | TestSize.Level0)
{
    EXPECT_TRUE(BundleActiveUtil::GetSteadyTime() > 0);
}

/*
 * @tc.name: GetFolderOrFileSize_001
 * @tc.desc: BundleActiveUtil Test
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveUsageDatabaseTest, GetFolderOrFileSize_001,
    Function | MediumTest | TestSize.Level0)
{
    EXPECT_EQ(BundleActiveUtil::GetFolderOrFileSize(""), 0);
    EXPECT_TRUE(BundleActiveUtil::GetFolderOrFileSize(BUNDLE_ACTIVE_DATABASE_DIR) > 0);
}

/*
 * @tc.name: GetPartitionName_001
 * @tc.desc: BundleActiveUtil Test
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveUsageDatabaseTest, GetPartitionName_001,
    Function | MediumTest | TestSize.Level0)
{
    EXPECT_EQ(BundleActiveUtil::GetPartitionName(""), "/");
    EXPECT_EQ(BundleActiveUtil::GetPartitionName(BUNDLE_ACTIVE_DATABASE_DIR), "/data");
    EXPECT_EQ(BundleActiveUtil::GetPartitionName("/data"), "/data");
}

/*
 * @tc.name: GetDeviceValidSize_001
 * @tc.desc: BundleActiveUtil Test
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveUsageDatabaseTest, GetDeviceValidSize_001,
    Function | MediumTest | TestSize.Level0)
{
    EXPECT_EQ(BundleActiveUtil::GetDeviceValidSize(""), 0);
    EXPECT_TRUE(BundleActiveUtil::GetDeviceValidSize(BUNDLE_ACTIVE_DATABASE_DIR) > 0);
}

/*
 * @tc.name: GetPercentOfAvailableUserSpace_001
 * @tc.desc: BundleActiveUtil Test
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveUsageDatabaseTest, GetPercentOfAvailableUserSpace_001,
    Function | MediumTest | TestSize.Level0)
{
    EXPECT_EQ(BundleActiveUtil::GetPercentOfAvailableUserSpace(""), 0);
    EXPECT_TRUE(BundleActiveUtil::GetPercentOfAvailableUserSpace(BUNDLE_ACTIVE_DATABASE_DIR) > 0);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
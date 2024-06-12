/*
 * Copyright (c) 2022  Huawei Device Co., Ltd.
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
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    std::string fileInput = "";
    database->GetVersionByFileInput(fileInput);
    fileInput = "aa123";
    int32_t result = database->GetVersionByFileInput(fileInput);
    EXPECT_EQ(result, 123);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_UpgradleDatabase_001
 * @tc.desc: UpgradleDatabase
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_UpgradleDatabase_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t oldVersion = 1;
    int32_t curVersion = 2;
    database->UpgradleDatabase(oldVersion, curVersion);
    oldVersion = 2;
    curVersion = 2;
    database->UpgradleDatabase(oldVersion, curVersion);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_AddRdbColumn_001
 * @tc.desc: AddRdbColumn
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(BundleActiveUsageDatabaseTest, BundleActiveUsageDatabaseTest_AddRdbColumn_001,
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
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
    Function | MediumTest | Level0)
{
    int64_t timeStamp = 10000;
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->moduleRecordsTableName_ = "unknownTableName";
    database->formRecordsTableName_ = "unknownTableName";
    database->CreateRecordTable(timeStamp);;
    EXPECT_NE(database, nullptr);
}

}  // namespace DeviceUsageStats
}  // namespace OHOS
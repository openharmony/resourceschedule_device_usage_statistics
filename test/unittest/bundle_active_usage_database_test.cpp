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
    std::string bundleName = "defaultBundleName";std::vector<std::vector<int64_t>>
    database->sortedTableArray_ = {{1}};
    database->eventTableName_ = "defaultBundleName";
    database->bundleHistoryTableName_ = "defaultBundleName";
    database->moduleRecordsTableName_ = "defaultBundleName";
    database->formRecordsTableName_ = "defaultBundleName";

    database->OnPackageUninstalled(userId, bundleName);
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
    int64_t currentTime = 0,
    vector<int64_t> sortedTableArray {0};
    database->NearIndexOnOrBeforeCurrentTime(currentTime, sortedTableArray);
    EXPECT_NE(database, nullptr);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
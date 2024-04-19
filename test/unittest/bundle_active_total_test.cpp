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

#include "bundle_active_power_state_callback_proxy.h"
#include "bundle_active_stub.h"
#include "bundle_active_core.h"
#include "bundle_active_power_state_callback_stub.h"
#include "bundle_active_continuous_task_observer.h"
#include "bundle_active_bundle_mgr_helper.h"
#include "bundle_active_app_state_observer.h"
#include "bundle_active_usage_database.h"
#include "bundle_active_account_helper.h"
#include "bundle_active_power_state_callback_service.h"
#include "bundle_active_open_callback.h"
#include "bundle_active_shutdown_callback_service.h"
#include "bundle_active_binary_search.h"
#include "bundle_active_debug_mode.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {

class BundleActiveTotalTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BundleActiveTotalTest::SetUpTestCase(void)
{
}

void BundleActiveTotalTest::TearDownTestCase(void)
{
}

void BundleActiveTotalTest::SetUp(void)
{
}

void BundleActiveTotalTest::TearDown(void)
{
}

/*
 * @tc.name: BundleActiveAccountHelperTest_001
 * @tc.desc: test the interface of bundle_active_account_helper
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveAccountHelperTest_001, Function | MediumTest | Level0)
{
    // Given
    std::vector<int32_t> activatedOsAccountIds;

    // When
    BundleActiveAccountHelper::GetActiveUserId(activatedOsAccountIds);

    // Then
    SUCCEED();
}

/*
 * @tc.name: BundleActiveContinuousTaskObserverTest_001
 * @tc.desc: test the interface of bundle_active_continuous_task_observer
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveContinuousTaskObserverTest_001, Function | MediumTest | Level0)
{
#ifdef BGTASKMGR_ENABLE
#ifdef OS_ACCOUNT_PART_ENABLED
    std::shared_ptr<AppExecFwk::EventRunner> runner;
    auto reportHandler = std::make_shared<BundleActiveReportHandler>(runner);
    auto reportHandler1 = std::make_shared<BundleActiveReportHandler>(runner);
    BundleActiveContinuousTaskObserver test;
    test.Init(reportHandler);
    test.Init(reportHandler1);

    std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo> continuousTaskCallbackInfo;
    test.OnContinuousTaskStart(continuousTaskCallbackInfo);

    test.ReportContinuousTaskEvent(continuousTaskCallbackInfo, true);
    test.ReportContinuousTaskEvent(continuousTaskCallbackInfo, false);

    test.GetBundleMgr();
    test.bundleMgr_ = nullptr;
    test.GetBundleMgr();
#endif
#endif
}

/*
 * @tc.name: BundleActiveBundleMgrHelperTest_001
 * @tc.desc: test the interface of bundle_active_bundle_mgr_helper
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveBundleMgrHelperTest_001, Function | MediumTest | Level0)
{
    AppExecFwk::ApplicationFlag flag = AppExecFwk::GET_BASIC_APPLICATION_INFO;
    AppExecFwk::ApplicationInfo appInfo;
    AppExecFwk::BundleInfo bundleInfo;
    BundleActiveBundleMgrHelper test;

    std::string string = "test";
    test.GetNameForUid(0, string);
    test.GetApplicationInfo(string, flag, 0, appInfo);
    test.GetBundleInfo(string, AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, 0);

    test.bundleMgr_ = nullptr;
    test.GetNameForUid(0, string);
    test.GetApplicationInfo(string, flag, 0, appInfo);
    test.GetBundleInfo(string, AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, 0);
}

/*
 * @tc.name: BundleActiveAppStateObserverTest_001
 * @tc.desc: test the interface of bundle_active_app_state_observer
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveAppStateObserverTest_001, Function | MediumTest | Level0)
{
    AbilityStateData abilityStateData;
    std::shared_ptr<AppExecFwk::EventRunner> runner;
    auto reportHandler = std::make_shared<BundleActiveReportHandler>(runner);
    BundleActiveAppStateObserver test;
    test.Init(reportHandler);
    test.OnAbilityStateChanged(abilityStateData);

    test.Init(reportHandler);
    abilityStateData.abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
    test.OnAbilityStateChanged(abilityStateData);

    abilityStateData.abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
    test.OnAbilityStateChanged(abilityStateData);

    abilityStateData.abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED);
    test.OnAbilityStateChanged(abilityStateData);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_001
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveUsageDatabaseTest_001, Function | MediumTest | Level0)
{
    BundleActiveUsageDatabase test;
    test.InitDatabaseTableInfo(test.ParseStartTime(test.eventTableName_)-1);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_002
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveUsageDatabaseTest_002, Function | MediumTest | Level0)
{
    BundleActiveUsageDatabase test;
    test.debugDatabase_ = true;
    test.DeleteExcessiveTableData(100);
    test.DeleteExcessiveTableData(0);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_003
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveUsageDatabaseTest_003, Function | MediumTest | Level0)
{
    BundleActiveUsageDatabase test;
    test.GetOverdueTableCreateTime(100, 0);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_005
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveUsageDatabaseTest_005, Function | MediumTest | Level0)
{
    BundleActiveUsageDatabase test;
    test.bundleHistoryTableName_ = "unknownTableName";
    std::shared_ptr<map<string, std::shared_ptr<BundleActivePackageHistory>>> userHistory;
    test.PutBundleHistoryData(0, userHistory);
    test.GetTableIndexSql(0, 0, true, 0);
    test.GetTableIndexSql(5, 0, true, 0);
    test.GetTableIndexSql(5, 0, false, 0);
    test.GetTableIndexSql(5, 0, false, 1);
    test.GetTableIndexSql(5, 0, false, 2);
    test.GetTableIndexSql(0, 0, false, 0);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_006
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveUsageDatabaseTest_006, Function | MediumTest | Level0)
{
    BundleActiveUsageDatabase test;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore;
    test.SetNewIndexWhenTimeChanged(5, 0, 0, rdbStore);
    test.RenameTableName(0, 0, 0);
    test.ExecuteRenameTableName("test", 0, 0, rdbStore);
}

/*
 * @tc.name: BundleActiveUsageDatabaseTest_007
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveUsageDatabaseTest_007, Function | MediumTest | Level0)
{
    BundleActiveUsageDatabase test;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore;
    BundleActiveFormRecord formRecord;
    test.UpdateFormData(0, "test", "test", formRecord, rdbStore);
    test.GetSystemEventName(0);
    test.JudgeQueryCondition(0, 0, 1);
}

/*
 * @tc.name: BundleActivePowerStateCallbackServiceTest_001
 * @tc.desc: test the interface of bundle_active_usage_database
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActivePowerStateCallbackServiceTest_001, Function | MediumTest | Level0)
{
#ifdef DEVICE_USAGES_STATISTICS_POWERMANGER_ENABLE
    BundleActivePowerStateCallbackService test1(nullptr);

    std::shared_ptr<BundleActiveCore> bundleActiveCore = std::make_shared<BundleActiveCore>();
    BundleActivePowerStateCallbackService test2(bundleActiveCore);
    
    test2.OnPowerStateChanged(PowerState::AWAKE);
    test2.OnPowerStateChanged(PowerState::SLEEP);
#endif
}

/*
 * @tc.name: BundleActiveBinarySearchTest_001
 * @tc.desc: test the interface of bundle_active_binary_search
 * @tc.type: FUNC
 * @tc.require: issuesI9BMBP
 */
HWTEST_F(BundleActiveTotalTest, BundleActiveBinarySearchTest_001, Function | MediumTest | Level0)
{
    std::vector<int64_t> tableNameArray;
    BundleActiveBinarySearch test;
    test.BinarySearch(tableNameArray, 0);
    test.BinarySearch(tableNameArray, -100);
}
}
}
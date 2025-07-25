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

#include <gtest/gtest.h>
#include "system_ability_definition.h"

#include "bundle_active_client.h"
#include "bundle_active_event.h"
#include "bundle_active_service.h"
#include "bundle_active_group_map.h"
#include "app_group_callback_stub.h"
#include "app_group_callback_info.h"
#include "iapp_group_callback.h"
#include "bundle_active_constant.h"
#include "bundle_active_usage_database.h"
#include "bundle_active_test_util.h"
#include "bundle_active_high_frequency_period.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
static std::string g_defaultBundleName = "com.ohos.camera";
static std::string g_defaultMoudleName = "defaultmodulename";
static std::string g_defaultFormName = "defaultformname";
static int32_t g_defaultDimension = 4;
static int64_t g_defaultFormId = 1;
static int32_t g_defaultUserId = 0;
static int32_t g_commonUserid = 100;
static int64_t g_largeNum = 20000000000000;
static int32_t g_defaultGroup = 10;
static int32_t g_intervalType = 4;
static sptr<IAppGroupCallback> observer = nullptr;

class DeviceUsageStatisticsMockTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DeviceUsageStatisticsMockTest::SetUpTestCase(void)
{
    BundleActiveTestUtil::TestInit();
}

void DeviceUsageStatisticsMockTest::TearDownTestCase(void)
{
    BundleActiveTestUtil::TestDeInit();
}

void DeviceUsageStatisticsMockTest::SetUp(void)
{
}

void DeviceUsageStatisticsMockTest::TearDown(void)
{
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_GetBundleActiveProxy_001
 * @tc.desc: test client getBundleActiveProxy boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_GetBundleActiveProxy_001,
    Function | MediumTest | TestSize.Level0)
{
    BundleActiveEvent eventA(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        g_defaultDimension, g_defaultFormId, BundleActiveEvent::FORM_IS_CLICKED);
    EXPECT_NE(BundleActiveClient::GetInstance().ReportEvent(eventA, g_defaultUserId), ERR_OK);

    bool isIdle = false;
    bool isUsePeriod = false;
    EXPECT_NE(BundleActiveClient::GetInstance().IsBundleIdle(isIdle, g_defaultBundleName, g_defaultUserId), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().IsBundleUsePeriod(isUsePeriod, g_defaultBundleName, g_defaultUserId),
        ERR_OK);

    std::vector<BundleActivePackageStats> packageStats;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats,
        g_intervalType, 0, g_largeNum), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleStatsInfos(packageStats,
        g_intervalType, 0, g_largeNum), ERR_OK);

    std::vector<BundleActiveEvent> event;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleEvents(event, 0, g_largeNum, 100), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryCurrentBundleEvents(event, 0, g_largeNum), ERR_OK);

    int32_t result = 0;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryAppGroup(result, g_defaultBundleName, g_commonUserid), ERR_OK);

    EXPECT_NE(BundleActiveClient::GetInstance().SetAppGroup(
        g_defaultBundleName, g_defaultGroup, g_commonUserid), ERR_OK);

    EXPECT_NE(BundleActiveClient::GetInstance().RegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(observer), ERR_OK);

    std::vector<BundleActiveEventStats> eventStats;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryDeviceEventStats(0, g_largeNum, eventStats), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryNotificationEventStats(0, g_largeNum, eventStats), ERR_OK);

    std::vector<BundleActiveModuleRecord> moduleRecord;
    int32_t maxNum = 1000;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, moduleRecord, g_defaultUserId), ERR_OK);
    std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryHighFrequencyPeriodBundle(appFreqHours, g_defaultUserId), ERR_OK);
    int64_t latestUsedTime;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleTodayLatestUsedTime(
        latestUsedTime, g_defaultBundleName, g_commonUserid), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_GetBundleMgrProxy_001
 * @tc.desc: test service getBundleMgrProxy boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_GetBundleMgrProxy_001,
    Function | MediumTest | TestSize.Level0)
{
    std::string bundleName;
    bool isBundleIdle = false;
    bool isUsePeriod = false;
    ErrCode code = DelayedSingleton<BundleActiveService>::GetInstance()->IsBundleIdle(isBundleIdle, bundleName, -1);
    EXPECT_NE(code, ERR_OK);
    code = DelayedSingleton<BundleActiveService>::GetInstance()->
        IsBundleUsePeriod(isUsePeriod, bundleName, -1);
    EXPECT_NE(code, ERR_OK);

    code = DelayedSingleton<BundleActiveService>::GetInstance()->CheckBundleIsSystemAppAndHasPermission(100, 100000);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryDeviceEventStats_001
 * @tc.desc: test service queryDeviceEventStats boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryDeviceEventStats_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActiveEventStats> eventStats;
    ErrCode code =
        DelayedSingleton<BundleActiveService>::GetInstance()->QueryDeviceEventStats(0, g_largeNum, eventStats, -1);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryNotificationEventStats_001
 * @tc.desc: test service queryNotificationEventStats boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryNotificationEventStats_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActiveEventStats> eventStats;
    ErrCode code = DelayedSingleton<BundleActiveService>::GetInstance()->QueryNotificationEventStats(
        0, g_largeNum, eventStats, -1);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryModuleUsageRecords_001
 * @tc.desc: test service queryModuleUsageRecords boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryModuleUsageRecords_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActiveModuleRecord> records;
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->QueryModuleUsageRecords(1000, records, -1), ERR_OK);

    BundleActiveModuleRecord bundleActiveModuleRecord;
    DelayedSingleton<BundleActiveService>::GetInstance()->QueryModuleRecordInfos(bundleActiveModuleRecord);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryAppGroup_001
 * @tc.desc: test service queryAppGroup boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryAppGroup_001,
    Function | MediumTest | TestSize.Level0)
{
    int32_t appGroup;
    std::string bundleName;
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->QueryAppGroup(appGroup, bundleName, -1), ERR_OK);
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->QueryAppGroup(appGroup, bundleName, 100), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_SetAppGroup_001
 * @tc.desc: test service SetAppGroup boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_SetAppGroup_001,
    Function | MediumTest | TestSize.Level0)
{
    int32_t appGroup = 100;
    std::string bundleName;
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->SetAppGroup(bundleName, appGroup, -1), ERR_OK);
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->SetAppGroup(bundleName, appGroup, 100), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_AppGroupCallback_001
 * @tc.desc: test service appGroupCallback boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_AppGroupCallback_001,
    Function | MediumTest | TestSize.Level0)
{
    sptr<IAppGroupCallback> observer = nullptr;
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_ = nullptr;
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->RegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->UnRegisterAppGroupCallBack(observer), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleEvents_001
 * @tc.desc: test service queryBundleEvents boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleEvents_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActiveEvent> bundleActiveEvents;
    ErrCode code = DelayedSingleton<BundleActiveService>::GetInstance()
        ->QueryBundleEvents(bundleActiveEvents, 0, g_largeNum, -1);
    EXPECT_NE(code, ERR_OK);
    code = DelayedSingleton<BundleActiveService>::GetInstance()
        ->QueryCurrentBundleEvents(bundleActiveEvents, 0, g_largeNum);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleStatsInfoByInterval_001
 * @tc.desc: test service queryBundleStatsInfoByInterval boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleStatsInfoByInterval_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActivePackageStats> packageStats;
    ErrCode code = DelayedSingleton<BundleActiveService>::GetInstance()
        ->QueryBundleStatsInfoByInterval(packageStats, 0, 0, g_largeNum, -1);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleStatsInfos_001
 * @tc.desc: test service queryBundleStatsInfos boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleStatsInfos_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActivePackageStats> packageStats;
    ErrCode code = DelayedSingleton<BundleActiveService>::GetInstance()
        ->QueryBundleStatsInfos(packageStats, 0, 0, g_largeNum);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleStatsInfos_001
 * @tc.desc: test service queryBundleStatsInfos boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_CheckTimeChangeAndGetWallTime_001,
    Function | MediumTest | TestSize.Level0)
{
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();

    BundleActiveEvent event;
    event.bundleName_ = "com.ohos.settings";
    int32_t userId = 101;
    EXPECT_NE(bundleActiveCore->ReportEvent(event, userId), ERR_OK);

    EXPECT_NE(bundleActiveCore->ReportEventToAllUserId(event), ERR_OK);

    std::vector<BundleActiveEventStats> eventStats;
    EXPECT_NE(bundleActiveCore->QueryNotificationEventStats(0, g_largeNum, eventStats, g_defaultUserId), ERR_OK);
    EXPECT_NE(bundleActiveCore->QueryDeviceEventStats(0, g_largeNum, eventStats, g_defaultUserId), ERR_OK);

    std::vector<BundleActiveModuleRecord> moduleRecords;
    EXPECT_NE(bundleActiveCore->QueryModuleUsageRecords(1000, moduleRecords, g_defaultUserId), ERR_OK);

    std::vector<BundleActiveEvent> activeEvent;
    ErrCode code = bundleActiveCore->QueryBundleEvents(
        activeEvent, g_defaultUserId, 0, g_largeNum, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);

    std::vector<BundleActivePackageStats> packageStats;
    code = bundleActiveCore->QueryBundleStatsInfos(
        packageStats, g_defaultUserId, g_intervalType, 0, g_largeNum, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);
    
    BundleActiveEvent eventTemp;
    EXPECT_NE(bundleActiveCore->ReportEventToAllUserId(eventTemp), ERR_OK);
    int32_t uid = 0;
    int32_t appIndex = 0;
    bundleActiveCore->OnBundleUninstalled(g_defaultUserId, g_defaultBundleName, uid, appIndex);

    EXPECT_NE(bundleActiveCore->QueryNotificationEventStats(0, g_largeNum, eventStats, g_commonUserid), ERR_OK);
    EXPECT_NE(bundleActiveCore->QueryDeviceEventStats(0, g_largeNum, eventStats, g_commonUserid), ERR_OK);
    EXPECT_NE(bundleActiveCore->QueryModuleUsageRecords(1000, moduleRecords, g_commonUserid), ERR_OK);

    code = bundleActiveCore->QueryBundleEvents(activeEvent, g_commonUserid, 0, g_largeNum, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);

    code = bundleActiveCore->QueryBundleStatsInfos(packageStats, g_commonUserid,
        g_intervalType, 0, g_largeNum, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);
    
    bundleActiveCore->OnBundleUninstalled(g_commonUserid, g_defaultBundleName, uid, appIndex);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_getUserHistory_001
 * @tc.desc: test service getUserHistory boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_getUserHistory_001,
    Function | MediumTest | TestSize.Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>();
    const int64_t timeStamp = g_largeNum;
    int32_t uid = 0;
    int32_t appIndex = 0;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    groupController->bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(timeStamp, bundleActiveCore);

    groupController->OnBundleUninstalled(0, g_defaultBundleName, uid, appIndex);
    EXPECT_NE(groupController, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_calculationTimeOut_001
 * @tc.desc: test service calculationTimeOut boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_calculationTimeOut_001,
    Function | MediumTest | TestSize.Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>();

    std::shared_ptr<BundleActivePackageHistory> history = nullptr;
    groupController->calculationTimeOut(history, g_largeNum);
    history = std::make_shared<BundleActivePackageHistory>();
    groupController->calculationTimeOut(history, g_largeNum);
    EXPECT_NE(groupController, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryStatsInfoByStep_001
 * @tc.desc: test QueryStatsInfoByStep
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryStatsInfoByStep_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int32_t databaseType = DAILY_DATABASE_INDEX;
    bool forModuleRecords = true;
    database->InitUsageGroupDatabase(databaseType, forModuleRecords);

    database->HandleTableInfo(databaseType);
    int64_t currentTimeMillis = 20000000000000;
    database->GetOverdueTableCreateTime(databaseType, currentTimeMillis);

    int32_t userId = 100;
    database->GetBundleHistoryData(userId);

    database->GetDurationData();

    database->GetCurrentUsageData(databaseType, userId);

    int64_t beginTime = 0;
    int64_t endTime = 20000000000000;
    database->QueryDatabaseUsageStats(databaseType, beginTime, endTime, userId);

    std::string bundleName = "defaultBundleName";
    database->QueryDatabaseEvents(beginTime, endTime, userId, bundleName);

    std::map<std::string, std::shared_ptr<BundleActiveModuleRecord>> moduleRecords;
    database->LoadModuleData(userId, moduleRecords);

    database->LoadFormData(userId, moduleRecords);

    std::map<std::string, BundleActiveEventStats> eventStats;
    int32_t eventId = 2;
    database->QueryDeviceEventStats(eventId, beginTime, endTime, eventStats, userId);

    database->QueryNotificationEventStats(eventId, beginTime, endTime, eventStats, userId);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_GetBundleActiveRdbStore_001
 * @tc.desc: test GetBundleActiveRdbStore
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_GetBundleActiveRdbStore_001,
    Function | MediumTest | TestSize.Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t databaseType = EVENT_DATABASE_INDEX;
    database->DeleteExcessiveTableData(databaseType);
    database->DeleteInvalidTable(databaseType, 0);
    database->CreateEventLogTable(databaseType, 0);
    database->CreatePackageLogTable(databaseType, 0);
    database->CreateModuleRecordTable(databaseType, 0);
    database->CreateFormRecordTable(databaseType, 0);
    database->CreateDurationTable(databaseType);
    database->CreateBundleHistoryTable(databaseType);

    int32_t userId = 100;
    auto userHistory = std::make_shared<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>>();
    database->PutBundleHistoryData(userId, userHistory);

    int64_t bootBasedDuration = 0;
    int64_t screenOnDuration = 20000000000;
    database->PutDurationData(bootBasedDuration, screenOnDuration);

    BundleActivePeriodStats stats;
    database->FlushPackageInfo(databaseType, stats);
    database->FlushEventInfo(databaseType, stats);
    database->RenameTableName(databaseType, 0, 0);
    std::string bundleName = "defaultBundleName";
    std::string tableName = "defaultTableName";
    int32_t uid = 0;
    int32_t appIndex = 0;
    database->DeleteUninstalledInfo(userId, bundleName, uid, tableName, databaseType, appIndex);

    auto moduleRecords = std::map<std::string, std::shared_ptr<BundleActiveModuleRecord>>();
    int64_t timeStamp = 20000000000;
    database->UpdateModuleData(userId, moduleRecords, timeStamp);

    std::string moduleName = "defaultMoudleName";
    std::string formName = "defaultFormName";
    database->RemoveFormData(userId, bundleName, moduleName, formName, 0, 0, uid);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryHighFrequencyPeriodBundle_001
 * @tc.desc: test QueryHighFrequencyPeriodBundle
 * @tc.type: FUNC
 * @tc.require: SR20250319441801 AR20250322520501
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryHighFrequencyPeriodBundle_001,
    Function | MediumTest | TestSize.Level0)
{
    std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
    ErrCode code =
        DelayedSingleton<BundleActiveService>::GetInstance()->QueryHighFrequencyPeriodBundle(appFreqHours, 0);
    EXPECT_NE(code, ERR_OK);

    code =
        DelayedSingleton<BundleActiveService>::GetInstance()->QueryHighFrequencyPeriodBundle(appFreqHours, -1);
    EXPECT_NE(code, ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleTodayLatestUsedTime_001
 * @tc.desc: test QueryHighFrequencyPeriodBundle
 * @tc.type: FUNC
 * @tc.require: SR20250319441801 AR20250322520501
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleTodayLatestUsedTime_001,
    Function | MediumTest | TestSize.Level0)
{
    int64_t latestUsedTime = 0;
    ErrCode code =
        DelayedSingleton<BundleActiveService>::GetInstance()->QueryBundleTodayLatestUsedTime(latestUsedTime, "test", 0);
    EXPECT_NE(code, ERR_OK);
    EXPECT_EQ(latestUsedTime, 0);
    code = DelayedSingleton<BundleActiveService>::GetInstance()->QueryBundleTodayLatestUsedTime(
        latestUsedTime, "test", -1);
    EXPECT_NE(code, ERR_OK);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


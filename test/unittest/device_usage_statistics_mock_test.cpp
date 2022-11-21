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

#define private public
#define protected public

#include <gtest/gtest.h>
#include "system_ability_definition.h"

#include "bundle_active_client.h"
#include "bundle_active_event.h"
#include "bundle_active_service.h"
#include "bundle_active_group_map.h"
#include "app_group_callback_stub.h"
#include "app_group_callback_info.h"
#include "iapp_group_callback.h"


using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
static std::string g_defaultBundleName = "com.ohos.camera";
static std::string g_defaultMoudleName = "defaultmodulename";
static std::string g_defaultFormName = "defaultformname";
static int32_t DEFAULT_DIMENSION = 4;
static int64_t DEFAULT_FORMID = 1;
static int32_t DEFAULT_USERID = 0;
static int32_t COMMON_USERID = 100;
static int64_t LARGE_NUM = 20000000000000;
static int32_t DEFAULT_GROUP = 10;
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
}

void DeviceUsageStatisticsMockTest::TearDownTestCase(void)
{
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
    Function | MediumTest | Level0)
{
    BundleActiveEvent eventA(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
    EXPECT_NE(BundleActiveClient::GetInstance().ReportEvent(eventA, DEFAULT_USERID), ERR_OK);

    bool isIdle = false;
    EXPECT_NE(BundleActiveClient::GetInstance().IsBundleIdle(isIdle, g_defaultBundleName, DEFAULT_USERID), ERR_OK);

    std::vector<BundleActivePackageStats> packageStats;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats, 4, 0, LARGE_NUM), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleStatsInfos(packageStats, 4, 0, LARGE_NUM), ERR_OK);

    std::vector<BundleActiveEvent> event;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleEvents(event, 0, LARGE_NUM, 100), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryCurrentBundleEvents(event, 0, LARGE_NUM), ERR_OK);

    int32_t result = 0;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryAppGroup(result, g_defaultBundleName, COMMON_USERID), ERR_OK);

    EXPECT_NE(BundleActiveClient::GetInstance().SetAppGroup(g_defaultBundleName, DEFAULT_GROUP, COMMON_USERID), ERR_OK);

    EXPECT_NE(BundleActiveClient::GetInstance().RegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(observer), ERR_OK);

    std::vector<BundleActiveEventStats> eventStats;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryDeviceEventStats(0, LARGE_NUM, eventStats), ERR_OK);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryNotificationEventStats(0, LARGE_NUM, eventStats), ERR_OK);

    std::vector<BundleActiveModuleRecord> moduleRecord;
    int32_t maxNum = 1000;
    EXPECT_NE(BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, moduleRecord, DEFAULT_USERID), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_GetBundleMgrProxy_001
 * @tc.desc: test service getBundleMgrProxy boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_GetBundleMgrProxy_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();
    std::string bundleName;
    bool isBundleIdle = false;
    EXPECT_NE(service->IsBundleIdle(isBundleIdle, bundleName, -1), ERR_OK);
    EXPECT_NE(service->CheckBundleIsSystemAppAndHasPermission(100, 100000), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryDeviceEventStats_001
 * @tc.desc: test service queryDeviceEventStats boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryDeviceEventStats_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    std::vector<BundleActiveEventStats> eventStats;
    EXPECT_NE(service->QueryDeviceEventStats(0, LARGE_NUM, eventStats, -1), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryNotificationEventStats_001
 * @tc.desc: test service queryNotificationEventStats boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryNotificationEventStats_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    std::vector<BundleActiveEventStats> eventStats;
    EXPECT_NE(service->QueryNotificationEventStats(0, LARGE_NUM, eventStats, -1), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryModuleUsageRecords_001
 * @tc.desc: test service queryModuleUsageRecords boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryModuleUsageRecords_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    std::vector<BundleActiveModuleRecord> records;
    EXPECT_NE(service->QueryModuleUsageRecords(1000, records, -1), ERR_OK);

    BundleActiveModuleRecord bundleActiveModuleRecord;
    service->QueryModuleRecordInfos(bundleActiveModuleRecord);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryAppGroup_001
 * @tc.desc: test service queryAppGroup boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryAppGroup_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    int32_t appGroup;
    std::string bundleName;
    EXPECT_NE(service->QueryAppGroup(appGroup, bundleName, -1), ERR_OK);
    EXPECT_NE(service->QueryAppGroup(appGroup, bundleName, 100), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_SetAppGroup_001
 * @tc.desc: test service SetAppGroup boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_SetAppGroup_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    int32_t appGroup = 100;
    std::string bundleName;
    EXPECT_NE(service->SetAppGroup(bundleName, appGroup, -1), ERR_OK);
    EXPECT_NE(service->SetAppGroup(bundleName, appGroup, 100), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_AppGroupCallback_001
 * @tc.desc: test service appGroupCallback boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_AppGroupCallback_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    sptr<IAppGroupCallback> observer = nullptr;
    service->bundleActiveCore_ = nullptr;
    EXPECT_NE(service->RegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(service->UnRegisterAppGroupCallBack(observer), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleEvents_001
 * @tc.desc: test service queryBundleEvents boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleEvents_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    std::vector<BundleActiveEvent> bundleActiveEvents;
    EXPECT_NE(service->QueryBundleEvents(bundleActiveEvents, 0, LARGE_NUM, -1), ERR_OK);
    EXPECT_NE(service->QueryCurrentBundleEvents(bundleActiveEvents, 0, LARGE_NUM), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleStatsInfoByInterval_001
 * @tc.desc: test service queryBundleStatsInfoByInterval boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleStatsInfoByInterval_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    std::vector<BundleActivePackageStats> packageStats;
    EXPECT_NE(service->QueryBundleStatsInfoByInterval(packageStats, 0, 0, LARGE_NUM, -1), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleStatsInfos_001
 * @tc.desc: test service queryBundleStatsInfos boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_QueryBundleStatsInfos_001,
    Function | MediumTest | Level0)
{
    auto service = std::make_shared<BundleActiveService>();

    std::vector<BundleActivePackageStats> packageStats;
    EXPECT_NE(service->QueryBundleStatsInfos(packageStats, 0, 0, LARGE_NUM), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_QueryBundleStatsInfos_001
 * @tc.desc: test service queryBundleStatsInfos boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_CheckTimeChangeAndGetWallTime_001,
    Function | MediumTest | Level0)
{
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();

    BundleActiveEvent event;
    event.bundleName_ = "com.ohos.settings";
    int32_t userId = 101;
    EXPECT_NE(bundleActiveCore->ReportEvent(event, userId), ERR_OK);

    EXPECT_NE(bundleActiveCore->ReportEventToAllUserId(event), ERR_OK);

    std::vector<BundleActiveEventStats> eventStats;
    EXPECT_NE(bundleActiveCore->QueryNotificationEventStats(0, LARGE_NUM, eventStats, DEFAULT_USERID), ERR_OK);
    EXPECT_NE(bundleActiveCore->QueryDeviceEventStats(0, LARGE_NUM, eventStats, DEFAULT_USERID), ERR_OK);

    std::vector<BundleActiveModuleRecord> moduleRecords;
    EXPECT_NE(bundleActiveCore->QueryModuleUsageRecords(1000, moduleRecords, DEFAULT_USERID), ERR_OK);

    std::vector<BundleActiveEvent> activeEvent;
    ErrCode code = bundleActiveCore->QueryBundleEvents(activeEvent, DEFAULT_USERID, 0, LARGE_NUM, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);

    std::vector<BundleActivePackageStats> packageStats;
    code = bundleActiveCore->QueryBundleStatsInfos(packageStats, DEFAULT_USERID, 4, 0, LARGE_NUM, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);
    
    BundleActiveEvent eventTemp;
    EXPECT_NE(bundleActiveCore->ReportEventToAllUserId(eventTemp), ERR_OK);

    bundleActiveCore->OnBundleUninstalled(DEFAULT_USERID, g_defaultBundleName);

    EXPECT_NE(bundleActiveCore->QueryNotificationEventStats(0, LARGE_NUM, eventStats, COMMON_USERID), ERR_OK);
    EXPECT_NE(bundleActiveCore->QueryDeviceEventStats(0, LARGE_NUM, eventStats, COMMON_USERID), ERR_OK);
    EXPECT_NE(bundleActiveCore->QueryModuleUsageRecords(1000, moduleRecords, COMMON_USERID), ERR_OK);

    code = bundleActiveCore->QueryBundleEvents(activeEvent, COMMON_USERID, 0, LARGE_NUM, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);

    code = bundleActiveCore->QueryBundleStatsInfos(packageStats, COMMON_USERID, 4, 0, LARGE_NUM, g_defaultBundleName);
    EXPECT_NE(code, ERR_OK);
    
    bundleActiveCore->OnBundleUninstalled(COMMON_USERID, g_defaultBundleName);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_getUserHistory_001
 * @tc.desc: test service getUserHistory boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_getUserHistory_001,
    Function | MediumTest | Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>(false);
    const int64_t timeStamp = LARGE_NUM;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    groupController->bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(timeStamp, bundleActiveCore);

    groupController->OnBundleUninstalled(0, g_defaultBundleName);
}

/*
 * @tc.name: DeviceUsageStatisticsMockTest_calculationTimeOut_001
 * @tc.desc: test service calculationTimeOut boundary condition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsMockTest, DeviceUsageStatisticsMockTest_calculationTimeOut_001,
    Function | MediumTest | Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>(false);

    std::shared_ptr<BundleActivePackageHistory> history = nullptr;
    groupController->calculationTimeOut(history, LARGE_NUM);
    history = std::make_shared<BundleActivePackageHistory>();
    groupController->calculationTimeOut(history, LARGE_NUM);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


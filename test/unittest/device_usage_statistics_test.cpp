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

#include <string>

#include <gtest/gtest.h>
#include "system_ability_definition.h"

#include "bundle_active_client.h"
#include "bundle_active_event.h"
#include "app_group_callback_stub.h"
#include "bundle_active_group_map.h"
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
static int32_t DEFAULT_ERRCODE = 0;
static int64_t LARGE_NUM = 20000000000000;
static int32_t DEFAULT_GROUP = 10;
static std::vector<int32_t> GROUP_TYPE {10, 20, 30, 40, 50};
static sptr<IAppGroupCallback> observer = nullptr;

class DeviceUsageStatisticsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DeviceUsageStatisticsTest::SetUpTestCase(void)
{
}

void DeviceUsageStatisticsTest::TearDownTestCase(void)
{
}

void DeviceUsageStatisticsTest::SetUp(void)
{
}

void DeviceUsageStatisticsTest::TearDown(void)
{
}

class TestAppGroupChangeCallback : public AppGroupCallbackStub {
public:
    void OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo) override;
};

void TestAppGroupChangeCallback::OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
{
    BUNDLE_ACTIVE_LOGI("TestAppGroupChangeCallback::OnAppGroupChanged!");
    MessageParcel data;
    if (!appGroupCallbackInfo.Marshalling(data)) {
        BUNDLE_ACTIVE_LOGE("Marshalling fail");
    }
    appGroupCallbackInfo.Unmarshalling(data);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_GetServiceObject_001
 * @tc.desc: get service object
 * @tc.type: FUNC
 * @tc.require: SR000GGTO8 AR000GH6PK
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_GetServiceObject_001, Function | MediumTest | Level0)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);

    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID);
    EXPECT_NE(remoteObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_ReportEvent_001
 * @tc.desc: report a mock event
 * @tc.type: FUNC
 * @tc.require: SR000GGTO7 SR000GU31B AR000GH6PJ AR000GU380
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_ReportEvent_001, Function | MediumTest | Level0)
{
    BundleActiveEvent eventA(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
    BundleActiveClient::GetInstance().ReportEvent(eventA, DEFAULT_USERID);
    BundleActiveEvent eventB(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_REMOVED);
    BundleActiveClient::GetInstance().ReportEvent(eventB, DEFAULT_USERID);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryBundleEvents_001
 * @tc.desc: QueryBundleEvents
 * @tc.type: FUNC
 * @tc.require: SR000GGTO6 AR000GH6PH
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryBundleEvents_001, Function | MediumTest | Level0)
{
    std::vector<BundleActiveEvent> result;
    BundleActiveClient::GetInstance().QueryBundleEvents(result, 0, LARGE_NUM, 100);
    EXPECT_EQ(result.size() > 0, true);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleEvents(result, LARGE_NUM, LARGE_NUM, 100), 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryCurrentBundleEvents_001
 * @tc.desc: QueryCurrentBundleEvents
 * @tc.type: FUNC
 * @tc.require: SR000GGTO4 AR000GH6PF
 */
HWTEST_F(DeviceUsageStatisticsTest,
    DeviceUsageStatisticsTest_QueryCurrentBundleEvents_001, Function | MediumTest | Level0)
{
    std::vector<BundleActiveEvent> result;
    BundleActiveClient::GetInstance().QueryCurrentBundleEvents(result, 0, LARGE_NUM);
    EXPECT_EQ(result.size(), 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryPackagesStats_001
 * @tc.desc: querypackagestats
 * @tc.type: FUNC
 * @tc.require: SR000GGTO3 AR000GH6PD
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryPackagesStats_001, Function | MediumTest | Level0)
{
    std::vector<BundleActivePackageStats> result;
    BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(result, 4, 0, LARGE_NUM);
    EXPECT_EQ(result.size(), 0);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(result, 4, LARGE_NUM, LARGE_NUM), 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryBundleStatsInfos_001
 * @tc.desc: QueryBundleStatsInfos
 * @tc.type: FUNC
 * @tc.require: issuesI5QJD9
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryBundleStatsInfos_001,
    Function | MediumTest | Level0)
{
    std::vector<BundleActivePackageStats> result;
    BundleActiveClient::GetInstance().QueryBundleStatsInfos(result, 4, 0, LARGE_NUM);
    EXPECT_EQ(result.size(), 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_IsBundleIdle_001
 * @tc.desc: isbundleidle
 * @tc.type: FUNC
 * @tc.require: SR000GGTO5 AR000GH6PG
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_IsBundleIdle_001, Function | MediumTest | Level0)
{
    bool result = false;
    int32_t errCode = BundleActiveClient::GetInstance().IsBundleIdle(result, g_defaultBundleName, DEFAULT_USERID);
    EXPECT_EQ(result, false);
    EXPECT_EQ(errCode, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryModuleUsageRecords_001
 * @tc.desc: QueryModuleUsageRecords
 * @tc.type: FUNC
 * @tc.require: SR000GU2T1 AR000GU37U
 */
HWTEST_F(DeviceUsageStatisticsTest,
    DeviceUsageStatisticsTest_QueryModuleUsageRecords_001, Function | MediumTest | Level0)
{
    int32_t maxNum = 1;
    BundleActiveEvent eventA(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
    BundleActiveClient::GetInstance().ReportEvent(eventA, DEFAULT_USERID);
    std::vector<BundleActiveModuleRecord> results;
    int32_t errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, DEFAULT_USERID);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(results.size(), 0);

    results.clear();
    maxNum = 0;
    errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, DEFAULT_USERID);
    EXPECT_NE(errCode, 0);

    results.clear();
    maxNum = 1001;
    errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, DEFAULT_USERID);
    EXPECT_NE(errCode, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_RegisterAppGroupCallBack_001
 * @tc.desc: RegisterAppGroupCallBack
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest,
    DeviceUsageStatisticsTest_RegisterAppGroupCallBack_001, Function | MediumTest | Level0)
{
    if (!observer) {
        BUNDLE_ACTIVE_LOGI("RegisterAppGroupCallBack construct observer!");
        observer = new (std::nothrow) TestAppGroupChangeCallback();
    }
    ASSERT_NE(observer, nullptr);
    int32_t result = BundleActiveClient::GetInstance().RegisterAppGroupCallBack(observer);
    EXPECT_EQ(result, DEFAULT_ERRCODE);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_SetBundleGroup_001
 * @tc.desc: setbundlename
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_SetAppGroup_001, Function | MediumTest | Level0)
{
    int32_t result = 0;
    BundleActiveClient::GetInstance().QueryAppGroup(result, g_defaultBundleName, COMMON_USERID);
    DEFAULT_GROUP = (result == DEFAULT_GROUP) ? (result + 10) : DEFAULT_GROUP;
    result = BundleActiveClient::GetInstance().SetAppGroup(g_defaultBundleName, DEFAULT_GROUP, COMMON_USERID);
    EXPECT_EQ(result, DEFAULT_ERRCODE);

    result = BundleActiveClient::GetInstance().SetAppGroup(g_defaultBundleName, DEFAULT_GROUP, -1);
    EXPECT_NE(result, DEFAULT_ERRCODE);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryAppGroup_001
 * @tc.desc: QueryAppGroup, no bundleName
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryAppGroup_001, Function | MediumTest | Level0)
{
    BundleActiveClient::GetInstance().SetAppGroup(g_defaultBundleName, DEFAULT_GROUP, COMMON_USERID);
    int32_t result = 0;
    BundleActiveClient::GetInstance().QueryAppGroup(result, g_defaultBundleName, COMMON_USERID);
    bool flag = false;
    for (auto item = GROUP_TYPE.begin(); item != GROUP_TYPE.end(); item++) {
        if (*item == result) {
            flag = true;
            break;
        }
    }
    EXPECT_EQ(flag, true);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_UnRegisterAppGroupCallBack_001
 * @tc.desc: UnRegisterAppGroupCallBack
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_UnRegisterAppGroupCallBack_001,
    Function | MediumTest | Level0)
{
    if (!observer) {
        BUNDLE_ACTIVE_LOGI("observer has been delete");
    }
    ASSERT_NE(observer, nullptr);
    int32_t result = BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(observer);
    observer = nullptr;
    EXPECT_EQ(result, DEFAULT_ERRCODE);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryDeviceEventStats_001
 * @tc.desc: QueryDeviceEventStats
 * @tc.type: FUNC
 * @tc.require: SR000H0H9H AR000H0ROG
 */
HWTEST_F(DeviceUsageStatisticsTest,
    DeviceUsageStatisticsTest_QueryDeviceEventStats_001, Function | MediumTest | Level0)
{
    std::vector<BundleActiveEventStats> eventStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryDeviceEventStats(0, LARGE_NUM, eventStats);
    EXPECT_EQ(errCode, 0);
    errCode = BundleActiveClient::GetInstance().QueryDeviceEventStats(0, LARGE_NUM, eventStats, COMMON_USERID);
    EXPECT_EQ(errCode, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryNotificationEventStats_001
 * @tc.desc: QueryNotificationEventStats
 * @tc.type: FUNC
 * @tc.require: SR000H0H7D AR000H0RR6
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryNotificationEventStats_001, Function
    | MediumTest | Level0)
{
    std::vector<BundleActiveEventStats> eventStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryNotificationEventStats(0, LARGE_NUM, eventStats);
    EXPECT_EQ(errCode, 0);
    errCode = BundleActiveClient::GetInstance().QueryNotificationEventStats(0, LARGE_NUM, eventStats, COMMON_USERID);
    EXPECT_EQ(errCode, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_BundleActiveGroupMap_001
 * @tc.desc: BundleActiveGroupMap
 * @tc.type: FUNC
 * @tc.require: SR000H0G4F AR000H2US8
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_BundleActiveGroupMap_001, Function | MediumTest | Level0)
{
    int64_t minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_FORCED_SET);
    EXPECT_EQ(minInterval, 0);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_ALIVE);
    EXPECT_EQ(minInterval, DeviceUsageStatsGroupConst::TWO_HOUR);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_DAILY);
    EXPECT_EQ(minInterval, 2 * DeviceUsageStatsGroupConst::TWO_HOUR);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_FIXED);
    EXPECT_EQ(minInterval, DeviceUsageStatsGroupConst::TWENTY_FOUR_HOUR);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_RARE);
    EXPECT_EQ(minInterval, DeviceUsageStatsGroupConst::FOURTY_EIGHT_HOUR);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_DeathRecipient_001
 * @tc.desc: DeathRecipient_001
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_DeathRecipient_001, Function | MediumTest | Level0)
{
    auto deathTest = std::make_shared<BundleActiveClient::BundleActiveClientDeathRecipient>();
    deathTest->AddObserver(observer);
    deathTest->RemoveObserver();
    deathTest->OnServiceDiedInner();

    deathTest->observer_ = new (std::nothrow) TestAppGroupChangeCallback();
    deathTest->OnServiceDiedInner();
    EXPECT_TRUE(deathTest != nullptr);
    deathTest->OnRemoteDied(nullptr);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


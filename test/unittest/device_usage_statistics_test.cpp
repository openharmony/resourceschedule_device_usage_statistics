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
#undef private
#undef protected

#include <string>

#include <gtest/gtest.h>
#include "system_ability_definition.h"

#include "bundle_active_client.h"
#include "bundle_active_event.h"
#include "bundle_active_group_callback_stub.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
static std::string DEFAULT_BUNDLENAME = "com.ohos.camera";
static std::string DEFAULT_MODULENAME = "defaultmodulename";
static std::string DEFAULT_FORM_NAME = "defaultformname";
static int32_t DEFAULT_DIMENSION = 4;
static int64_t DEFAULT_FORMID = 1;
static std::string DEFAULT_ABILITYID = "1234";
static std::string DEFAULT_ABILITYNAME = "testability";
static int32_t DEFAULT_USERID = 0;
static int32_t COMMON_USERID = 100;
static int32_t DEFAULT_ERRCODE = 0;
static int64_t LARGE_NUM = 20000000000000;
static int32_t DEFAULT_GROUP = 10;
static std::vector<int32_t> GROUP_TYPE {10, 20, 30, 40, 50};
static std::shared_ptr<BundleActiveGroupCallbackStub> observer = nullptr;

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
    BundleActiveEvent eventA(DEFAULT_BUNDLENAME, DEFAULT_MODULENAME, DEFAULT_FORM_NAME,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
    BundleActiveClient::GetInstance().ReportEvent(eventA, DEFAULT_USERID);
    BundleActiveEvent eventB(DEFAULT_BUNDLENAME, DEFAULT_MODULENAME, DEFAULT_FORM_NAME,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_REMOVED);
    BundleActiveClient::GetInstance().ReportEvent(eventB, DEFAULT_USERID);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryEvents_001
 * @tc.desc: queryevents
 * @tc.type: FUNC
 * @tc.require: SR000GGTO6 AR000GH6PH
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryEvents_001, Function | MediumTest | Level0)
{
    int32_t errCode = 0;
    std::vector<BundleActiveEvent> result = BundleActiveClient::GetInstance().QueryEvents(0, LARGE_NUM, errCode);
    EXPECT_EQ(result.size(), 1);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryCurrentEvents_001
 * @tc.desc: querycurrentevents
 * @tc.type: FUNC
 * @tc.require: SR000GGTO4 AR000GH6PF
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryCurrentEvents_001, Function | MediumTest | Level0)
{
    std::vector<BundleActiveEvent> result = BundleActiveClient::GetInstance().QueryCurrentEvents(0, LARGE_NUM);
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
    int32_t errCode = 0;
    std::vector<BundleActivePackageStats> result = BundleActiveClient::GetInstance().QueryPackageStats(4, 0,
        LARGE_NUM, errCode);
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
    bool result = BundleActiveClient::GetInstance().IsBundleIdle(DEFAULT_BUNDLENAME);
    EXPECT_EQ(result, true);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryFormStatistics_001
 * @tc.desc: QueryFormStatistics
 * @tc.type: FUNC
 * @tc.require: SR000GU2T1 AR000GU37U
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryFormStatistics_001, Function | MediumTest | Level0)
{
    int32_t maxNum = 1;
    BundleActiveEvent eventA(DEFAULT_BUNDLENAME, DEFAULT_MODULENAME, DEFAULT_FORM_NAME,
        DEFAULT_DIMENSION, DEFAULT_FORMID, BundleActiveEvent::FORM_IS_CLICKED);
    BundleActiveClient::GetInstance().ReportEvent(eventA, DEFAULT_USERID);
    std::vector<BundleActiveModuleRecord> results;
    int32_t errCode = BundleActiveClient::GetInstance().QueryFormStatistics(maxNum, results, DEFAULT_USERID);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(results.size(), 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryPackageGroup_001
 * @tc.desc: querypackagegroup, no bundleName
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryPackageGroup_001, Function | MediumTest | Level0)
{
    int32_t result = BundleActiveClient::GetInstance().QueryPackageGroup(DEFAULT_BUNDLENAME, COMMON_USERID);
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
 * @tc.name: DeviceUsageStatisticsTest_SetBundleGroup_001
 * @tc.desc: setbundlename
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_SetBundleGroup_001, Function | MediumTest | Level0)
{
    int32_t result = BundleActiveClient::GetInstance().SetBundleGroup(DEFAULT_BUNDLENAME, DEFAULT_GROUP,
        DEFAULT_ERRCODE, COMMON_USERID);
    EXPECT_EQ(result, DEFAULT_ERRCODE);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_RegisterGroupCallBack_001
 * @tc.desc: registercallback
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_RegisterGroupCallBack_001, Function | MediumTest | Level0)
{
    observer=std::make_shared<BundleActiveGroupCallbackStub>();
    if (!observer) {
        BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack construct observer!------------------------------");
    }
    int32_t result = BundleActiveClient::GetInstance().RegisterGroupCallBack(observer.get());
    EXPECT_EQ(result, DEFAULT_ERRCODE);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_UnRegisterGroupCallBack_001
 * @tc.desc: unregistercallback
 * @tc.type: FUNC
 * @tc.require: SR000H0HAQ AR000H0ROE
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_UnRegisterGroupCallBack_001,
    Function | MediumTest | Level0)
{
    if (!observer) {
        BUNDLE_ACTIVE_LOGI("observer has been delete");
    }
    int32_t result = BundleActiveClient::GetInstance().UnregisterGroupCallBack(observer.get());
    observer = nullptr;
    EXPECT_EQ(result, DEFAULT_ERRCODE);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryEventStats_001
 * @tc.desc: QueryEventStats
 * @tc.type: FUNC
 * @tc.require: SR000H0H9H AR000H0ROG
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryEventStats_001, Function | MediumTest | Level0)
{
    std::vector<BundleActiveEventStats> eventStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryEventStats(0, LARGE_NUM, eventStats);
    EXPECT_EQ(errCode, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsTest_QueryAppNotificationNumber_001
 * @tc.desc: QueryAppNotificationNumber
 * @tc.type: FUNC
 * @tc.require: SR000H0H7D AR000H0RR6
 */
HWTEST_F(DeviceUsageStatisticsTest, DeviceUsageStatisticsTest_QueryAppNotificationNumber_001, Function
    | MediumTest | Level0)
{
    std::vector<BundleActiveEventStats> eventStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryAppNotificationNumber(0, LARGE_NUM, eventStats);
    EXPECT_EQ(errCode, 0);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


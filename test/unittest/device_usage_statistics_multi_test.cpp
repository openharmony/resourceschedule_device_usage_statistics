/*
 * Copyright (c) 2023  Huawei Device Co., Ltd.
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
#include <gtest/hwext/gtest-multithread.h>
#include <unistd.h>
#include "system_ability_definition.h"

#include "bundle_active_client.h"
#include "bundle_active_event.h"
#include "app_group_callback_stub.h"
#include "bundle_active_group_map.h"
#include "app_group_callback_info.h"
#include "bundle_active_form_record.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_module_record.h"
#include "bundle_active_package_stats.h"
#include "app_group_callback_proxy.h"
#include "bundle_active_log.h"
#include "iapp_group_callback.h"

using namespace testing::ext;
using namespace testing::mt;

namespace OHOS {
namespace DeviceUsageStats {
#ifdef __aarch64__
static std::string g_defaultBundleName = "com.huawei.hmos.camera";
#else
static std::string g_defaultBundleName = "com.ohos.camera";
#endif
static std::string g_defaultMoudleName = "defaultmodulename";
static std::string g_defaultFormName = "defaultformname";
static int32_t g_defaultDimension = 4;
static int64_t g_defaultFormId = 1;
static int32_t g_defaultUserId = 0;
static int32_t g_commonUserid = 100;
static int64_t g_largeNum = 20000000000000;
static int32_t g_defaultGroup = 10;
static int32_t g_intervalType = 4;
static int32_t g_maxNum = 1001;
static int32_t g_fourHour = 2 * DeviceUsageStatsGroupConst::TWO_HOUR;
static std::vector<int32_t> GROUP_TYPE {10, 20, 30, 40, 50};
static sptr<IAppGroupCallback> observer = nullptr;

class DeviceUsageStatisticsMultiTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DeviceUsageStatisticsMultiTest::SetUpTestCase(void)
{
}

void DeviceUsageStatisticsMultiTest::TearDownTestCase(void)
{
}

void DeviceUsageStatisticsMultiTest::SetUp(void)
{
}

void DeviceUsageStatisticsMultiTest::TearDown(void)
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
 * @tc.name: DeviceUsageStatisticsMultiTest_IsBundleIdle_001
 * @tc.desc: isbundleidle
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestIsBundleIdle(void)
{
    bool result = false;
    int32_t errCode = BundleActiveClient::GetInstance().IsBundleIdle(result, g_defaultBundleName, g_defaultUserId);
    EXPECT_EQ(result, false);
    EXPECT_EQ(errCode, 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_IsBundleIdle_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestIsBundleIdle);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_ReportEvent_001
 * @tc.desc: report a mock event
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestReportEvent(void)
{
    BundleActiveEvent eventA(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        g_defaultDimension, g_defaultFormId, BundleActiveEvent::FORM_IS_CLICKED);
    BundleActiveClient::GetInstance().ReportEvent(eventA, g_defaultUserId);
    BundleActiveEvent eventB(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        g_defaultDimension, g_defaultFormId, BundleActiveEvent::FORM_IS_REMOVED);
    BundleActiveClient::GetInstance().ReportEvent(eventB, g_defaultUserId);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_ReportEvent_001, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestReportEvent);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryBundleEvents_001
 * @tc.desc: QueryBundleEvents
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestQueryBundleEvents(void)
{
    std::vector<BundleActiveEvent> result;
    BundleActiveClient::GetInstance().QueryBundleEvents(result, 0, g_largeNum, g_commonUserid);
    EXPECT_EQ(result.size() > 0, true);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleEvents(result, g_largeNum, g_largeNum, g_commonUserid), 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryBundleEvents_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryBundleEvents);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryCurrentBundleEvents_001
 * @tc.desc: QueryCurrentBundleEvents
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestQueryCurrentBundleEvents(void)
{
    std::vector<BundleActiveEvent> result;
    BundleActiveClient::GetInstance().QueryCurrentBundleEvents(result, 0, g_largeNum);
    EXPECT_EQ(result.size(), 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryCurrentBundleEvents_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryCurrentBundleEvents);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryPackagesStats_001
 * @tc.desc: querypackagestats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestQueryBundleStatsInfoByInterval(void)
{
    std::vector<BundleActivePackageStats> result;
    BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(result, g_intervalType, 0, g_largeNum);
    EXPECT_EQ(result.size(), 0);
    EXPECT_NE(BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(
        result, g_intervalType, g_largeNum, g_largeNum), 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryPackagesStats_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryBundleStatsInfoByInterval);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryBundleStatsInfos_001
 * @tc.desc: QueryBundleStatsInfos
 * @tc.type: FUNC
 * @tc.require: issuesI5QJD9
 */
void MultiTestQueryBundleStatsInfos(void)
{
    std::vector<BundleActivePackageStats> result;
    BundleActiveClient::GetInstance().QueryBundleStatsInfos(result, g_intervalType, 0, g_largeNum);
    EXPECT_EQ(result.size(), 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryBundleStatsInfos_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryBundleStatsInfos);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryModuleUsageRecords_001
 * @tc.desc: QueryModuleUsageRecords
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestQueryModuleUsageRecords(void)
{
    int32_t maxNum = 1;
    BundleActiveEvent eventA(g_defaultBundleName, g_defaultMoudleName, g_defaultFormName,
        g_defaultDimension, g_defaultFormId, BundleActiveEvent::FORM_IS_CLICKED);
    BundleActiveClient::GetInstance().ReportEvent(eventA, g_defaultUserId);
    std::vector<BundleActiveModuleRecord> results;
    int32_t errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, g_defaultUserId);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(results.size(), 0);

    results.clear();
    maxNum = 0;
    errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, g_defaultUserId);
    EXPECT_NE(errCode, 0);

    results.clear();
    maxNum = g_maxNum;
    errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, g_defaultUserId);
    EXPECT_NE(errCode, 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryModuleUsageRecords_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryModuleUsageRecords);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryAppGroup_001
 * @tc.desc: QueryAppGroup, no bundleName
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestAppGroup(void)
{
    BundleActiveClient::GetInstance().SetAppGroup(g_defaultBundleName, g_defaultGroup, g_commonUserid);
    int32_t result = 0;
    BundleActiveClient::GetInstance().QueryAppGroup(result, g_defaultBundleName, g_commonUserid);
    bool flag = false;
    for (auto item = GROUP_TYPE.begin(); item != GROUP_TYPE.end(); item++) {
        if (*item == result) {
            flag = true;
            break;
        }
    }
    EXPECT_EQ(flag, true);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_AppGroup_001, Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestAppGroup);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryDeviceEventStats_001
 * @tc.desc: QueryDeviceEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestQueryDeviceEventStats(void)
{
    std::vector<BundleActiveEventStats> eventStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryDeviceEventStats(0, g_largeNum, eventStats);
    EXPECT_EQ(errCode, 0);
    errCode = BundleActiveClient::GetInstance().QueryDeviceEventStats(0, g_largeNum, eventStats, g_commonUserid);
    EXPECT_EQ(errCode, 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryDeviceEventStats_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryDeviceEventStats);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_QueryNotificationEventStats_001
 * @tc.desc: QueryNotificationEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestQueryNotificationEventStats(void)
{
    std::vector<BundleActiveEventStats> eventStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryNotificationEventStats(0, g_largeNum, eventStats);
    EXPECT_EQ(errCode, 0);
    errCode = BundleActiveClient::GetInstance().QueryNotificationEventStats(0, g_largeNum, eventStats, g_commonUserid);
    EXPECT_EQ(errCode, 0);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_QueryNotificationEventStats_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestQueryNotificationEventStats);
}

/*
 * @tc.name: DeviceUsageStatisticsMultiTest_BundleActiveGroupMap_001
 * @tc.desc: BundleActiveGroupMap
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
void MultiTestDeviceUsageStatsGroupMap(void)
{
    int64_t minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_FORCED_SET);
    EXPECT_EQ(minInterval, 0);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_ALIVE);
    EXPECT_EQ(minInterval, DeviceUsageStatsGroupConst::TWO_HOUR);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_DAILY);
    EXPECT_EQ(minInterval, g_fourHour);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_FIXED);
    EXPECT_EQ(minInterval, DeviceUsageStatsGroupConst::TWENTY_FOUR_HOUR);
    minInterval = DeviceUsageStatsGroupMap::groupIntervalMap_
        .at(DeviceUsageStatsGroupConst::ACTIVE_GROUP_RARE);
    EXPECT_EQ(minInterval, DeviceUsageStatsGroupConst::FOURTY_EIGHT_HOUR);
}

HWTEST_F(DeviceUsageStatisticsMultiTest, DeviceUsageStatisticsMultiTest_BundleActiveGroupMap_001,
    Function | MediumTest | Level0)
{
    SET_THREAD_NUM(100);
    GTEST_RUN_TASK(MultiTestDeviceUsageStatsGroupMap);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


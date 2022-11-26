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

#include "bundle_active_module_record.h"
#include "bundle_active_event_tracker.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"
#include "bundle_active_form_record.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_user_service.h"
#include "bundle_active_core.h"
#include "bundle_active_stats_combiner.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
class PackageUsageTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PackageUsageTest::SetUpTestCase(void)
{
}

void PackageUsageTest::TearDownTestCase(void)
{
}

void PackageUsageTest::SetUp(void)
{
}

void PackageUsageTest::TearDown(void)
{
}

/*
 * @tc.name: PackageUsageTest_Update_001
 * @tc.desc: Update
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_Update_001, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    std::string longTimeTaskName = "defaultLongTimeTaskName";
    int64_t timeStamp = 20000000000000;
    int32_t eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    std::string abilityId = "defaultAbilityId";
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::ABILITY_BACKGROUND;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::ABILITY_STOP;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::END_OF_THE_DAY;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::LONG_TIME_TASK_STARTTED;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::LONG_TIME_TASK_ENDED;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::SHUTDOWN;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);
    
    eventId = BundleActiveEvent::FLUSH;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::SYSTEM_LOCK;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId);
}

/*
 * @tc.name: PackageUsageTest_UpdateLongTimeTask_001
 * @tc.desc: UpdateLongTimeTask
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_UpdateLongTimeTask_001, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    std::string longTimeTaskName = "defaultLongTimeTaskName";
    int64_t timeStamp = 20000000000000;
    int32_t eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    packageStats->UpdateLongTimeTask(longTimeTaskName, timeStamp, eventId);

    eventId = BundleActiveEvent::LONG_TIME_TASK_ENDED;
    packageStats->UpdateLongTimeTask(longTimeTaskName, timeStamp, eventId);
    packageStats->UpdateLongTimeTask(longTimeTaskName, timeStamp, eventId);

    eventId = BundleActiveEvent::LONG_TIME_TASK_STARTTED;
    packageStats->UpdateLongTimeTask(longTimeTaskName, timeStamp, eventId);
}

/*
 * @tc.name: PackageUsageTest_UpdateAbility_001
 * @tc.desc: UpdateAbility
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_UpdateAbility_001, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    std::string abilityId = "defaultAbilityId";
    int64_t timeStamp = 20000000000000;
    int32_t eventId = BundleActiveEvent::LONG_TIME_TASK_ENDED;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);

    eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);
    
    eventId = BundleActiveEvent::ABILITY_BACKGROUND;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);
    packageStats->HasFrontAbility();

    eventId = BundleActiveEvent::ABILITY_STOP;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);
}

/*
 * @tc.name: PackageUsageTest_Increment_001
 * @tc.desc: IncrementServiceTimeUsed and IncrementTimeUsed
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_Increment_001, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    int64_t largeNum = 20000000000000;
    packageStats->lastContiniousTaskUsed_ = largeNum;
    packageStats->IncrementServiceTimeUsed(largeNum + 1);
    packageStats->IncrementServiceTimeUsed(largeNum);

    packageStats->lastTimeUsed_ = largeNum;
    packageStats->IncrementTimeUsed(largeNum + 1);
    packageStats->IncrementTimeUsed(largeNum);
}

/*
 * @tc.name: PackageUsageTest_BundleActiveModuleRecord_001
 * @tc.desc: BundleActiveModuleRecord
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_BundleActiveModuleRecord_001, Function | MediumTest | Level0)
{
    auto moduleRecord = std::make_shared<BundleActiveModuleRecord>();
    std::string forName = "defaultformname";
    int32_t formDimension = 1;
    int64_t formId = 1;
    int64_t timeStamp = 20000000000000;
    moduleRecord->AddOrUpdateOneFormRecord(forName, formDimension, formId, timeStamp);
    moduleRecord->AddOrUpdateOneFormRecord(forName, formDimension, formId, timeStamp*10);
    moduleRecord->lastModuleUsedTime_ = timeStamp;
    moduleRecord->UpdateModuleRecord(timeStamp);
    moduleRecord->RemoveOneFormRecord(forName, formDimension, formId);
}

/*
 * @tc.name: PackageUsageTest_BundleActiveEventTracker_001
 * @tc.desc: BundleActiveEventTracker
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_BundleActiveEventTracker_001, Function | MediumTest | Level0)
{
    auto eventTracker = std::make_shared<BundleActiveEventTracker>();
    eventTracker->curStartTime_ = 0;
    int64_t timeStamp = 20000000000000;
    eventTracker->Update(timeStamp);

    eventTracker->curStartTime_ = 1;
    eventTracker->Update(timeStamp);

    eventTracker->count_ = 0;
    eventTracker->duration_ = 0;
    std::vector<BundleActiveEventStats> eventStatsList;
    int32_t eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    int64_t beginTime = 0;
    int64_t endTime = timeStamp;
    eventTracker->AddToEventStats(eventStatsList, eventId, beginTime, endTime);

    eventTracker->count_ = 0;
    eventTracker->duration_ = 1;
    eventTracker->AddToEventStats(eventStatsList, eventId, beginTime, endTime);

    eventTracker->count_ = 1;
    eventTracker->duration_ = 0;
    eventTracker->AddToEventStats(eventStatsList, eventId, beginTime, endTime);
}

/*
 * @tc.name: PackageUsageTest_BundleActiveFormRecord_001
 * @tc.desc: BundleActiveFormRecord
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_BundleActiveFormRecord_001, Function | MediumTest | Level0)
{
    auto formRecord = std::make_shared<BundleActiveFormRecord>();
    int64_t timeStamp = 20000000000000;
    formRecord->UpdateFormRecord(timeStamp);
    formRecord->UpdateFormRecord(timeStamp);
}

/*
 * @tc.name: PackageUsageTest_BundleActiveEventStats_001
 * @tc.desc: BundleActiveEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_BundleActiveEventStats_001, Function | MediumTest | Level0)
{
    auto eventStats = std::make_shared<BundleActiveEventStats>();
    BundleActiveEventStats stat;
    stat.eventId_ = 1;
    eventStats->add(stat);

    stat.eventId_ = 0;
    stat.beginTimeStamp_ = 1;
    eventStats->add(stat);

    stat.beginTimeStamp_ = -1;
    eventStats->add(stat);
}

/*
 * @tc.name: PackageUsageTest_ReportForShutdown_001
 * @tc.desc: ReportForShutdown
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_ReportForShutdown_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), false);
    BundleActiveEvent event;
    event.eventId_ = BundleActiveEvent::ABILITY_FOREGROUND;
    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    bundleUserService->ReportForShutdown(event);

    event.eventId_ = BundleActiveEvent::SHUTDOWN;
    bundleUserService->ReportForShutdown(event);

    event.timeStamp_ = -1;
    bundleUserService->ReportForShutdown(event);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_ReportFormEvent_001
 * @tc.desc: ReportFormEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_ReportFormEvent_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    BundleActiveEvent event;
    event.bundleName_ = "defaultBundleName";
    event.moduleName_ = "defaultModuleName";
    event.eventId_ = BundleActiveEvent::FORM_IS_CLICKED;
    bundleUserService->ReportFormEvent(event);

    event.eventId_ = BundleActiveEvent::FORM_IS_REMOVED;
    bundleUserService->ReportFormEvent(event);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_PrintInMemFormStats_001
 * @tc.desc: PrintInMemFormStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_PrintInMemFormStats_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    BundleActiveEvent event;
    event.bundleName_ = "defaultBundleName";
    event.moduleName_ = "defaultModuleName";
    bundleUserService->GetOrCreateModuleRecord(event);
    
    bundleUserService->PrintInMemFormStats(true, true);
    bundleUserService->PrintInMemFormStats(true, false);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_QueryDeviceEventStats_001
 * @tc.desc: QueryDeviceEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_QueryDeviceEventStats_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    int64_t beginTime = 0;
    int64_t endTime = 0;
    std::vector<BundleActiveEventStats> eventStats;
    bundleUserService->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);

    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    bundleUserService->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);

    beginTime = -1;
    bundleUserService->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);

    bundleUserService->currentStats_[0]->endTime_ = 1;
    beginTime = 0;
    bundleUserService->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_QueryNotificationEventStats_001
 * @tc.desc: QueryNotificationEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_QueryNotificationEventStats_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    int64_t beginTime = 0;
    int64_t endTime = 0;
    std::vector<BundleActiveEventStats> eventStats;
    bundleUserService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);

    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    bundleUserService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);

    beginTime = -1;
    bundleUserService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);

    bundleUserService->currentStats_[0]->endTime_ = 1;
    beginTime = 0;
    bundleUserService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);

    bundleUserService->currentStats_[0]->endTime_ = 1;
    beginTime = 0;
    endTime = 20000000000000;
    bundleUserService->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_QueryBundleEvents_001
 * @tc.desc: QueryBundleEvents
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_QueryBundleEvents_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    int64_t beginTime = 0;
    int64_t endTime = 0;
    std::vector<BundleActiveEvent> bundleActiveEvent;
    std::string bundleName = "defaultBundleName";
    bundleUserService->QueryBundleEvents(bundleActiveEvent, beginTime, endTime, userId, bundleName);

    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    EXPECT_NE(
        bundleUserService->QueryBundleEvents(bundleActiveEvent, beginTime, endTime, userId, bundleName), ERR_OK);

    beginTime = -1;
    bundleUserService->QueryBundleEvents(bundleActiveEvent, beginTime, endTime, userId, bundleName);

    bundleUserService->currentStats_[0]->endTime_ = 1;
    endTime = 20000000000000;
    bundleUserService->QueryBundleEvents(bundleActiveEvent, beginTime, endTime, userId, bundleName);

    bundleUserService->currentStats_[0]->endTime_ = 1;
    beginTime = 0;
    endTime = 20000000000000;
    bundleUserService->QueryBundleEvents(bundleActiveEvent, beginTime, endTime, userId, bundleName);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_PrintInMemPackageStats_001
 * @tc.desc: PrintInMemPackageStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_PrintInMemPackageStats_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    bundleUserService->currentStats_[0]->bundleStats_.emplace(
        "defaultBundleStat", std::make_shared<BundleActivePackageStats>());
    bundleUserService->PrintInMemPackageStats(0, true);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_QueryBundleStatsInfos_001
 * @tc.desc: QueryBundleStatsInfos
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_QueryBundleStatsInfos_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    int64_t beginTime = 0;
    int64_t endTime = 0;
    std::vector<BundleActivePackageStats> PackageStats;
    std::string bundleName = "defaultBundleName";
    int32_t intervalType = -1;
    EXPECT_NE(bundleUserService->QueryBundleStatsInfos(
        PackageStats, intervalType, beginTime, endTime, userId, bundleName), ERR_OK);

    intervalType = 5;
    EXPECT_NE(bundleUserService->QueryBundleStatsInfos(
        PackageStats, intervalType, beginTime, endTime, userId, bundleName), ERR_OK);

    intervalType = 0;
    bundleUserService->currentStats_[0] = nullptr;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    beginTime = ONE_DAY_TIME + 1;
    EXPECT_NE(bundleUserService->QueryBundleStatsInfos(
        PackageStats, intervalType, beginTime, endTime, userId, bundleName), ERR_OK);

    beginTime = ONE_DAY_TIME;
    EXPECT_NE(bundleUserService->QueryBundleStatsInfos(
        PackageStats, intervalType, beginTime, endTime, userId, bundleName), ERR_OK);

    bundleUserService->currentStats_[0]->endTime_ = 1;
    beginTime = 0;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    endTime = 0;
    auto packageStatsObject = std::make_shared<BundleActivePackageStats>();
    bundleUserService->currentStats_[0]->bundleStats_.emplace("defaultBundleStat", packageStatsObject);
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    packageStatsObject->totalInFrontTime_ = 1;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    packageStatsObject->bundleName_ = "defaultBundleName";
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    packageStatsObject->lastTimeUsed_ = 1;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    endTime = 1;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);
    packageStatsObject = nullptr;

    endTime = 0;
    bundleName = "";
    packageStatsObject = std::make_shared<BundleActivePackageStats>();
    bundleUserService->currentStats_[0]->bundleStats_.emplace("defaultBundleStat", packageStatsObject);
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    packageStatsObject->totalInFrontTime_ = 1;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    packageStatsObject->lastTimeUsed_ = 1;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);

    endTime = 1;
    bundleUserService->QueryBundleStatsInfos(PackageStats, intervalType, beginTime, endTime, userId, bundleName);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_RestoreStats_001
 * @tc.desc: RestoreStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_RestoreStats_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    auto packageStatsObject = std::make_shared<BundleActivePackageStats>();
    bundleUserService->currentStats_[0]->bundleStats_.emplace(
        "defaultBundleStat", packageStatsObject);
    bundleUserService->moduleRecords_.emplace("defaultModule", std::make_shared<BundleActiveModuleRecord>());

    bool forced = false;
    bundleUserService->statsChanged_ = false;
    bundleUserService->RestoreStats(forced);

    forced = true;
    bundleUserService->currentStats_[3] = nullptr;
    bundleUserService->RestoreStats(forced);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_LoadActiveStats_001
 * @tc.desc: LoadActiveStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_LoadActiveStats_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);

    bundleUserService->debugUserService_ = true;
    bool forced = true;
    bool timeChanged = true;
    bundleUserService->LoadActiveStats(timeStamp, forced, timeChanged);

    forced = false;
    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    bundleUserService->currentStats_[3] = nullptr;
    bundleUserService->LoadActiveStats(timeStamp, forced, timeChanged);

    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_IsBundleEvent_001
 * @tc.desc: IsBundleEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_IsBundleEvent_001, Function | MediumTest | Level0)
{
    auto bundleEvent = std::make_shared<BundleActiveEvent>();
    EXPECT_EQ(bundleEvent->IsBundleEvent(BundleActiveEvent::END_OF_THE_DAY), true);
    EXPECT_NE(bundleEvent->IsBundleEvent(BundleActiveEvent::SHUTDOWN), true);
}

/*
 * @tc.name: PackageUsageTest_combine_001
 * @tc.desc: combine
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_combine_001, Function | MediumTest | Level0)
{
    auto combiner = std::make_shared<BundleActiveStatsCombiner<BundleActivePackageStats>>();
    auto stats = std::make_shared<BundleActivePeriodStats>();
    auto packageStat = std::make_shared<BundleActivePackageStats>();
    stats->bundleStats_.emplace("normal", packageStat);
    packageStat = nullptr;
    stats->bundleStats_.emplace("default", packageStat);
    int64_t beginTime = 0;
    std::vector<BundleActivePackageStats> accumulatedResult;
    combiner->combine(stats, accumulatedResult, beginTime);

    auto eventCombiner = std::make_shared<BundleActiveStatsCombiner<BundleActiveEvent>>();
    std::vector<BundleActiveEvent> activeEventResult;
    eventCombiner->combine(stats, activeEventResult, beginTime);
}

/*
 * @tc.name: PackageUsageTest_ReportEvent_001
 * @tc.desc: ReportEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_ReportEvent_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000;
    bundleUserService->Init(timeStamp);

    BundleActiveEvent event;
    event.timeStamp_ = 20000000000000000;
    bundleUserService->Init(timeStamp);
    event.eventId_ = BundleActiveEvent::SYSTEM_INTERACTIVE;
    bundleUserService->ReportEvent(event);

    event.eventId_ = BundleActiveEvent::FLUSH;
    bundleUserService->ReportEvent(event);

    event.eventId_ = BundleActiveEvent::SCREEN_INTERACTIVE;
    bundleUserService->ReportEvent(event);

    event.eventId_ = BundleActiveEvent::SCREEN_NON_INTERACTIVE;
    bundleUserService->ReportEvent(event);

    event.eventId_ = BundleActiveEvent::KEYGUARD_SHOWN;
    bundleUserService->ReportEvent(event);

    event.eventId_ = BundleActiveEvent::KEYGUARD_HIDDEN;
    bundleUserService->ReportEvent(event);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_RenewStatsInMemory_001
 * @tc.desc: RenewStatsInMemory
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_RenewStatsInMemory_001, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000;
    bundleUserService->Init(timeStamp);

    auto packageStat = std::make_shared<BundleActivePackageStats>();
    bundleUserService->currentStats_[0]->bundleStats_.emplace("normal", packageStat);

    packageStat->abilities_.emplace("normal", 123);
    packageStat->longTimeTasks_.emplace("normal", 123);
    bundleUserService->currentStats_[0]->bundleStats_.emplace("normal", packageStat);
    packageStat = nullptr;
    bundleUserService->currentStats_[0]->bundleStats_.emplace("default", packageStat);
    bundleUserService->RenewStatsInMemory(timeStamp);
    EXPECT_NE(bundleUserService, nullptr);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
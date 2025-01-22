/*
 * Copyright (c) 2022-2024  Huawei Device Co., Ltd.
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

#include "bundle_active_calendar.h"
#include "bundle_active_module_record.h"
#include "bundle_active_event_tracker.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"
#include "bundle_active_form_record.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_user_service.h"
#include "bundle_active_core.h"
#include "bundle_active_stats_combiner.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_log.h"
#include "bundle_active_group_controller.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
class PackageUsageTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static std::shared_ptr<BundleActiveCore> bundleActiveCore_;
};

std::shared_ptr<BundleActiveCore> PackageUsageTest::bundleActiveCore_ = nullptr;

void PackageUsageTest::SetUpTestCase(void)
{
    bundleActiveCore_ = std::make_shared<BundleActiveCore>();
    bundleActiveCore_->Init();
    bundleActiveCore_->InitBundleGroupController();
}

void PackageUsageTest::TearDownTestCase(void)
{
    bundleActiveCore_->bundleGroupHandler_->ffrtQueue_.reset();
    int64_t sleepTime = 3;
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
}

void PackageUsageTest::SetUp(void)
{
}

void PackageUsageTest::TearDown(void)
{
    int64_t sleepTime = 300;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
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
    int32_t uid = 0;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::ABILITY_BACKGROUND;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::ABILITY_STOP;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::END_OF_THE_DAY;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::LONG_TIME_TASK_STARTTED;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::LONG_TIME_TASK_ENDED;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::SHUTDOWN;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);
    
    eventId = BundleActiveEvent::FLUSH;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

    eventId = BundleActiveEvent::SYSTEM_LOCK;
    packageStats->Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);
    EXPECT_NE(packageStats, nullptr);
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
    EXPECT_NE(packageStats, nullptr);
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
    EXPECT_NE(packageStats, nullptr);
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
    EXPECT_NE(packageStats, nullptr);
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
    int32_t uid = 0;
    moduleRecord->AddOrUpdateOneFormRecord(forName, formDimension, formId, timeStamp, uid);
    moduleRecord->AddOrUpdateOneFormRecord(forName, formDimension, formId, timeStamp*10, uid);
    moduleRecord->lastModuleUsedTime_ = timeStamp;
    moduleRecord->UpdateModuleRecord(timeStamp);
    moduleRecord->RemoveOneFormRecord(forName, formDimension, formId);
    EXPECT_NE(moduleRecord, nullptr);
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
    EXPECT_NE(eventTracker, nullptr);
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
    EXPECT_NE(formRecord, nullptr);
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
    EXPECT_NE(eventStats, nullptr);
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: PackageUsageTest_QueryBundleStatsInfos_002
 * @tc.desc: QueryBundleStatsInfos
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(PackageUsageTest, PackageUsageTest_QueryBundleStatsInfos_002, Function | MediumTest | Level0)
{
    int32_t userId = 100;
    auto bundleActiveCore = bundleActiveCore_;
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000000;
    bundleUserService->Init(timeStamp);
    int64_t beginTime = 0;
    int64_t endTime = 0;
    std::vector<BundleActivePackageStats> PackageStats;
    std::string bundleName = "defaultBundleName";
    int32_t intervalType = 0;
    bundleUserService->currentStats_[0] = std::make_shared<BundleActivePeriodStats>();
    bundleUserService->currentStats_[0]->endTime_ = 1;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
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
    EXPECT_NE(combiner, nullptr);
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
    auto bundleActiveCore = bundleActiveCore_;
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
    auto bundleActiveCore = bundleActiveCore_;
    auto bundleUserService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore.get()), true);
    int64_t timeStamp = 20000000000;
    bundleUserService->Init(timeStamp);

    auto packageStat = std::make_shared<BundleActivePackageStats>();
    packageStat->uid_ = 0;
    packageStat->bundleName_ = "normal";
    std::string bundleStatsKey = packageStat->bundleName_ + std::to_string(packageStat->uid_);
    bundleUserService->currentStats_[0]->bundleStats_.emplace(bundleStatsKey, packageStat);

    packageStat->abilities_.emplace("normal", 123);
    packageStat->longTimeTasks_.emplace("normal", 123);
    bundleUserService->currentStats_[0]->bundleStats_.emplace(bundleStatsKey, packageStat);
    bundleUserService->RenewStatsInMemory(timeStamp);
    packageStat = nullptr;
    bundleUserService->currentStats_[0]->bundleStats_.emplace("default", packageStat);
    bundleUserService->RenewStatsInMemory(timeStamp);
    EXPECT_NE(bundleUserService, nullptr);
}

/*
 * @tc.name: BundleActiveGroupController_001
 * @tc.desc: DeleteMemoryUsageGroup
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupController_001, Function | MediumTest | Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>(false);
    auto userHistory = std::make_shared<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>>();
    int32_t uid = 0;
    int32_t appIndex = 1;
    std::string bundleName = "test";
    userHistory->emplace(bundleName + std::to_string(uid), std::make_shared<BundleActivePackageHistory>());
    uid = 100;
    userHistory->emplace(bundleName + std::to_string(uid), std::make_shared<BundleActivePackageHistory>());
    uid = 200;
    userHistory->emplace(bundleName + std::to_string(uid), std::make_shared<BundleActivePackageHistory>());
    groupController->DeleteUsageGroupCache(userHistory, bundleName, uid, appIndex);
    auto it = userHistory->find(bundleName + std::to_string(uid));
    EXPECT_EQ(it, userHistory->end());
    appIndex = 0;
    groupController->DeleteUsageGroupCache(userHistory, bundleName, uid, appIndex);
    uid = 0;
    it = userHistory->find(bundleName + std::to_string(uid));
    EXPECT_EQ(it, userHistory->end());
}

/*
 * @tc.name: BundleActiveGroupController_002
 * @tc.desc: ReportEvent
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupController_002, Function | MediumTest | Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>(false);
    auto coreObject = bundleActiveCore_;
    int userId = 100;
    BundleActiveEvent event;
    event.eventId_ = BundleActiveEvent::SYSTEM_INTERACTIVE;
    int64_t timeStamp = 20000000000000;
    coreObject->bundleGroupController_->ReportEvent(event, timeStamp, userId);
    EXPECT_TRUE(coreObject->bundleGroupController_ != nullptr);
}

/*
 * @tc.name: BundleActiveReportHandlerTest_001
 * @tc.desc: ProcessEvent
 * @tc.type: FUNC
 * @tc.require: issuesIAF8RF
 */
HWTEST_F(PackageUsageTest, BundleActiveReportHandlerTest_001, Function | MediumTest | Level0)
{
    BundleActiveReportHandlerObject tmpObject;
    auto handlerObject = std::make_shared<BundleActiveReportHandlerObject>(tmpObject);
    auto bundleActiveReportHandler = std::make_shared<BundleActiveReportHandler>();
    bundleActiveReportHandler->Init(bundleActiveCore_);
    EXPECT_TRUE(bundleActiveReportHandler->isInited_);
    bundleActiveReportHandler->ProcessEvent(0, handlerObject);
    bundleActiveReportHandler->ProcessEvent(0, handlerObject);
}

/*
 * @tc.name: BundleActiveReportHandlerTest_002
 * @tc.desc: SendEvent and removeEvent
 * @tc.type: FUNC
 * @tc.require: issuesIAF8RF
 */
HWTEST_F(PackageUsageTest, BundleActiveReportHandlerTest_002, Function | MediumTest | Level0)
{
    BundleActiveReportHandlerObject tmpObject;
    auto handlerObject = std::make_shared<BundleActiveReportHandlerObject>(tmpObject);
    auto bundleActiveReportHandler = std::make_shared<BundleActiveReportHandler>();
    bundleActiveReportHandler->SendEvent(0, handlerObject);
    bundleActiveReportHandler->RemoveEvent(0);
    EXPECT_EQ(bundleActiveReportHandler->taskHandlerMap_.size(), 0);
    bundleActiveReportHandler->Init(bundleActiveCore_);
    bundleActiveReportHandler->SendEvent(0, handlerObject);
    bundleActiveReportHandler->SendEvent(0, handlerObject, 10);
    EXPECT_NE(bundleActiveReportHandler->taskHandlerMap_.size(), 0);
    bundleActiveReportHandler->RemoveEvent(0);
    bundleActiveReportHandler->RemoveEvent(0);
    EXPECT_EQ(bundleActiveReportHandler->taskHandlerMap_.size(), 0);
}


/*
 * @tc.name: BundleActiveReportHandlerTest_003
 * @tc.desc: HasEvent
 * @tc.type: FUNC
 * @tc.require: issuesIAF8RF
 */
HWTEST_F(PackageUsageTest, BundleActiveReportHandlerTest_003, Function | MediumTest | Level0)
{
    BundleActiveReportHandlerObject tmpObject;
    auto handlerObject = std::make_shared<BundleActiveReportHandlerObject>(tmpObject);
    auto bundleActiveReportHandler = std::make_shared<BundleActiveReportHandler>();
    bundleActiveReportHandler->HasEvent(0);
    bundleActiveReportHandler->Init(bundleActiveCore_);
    bundleActiveReportHandler->SendEvent(0, handlerObject, 10);
    EXPECT_EQ(bundleActiveReportHandler->HasEvent(0), true);
}

/*
 * @tc.name: BundleActiveReportHandlerTest_004
 * @tc.desc: Send Uninstalled APP Event
 * @tc.type: FUNC
 * @tc.require: IAHDJW
 */
HWTEST_F(PackageUsageTest, BundleActiveReportHandlerTest_004, Function | MediumTest | Level0)
{
    auto bundleActiveReportHandler = std::make_shared<BundleActiveReportHandler>();
    bundleActiveReportHandler->Init(bundleActiveCore_);
    int32_t userId = 100;
    std::string bundleName = "test";
    int32_t uid = 100010;
    int32_t appIndex = 1;
    int64_t timeNow = bundleActiveCore_->CheckTimeChangeAndGetWallTime(userId);
    BundleActiveReportHandlerObject tmpObject;
    tmpObject.event_.eventId_ = tmpObject.event_.ABILITY_STOP;
    tmpObject.event_.uid_ = uid;
    tmpObject.event_.timeStamp_ = timeNow;
    auto handlerObject = std::make_shared<BundleActiveReportHandlerObject>(tmpObject);
    bundleActiveReportHandler->SendEvent(0, handlerObject);
    auto service = bundleActiveCore_->GetUserDataAndInitializeIfNeeded(userId, timeNow, false);
    EXPECT_EQ(service->currentStats_[0]->endTime_, timeNow);
    bundleActiveCore_->OnBundleUninstalled(userId, bundleName, uid, appIndex);
    EXPECT_TRUE(bundleActiveCore_->isUninstalledApp(uid));
    timeNow = timeNow + 100;
    tmpObject.event_.timeStamp_ = timeNow;
    bundleActiveReportHandler->SendEvent(0, handlerObject);
    EXPECT_NE(service->currentStats_[0]->endTime_, timeNow);
    bundleActiveCore_->OnBundleInstalled(userId, bundleName, uid, appIndex);
    EXPECT_FALSE(bundleActiveCore_->isUninstalledApp(uid));
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupHandler_001
 * @tc.desc: SendEvent
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_001, Function | MediumTest | Level0)
{
    BundleActiveGroupHandlerObject tmpObject;
    auto handlerObject = std::make_shared<BundleActiveGroupHandlerObject>(tmpObject);
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    bundleActiveGroupHandler->SendEvent(0, handlerObject);
    EXPECT_EQ(bundleActiveGroupHandler->taskHandlerMap_.size(), 0);
    bundleActiveGroupHandler->SendCheckBundleMsg(0, handlerObject);
    EXPECT_EQ(bundleActiveGroupHandler->checkBundleTaskMap_.size(), 0);
}

/*
 * @tc.name: BundleActiveGroupHandler_002
 * @tc.desc: SendEvent
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_002, Function | MediumTest | Level0)
{
    BundleActiveGroupHandlerObject tmpObject;
    auto handlerObject = std::make_shared<BundleActiveGroupHandlerObject>(tmpObject);
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    bundleActiveGroupHandler->Init(bundleActiveCore_->bundleGroupController_);
    bundleActiveGroupHandler->SendEvent(0, handlerObject);
    bundleActiveGroupHandler->SendEvent(0, handlerObject, 10);
    EXPECT_NE(bundleActiveGroupHandler->taskHandlerMap_.size(), 0);
    bundleActiveGroupHandler->RemoveEvent(0);
    bundleActiveGroupHandler->RemoveEvent(0);
    EXPECT_EQ(bundleActiveGroupHandler->taskHandlerMap_.size(), 0);
}

/*
 * @tc.name: BundleActiveGroupHandler_003
 * @tc.desc: SendCheckBundleMsg
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_003, Function | MediumTest | Level0)
{
    BundleActiveGroupHandlerObject tmpObject;
    tmpObject.bundleName_ = "test";
    tmpObject.uid_ = 10000;
    tmpObject.userId_ = 100;
    auto handlerObject = std::make_shared<BundleActiveGroupHandlerObject>(tmpObject);
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    bundleActiveGroupHandler->Init(bundleActiveCore_->bundleGroupController_);
    bundleActiveGroupHandler->SendCheckBundleMsg(0, handlerObject);
    bundleActiveGroupHandler->SendCheckBundleMsg(0, handlerObject, 10);
    auto msgKey = bundleActiveGroupHandler->GetMsgKey(0, handlerObject, 10);
    EXPECT_NE(bundleActiveGroupHandler->checkBundleTaskMap_.size(), 0);
    bundleActiveGroupHandler->RemoveCheckBundleMsg(msgKey);
    bundleActiveGroupHandler->RemoveCheckBundleMsg(msgKey);
    msgKey = bundleActiveGroupHandler->GetMsgKey(0, handlerObject, 0);
    bundleActiveGroupHandler->RemoveCheckBundleMsg(msgKey);
    EXPECT_EQ(bundleActiveGroupHandler->checkBundleTaskMap_.size(), 0);
}

/*
 * @tc.name: BundleActiveGroupHandler_004
 * @tc.desc: GetMsgKey
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_004, Function | MediumTest | Level0)
{
    BundleActiveGroupHandlerObject tmpObject;
    tmpObject.bundleName_ = "test";
    tmpObject.uid_ = 10000;
    tmpObject.userId_ = 100;
    auto handlerObject = std::make_shared<BundleActiveGroupHandlerObject>(tmpObject);
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    auto msgkey = bundleActiveGroupHandler->GetMsgKey(0, nullptr, 10);
    EXPECT_EQ(msgkey, "");
    msgkey = bundleActiveGroupHandler->GetMsgKey(0, handlerObject, 10);
    EXPECT_NE(msgkey, "");
}

/*
 * @tc.name: BundleActiveGroupHandler_005
 * @tc.desc: PostTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_005, Function | MediumTest | Level0)
{
    BundleActiveGroupHandlerObject tmpObject;
    auto handlerObject = std::make_shared<BundleActiveGroupHandlerObject>(tmpObject);
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
        bundleActiveGroupHandler->PostTask([]() {
        SUCCEED();
    });
    bundleActiveGroupHandler->PostSyncTask([]() {
        SUCCEED();
    });
    bundleActiveGroupHandler->Init(bundleActiveCore_->bundleGroupController_);
    EXPECT_TRUE(bundleActiveGroupHandler->isInited_);
    bundleActiveGroupHandler->PostTask([]() {
        SUCCEED();
    });
    bundleActiveGroupHandler->PostSyncTask([]() {
        SUCCEED();
    });
}

/*
 * @tc.name: BundleActiveGroupHandler_006
 * @tc.desc: PostTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_006, Function | MediumTest | Level0)
{
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    
    bundleActiveGroupHandler->Init(bundleActiveCore_->bundleGroupController_);
    int32_t eventId = 0;
    std::shared_ptr<BundleActiveGroupHandlerObject> tmpObject = nullptr;
    bundleActiveGroupHandler->SendCheckBundleMsg(eventId, tmpObject, 0);
    EXPECT_TRUE(bundleActiveGroupHandler->isInited_);
}

/*
 * @tc.name: BundleActiveGroupHandler_007
 * @tc.desc: PostTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_007, Function | MediumTest | Level0)
{
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    bundleActiveGroupHandler->Init(nullptr);
    EXPECT_FALSE(bundleActiveGroupHandler->isInited_);
}

/*
 * @tc.name: BundleActiveGroupHandler_008
 * @tc.desc: PostTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_008, Function | MediumTest | Level0)
{
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    int32_t eventId = 0;
    bundleActiveGroupHandler->RemoveEvent(eventId);
    EXPECT_FALSE(bundleActiveGroupHandler->isInited_);
}

/*
 * @tc.name: BundleActiveGroupHandler_009
 * @tc.desc: PostTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_009, Function | MediumTest | Level0)
{
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    int32_t eventId = 0;
    std::shared_ptr<BundleActiveGroupHandlerObject> tmpObject = nullptr;
    bundleActiveGroupHandler->ProcessEvent(eventId, tmpObject);
    EXPECT_TRUE(bundleActiveGroupHandler->bundleActiveGroupController_ == nullptr);
}

/*
 * @tc.name: BundleActiveGroupHandler_010
 * @tc.desc: PostTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActiveGroupHandler_010, Function | MediumTest | Level0)
{
    auto bundleActiveGroupHandler = std::make_shared<BundleActiveGroupHandler>(true);
    bundleActiveGroupHandler->Init(bundleActiveCore_->bundleGroupController_);
    int32_t eventId = 2;
    std::shared_ptr<BundleActiveGroupHandlerObject> handlerObject = nullptr;
    bundleActiveGroupHandler->ProcessEvent(eventId, handlerObject);
    BundleActiveGroupHandlerObject tmpObject;
    handlerObject = std::make_shared<BundleActiveGroupHandlerObject>(tmpObject);
    bundleActiveGroupHandler->ProcessEvent(eventId, handlerObject);
    eventId = 100;
    bundleActiveGroupHandler->ProcessEvent(eventId, handlerObject);
    EXPECT_TRUE(bundleActiveGroupHandler->isInited_);
}

/*
 * @tc.name: BundleActivePackageStats_001
 * @tc.desc: END_OF_THE_DAY
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActivePackageStats_001, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    std::string taskName = "test";
    int64_t timeStamp = 0;
    int32_t eventId = BundleActiveEvent::END_OF_THE_DAY;
    std::string abilityId = "1";
    int32_t uid = 0;
    packageStats->Update("test", timeStamp, eventId, abilityId, uid);
    packageStats->lastTimeUsed_ = 0;
    packageStats->totalInFrontTime_ = 0;
    timeStamp = 1000;
    packageStats->abilities_["test"] = BundleActiveEvent::ABILITY_FOREGROUND;
    packageStats->longTimeTasks_["test"] = BundleActiveEvent::ABILITY_FOREGROUND;
    packageStats->Update("test", timeStamp, eventId, abilityId, uid);
    EXPECT_EQ(packageStats->totalInFrontTime_, timeStamp);
}

/*
 * @tc.name: BundleActivePackageStats_002
 * @tc.desc: FLUSH
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActivePackageStats_002, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    std::string taskName = "test";
    int64_t timeStamp = 0;
    int32_t eventId = BundleActiveEvent::FLUSH;
    std::string abilityId = "1";
    int32_t uid = 0;
    packageStats->Update("test", timeStamp, eventId, abilityId, uid);
    packageStats->lastTimeUsed_ = 0;
    packageStats->totalInFrontTime_ = 0;
    timeStamp = 1000;
    packageStats->abilities_["test"] = BundleActiveEvent::ABILITY_FOREGROUND;
    packageStats->longTimeTasks_["test"] = BundleActiveEvent::ABILITY_FOREGROUND;
    packageStats->Update("test", timeStamp, eventId, abilityId, uid);
    EXPECT_EQ(packageStats->totalInFrontTime_, timeStamp);
}

/*
 * @tc.name: BundleActivePackageStats_003
 * @tc.desc: Marshalling
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActivePackageStats_003, Function | MediumTest | Level0)
{
    MessageParcel reply;
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    bool result = packageStats->Marshalling(reply);
    EXPECT_TRUE(result);
}

/*
 * @tc.name: BundleActivePackageStats_004
 * @tc.desc: UpdateAbility
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActivePackageStats_004, Function | MediumTest | Level0)
{
    MessageParcel reply;
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    int64_t timeStamp = 0;
    int32_t eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    std::string abilityId = "1";
    packageStats->abilities_[abilityId] = BundleActiveEvent::ABILITY_STOP;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);
    packageStats->abilities_[abilityId] = BundleActiveEvent::ABILITY_BACKGROUND;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);
    int32_t startCount = packageStats->startCount_;
    EXPECT_TRUE(startCount > 0);
}

/*
 * @tc.name: BundleActivePackageStats_005
 * @tc.desc: UpdateLongTimeTask
 * @tc.type: FUNC
 * @tc.require: IA4GZ0
 */
HWTEST_F(PackageUsageTest, BundleActivePackageStats_005, Function | MediumTest | Level0)
{
    auto packageStats = std::make_shared<BundleActivePackageStats>();
    std::string taskName = "test";
    int64_t timeStamp = 0;
    std::string abilityId = "1";
    int32_t eventId = BundleActiveEvent::LONG_TIME_TASK_ENDED;
    packageStats->totalContiniousTaskUsedTime_ = 0;
    packageStats->longTimeTasks_[taskName] = BundleActiveEvent::LONG_TIME_TASK_ENDED;
    packageStats->UpdateAbility(timeStamp, eventId, abilityId);
    EXPECT_EQ(packageStats->totalContiniousTaskUsedTime_, 0);
}

/*
 * @tc.name: BundleActiveCalendar_001
 * @tc.desc: BundleActiveCalendar
 * @tc.type: FUNC
 * @tc.require: IAHDJW
 */
HWTEST_F(PackageUsageTest, BundleActiveCalendar_001, Function | MediumTest | Level0)
{
    int64_t nowTime = bundleActiveCore_->GetSystemTimeMs();
    BundleActiveCalendar testCalendar(nowTime);
    testCalendar.TruncateToDay();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
    BundleActiveCalendar testCalendar2(nowTime);
    testCalendar.TruncateToWeek();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
    BundleActiveCalendar testCalendar3(nowTime);
    testCalendar.TruncateToMonth();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
    BundleActiveCalendar testCalendar4(nowTime);
    testCalendar.TruncateToYear();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
}

/*
 * @tc.name: BundleActiveCalendar_002
 * @tc.desc: BundleActiveCalendar debug
 * @tc.type: FUNC
 * @tc.require: IAHDJW
 */
HWTEST_F(PackageUsageTest, BundleActiveCalendar_002, Function | MediumTest | Level0)
{
    int64_t nowTime = bundleActiveCore_->GetSystemTimeMs();
    BundleActiveCalendar testCalendar(nowTime);
    testCalendar.ChangeToDebug();
    testCalendar.TruncateToDay();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
    BundleActiveCalendar testCalendar2(nowTime);
    testCalendar.ChangeToDebug();
    testCalendar.TruncateToWeek();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
    BundleActiveCalendar testCalendar3(nowTime);
    testCalendar.ChangeToDebug();
    testCalendar.TruncateToMonth();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
    BundleActiveCalendar testCalendar4(nowTime);
    testCalendar.ChangeToDebug();
    testCalendar.TruncateToYear();
    EXPECT_TRUE(nowTime - testCalendar.GetMilliseconds() >= 0);
}

}  // namespace DeviceUsageStats
}  // namespace OHOS
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

#include "bundle_active_service.h"
#include "bundle_active_user_service.h"
#include "bundle_active_event.h"
#include "app_group_callback_stub.h"
#include "app_group_callback_info.h"
#include "bundle_active_usage_database.h"
#include "bundle_active_user_history.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
class DeviceUsageStatisticsServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DeviceUsageStatisticsServiceTest::SetUpTestCase(void)
{
}

void DeviceUsageStatisticsServiceTest::TearDownTestCase(void)
{
}

void DeviceUsageStatisticsServiceTest::SetUp(void)
{
}

void DeviceUsageStatisticsServiceTest::TearDown(void)
{
}

class TestServiceAppGroupChangeCallback : public AppGroupCallbackStub {
public:
    void OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo) override;
};

void TestServiceAppGroupChangeCallback::OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
{
    BUNDLE_ACTIVE_LOGI("TestServiceAppGroupChangeCallback::OnAppGroupChanged!");
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetServiceObject_001
 * @tc.desc: get service object
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetServiceObject_001,
    Function | MediumTest | Level0)
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(systemAbilityManager, nullptr);

    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID);
    EXPECT_NE(remoteObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_dump_001
 * @tc.desc: test dump
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_dump_001, Function | MediumTest | Level0)
{
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_ = std::make_shared<BundleActiveCore>();
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_->Init();
    BUNDLE_ACTIVE_LOGI("DeviceUsageStatisticsServiceTest create BundleActiveService!");

    std::vector<std::string> dumpOption{"-A", "Events"};
    std::vector<std::string> dumpInfo;
    DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "Events", "0", "20000000000000", "100"};
    DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "PackageUsage"};
    DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "PackageUsage", "1", "0", "20000000000000", "100"};
    DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "ModuleUsage"};
    DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "ModuleUsage", "1", "100"};
    DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);

    std::vector<std::u16string> args;
    DelayedSingleton<BundleActiveService>::GetInstance()->Dump(-1, args);

    args.clear();
    args = {to_utf16("-h")};
    DelayedSingleton<BundleActiveService>::GetInstance()->Dump(-1, args);

    args.clear();
    args = {to_utf16("-A")};
    DelayedSingleton<BundleActiveService>::GetInstance()->Dump(-1, args);

    args.clear();
    args = {to_utf16("-D")};
    DelayedSingleton<BundleActiveService>::GetInstance()->Dump(-1, args);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_QueryModuleUsageRecords_001
 * @tc.desc: QueryModuleUsageRecords
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_QueryModuleUsageRecords_001,
    Function | MediumTest | Level0)
{
    std::vector<BundleActiveModuleRecord> results;
    int32_t maxNum = 0;
    ErrCode code = DelayedSingleton<BundleActiveService>::GetInstance()->QueryModuleUsageRecords(maxNum, results, 100);
    EXPECT_NE(code, 0);

    maxNum = 1001;
    code = DelayedSingleton<BundleActiveService>::GetInstance()->QueryModuleUsageRecords(maxNum, results, 100);
    EXPECT_NE(code, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_AppGroupCallback_001
 * @tc.desc: AppGroupCallback
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_AppGroupCallback_001,
    Function | MediumTest | Level0)
{
    BUNDLE_ACTIVE_LOGI("DeviceUsageStatisticsServiceTest create BundleActiveService!");
    sptr<TestServiceAppGroupChangeCallback> observer = new (std::nothrow) TestServiceAppGroupChangeCallback();
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_ = std::make_shared<BundleActiveCore>();
    EXPECT_EQ(DelayedSingleton<BundleActiveService>::GetInstance()->RegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->RegisterAppGroupCallBack(observer), ERR_OK);
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_->AddObserverDeathRecipient(observer);

    EXPECT_EQ(DelayedSingleton<BundleActiveService>::GetInstance()->UnRegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->UnRegisterAppGroupCallBack(observer), ERR_OK);

    observer = nullptr;
    EXPECT_NE(DelayedSingleton<BundleActiveService>::GetInstance()->RegisterAppGroupCallBack(observer), ERR_OK);

    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_->AddObserverDeathRecipient(observer);
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_->RemoveObserverDeathRecipient(observer);

    wptr<IRemoteObject> remote = nullptr;
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_->OnObserverDied(remote);
    DelayedSingleton<BundleActiveService>::GetInstance()->bundleActiveCore_->OnObserverDiedInner(remote);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_OnUserRemoved_001
 * @tc.desc: OnUserRemoved
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_OnUserRemoved_001,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->bundleGroupController_ = std::make_shared<BundleActiveGroupController>(coreObject->debugCore_);
    coreObject->InitBundleGroupController();
    coreObject->RestoreToDatabase(userId);
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(coreObject.get()), false);
    coreObject->userStatServices_[userId] = userService;

    BundleActiveEvent event;
    coreObject->ReportEventToAllUserId(event);
    coreObject->currentUsedUser_ = userId;
    coreObject->OnUserRemoved(userId);
    coreObject->OnUserSwitched(userId);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_RestoreAllData_001
 * @tc.desc: RestoreAllData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_RestoreAllData_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->bundleGroupController_ = std::make_shared<BundleActiveGroupController>(coreObject->debugCore_);
    coreObject->InitBundleGroupController();

    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(coreObject.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    coreObject->userStatServices_[userId] = userService;
    userId = 101;
    coreObject->userStatServices_[userId] = nullptr;
    coreObject->RestoreAllData();

    BundleActiveEvent event;
    coreObject->ReportEventToAllUserId(event);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ObtainSystemEventName_001
 * @tc.desc: ObtainSystemEventName
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ObtainSystemEventName_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->bundleGroupController_ = std::make_shared<BundleActiveGroupController>(coreObject->debugCore_);
    BundleActiveEvent event;
    event.eventId_ = BundleActiveEvent::SYSTEM_LOCK;
    coreObject->ObtainSystemEventName(event);

    event.eventId_ = BundleActiveEvent::SYSTEM_UNLOCK;
    coreObject->ObtainSystemEventName(event);

    event.eventId_ = BundleActiveEvent::SYSTEM_SLEEP;
    coreObject->ObtainSystemEventName(event);

    event.eventId_ = BundleActiveEvent::SYSTEM_WAKEUP;
    coreObject->ObtainSystemEventName(event);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_JudgeQueryCondition_001
 * @tc.desc: JudgeQueryCondition
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_JudgeQueryCondition_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int64_t beginTime = 0;
    int64_t endTime = 0;
    int64_t eventTableTime = 0;
    database->eventTableName_ = "defaultTableName";
    EXPECT_EQ(database->JudgeQueryCondition(beginTime, endTime, eventTableTime), QUERY_CONDITION_INVALID);

    endTime = 10;
    eventTableTime = 11;
    EXPECT_EQ(database->JudgeQueryCondition(beginTime, endTime, eventTableTime), QUERY_CONDITION_INVALID);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetSystemEventName_001
 * @tc.desc: GetSystemEventName
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetSystemEventName_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t userId = BundleActiveEvent::SYSTEM_LOCK;
    EXPECT_EQ(database->GetSystemEventName(userId), "SYSTEM_LOCK");

    userId = BundleActiveEvent::SYSTEM_UNLOCK;
    EXPECT_EQ(database->GetSystemEventName(userId), "SYSTEM_UNLOCK");

    userId = BundleActiveEvent::SYSTEM_SLEEP;
    EXPECT_EQ(database->GetSystemEventName(userId), "SYSTEM_SLEEP");

    userId = BundleActiveEvent::SYSTEM_WAKEUP;
    EXPECT_EQ(database->GetSystemEventName(userId), "SYSTEM_WAKEUP");

    userId = BundleActiveEvent::ABILITY_FOREGROUND;
    EXPECT_EQ(database->GetSystemEventName(userId), "");
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetOverdueTableCreateTime_001
 * @tc.desc: GetOverdueTableCreateTime
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetOverdueTableCreateTime_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 4;
    int64_t currentTimeMillis = 20000000000000;
    database->GetOverdueTableCreateTime(databaseType, currentTimeMillis);

    databaseType = 0;
    database->GetOverdueTableCreateTime(databaseType, currentTimeMillis);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_DeleteInvalidTable_001
 * @tc.desc: DeleteInvalidTable
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_DeleteInvalidTable_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 4;
    int64_t currentTimeMillis = 20000000000000;
    database->DeleteInvalidTable(databaseType, currentTimeMillis);

    databaseType = 0;
    database->DeleteInvalidTable(databaseType, currentTimeMillis);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CreatePackageLogTable_001
 * @tc.desc: CreatePackageLogTable
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CreatePackageLogTable_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    int64_t currentTimeMillis = 20000000000000;
    database->CreatePackageLogTable(databaseType, currentTimeMillis);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CreateModuleRecordTable_001
 * @tc.desc: CreateModuleRecordTable
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CreateModuleRecordTable_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    int64_t currentTimeMillis = 20000000000000;
    database->CreateModuleRecordTable(databaseType, currentTimeMillis);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CreateFormRecordTable_001
 * @tc.desc: CreateFormRecordTable
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CreateFormRecordTable_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    int64_t currentTimeMillis = 20000000000000;
    database->CreateFormRecordTable(databaseType, currentTimeMillis);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CreateDurationTable_001
 * @tc.desc: CreateDurationTable
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CreateDurationTable_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t databaseType = DAILY_DATABASE_INDEX;
    bool forModuleRecords = true;
    database->InitUsageGroupDatabase(databaseType, forModuleRecords);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CreateBundleHistoryTable_001
 * @tc.desc: CreateBundleHistoryTable
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CreateBundleHistoryTable_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    database->CreateBundleHistoryTable(databaseType);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_PutBundleHistoryData_001
 * @tc.desc: PutBundleHistoryData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_PutBundleHistoryData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t userId = 100;
    std::shared_ptr<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>> userHistory = nullptr;
    database->PutBundleHistoryData(userId, userHistory);

    userHistory = std::make_shared<std::map<string, std::shared_ptr<BundleActivePackageHistory>>>();
    userHistory->emplace("defaultTest", std::make_shared<BundleActivePackageHistory>());
    database->PutBundleHistoryData(userId, userHistory);
    database->PutBundleHistoryData(userId, userHistory);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetBundleHistoryData_001
 * @tc.desc: GetBundleHistoryData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetBundleHistoryData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t userId = 100;
    database->GetBundleHistoryData(userId);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_FlushPackageInfo_001
 * @tc.desc: FlushPackageInfo
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_FlushPackageInfo_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    BundleActivePeriodStats stats;
    stats.bundleStats_.emplace("defaultTest", std::make_shared<BundleActivePackageStats>());
    database->FlushPackageInfo(databaseType, stats);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetCurrentUsageData_001
 * @tc.desc: GetCurrentUsageData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetCurrentUsageData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t databaseType = -1;
    int32_t userId = 100;
    database->GetCurrentUsageData(databaseType, userId);

    databaseType = 4;
    database->GetCurrentUsageData(databaseType, userId);

    databaseType = 0;
    database->GetCurrentUsageData(databaseType, userId);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetTableIndexSql_001
 * @tc.desc: GetTableIndexSql
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetTableIndexSql_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    int64_t tableTime = 20000000000000;
    bool createFlag = false;
    int32_t indexFlag = 0;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);

    createFlag = true;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);

    databaseType = 4;
    createFlag = false;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);

    databaseType = 5;
    createFlag = false;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);
    indexFlag = BUNDLE_ACTIVE_DB_INDEX_MODULE;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);

    createFlag = true;
    indexFlag = BUNDLE_ACTIVE_DB_INDEX_NORMAL;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);

    indexFlag = BUNDLE_ACTIVE_DB_INDEX_MODULE;
    database->GetTableIndexSql(databaseType, tableTime, createFlag, indexFlag);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_RenameTableName_001
 * @tc.desc: RenameTableName
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_RenameTableName_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    uint32_t databaseType = 0;
    int64_t tableOldTime = 0;
    int64_t tableNewTime = 20000000000000;
    database->RenameTableName(databaseType, tableOldTime, tableNewTime);

    databaseType = 4;
    database->RenameTableName(databaseType, tableOldTime, tableNewTime);

    databaseType = 5;
    database->RenameTableName(databaseType, tableOldTime, tableNewTime);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_RemoveOldData_001
 * @tc.desc: RemoveOldData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_RemoveOldData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int64_t currentTime = 20000000000000;
    database->RemoveOldData(currentTime);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_QueryDatabaseUsageStats_001
 * @tc.desc: QueryDatabaseUsageStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_QueryDatabaseUsageStats_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t databaseType = -1;
    int64_t beginTime = 0;
    int64_t endTime = 0;
    int32_t userId = 100;
    EXPECT_EQ(database->QueryDatabaseUsageStats(databaseType, beginTime, endTime, userId).size(), 0);

    databaseType = 4;
    EXPECT_EQ(database->QueryDatabaseUsageStats(databaseType, beginTime, endTime, userId).size(), 0);

    databaseType = 0;
    EXPECT_EQ(database->QueryDatabaseUsageStats(databaseType, beginTime, endTime, userId).size(), 0);

    databaseType = 0;
    endTime = 20000000000000;
    database->QueryDatabaseUsageStats(databaseType, beginTime, endTime, userId);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetSystemTimeMs_001
 * @tc.desc: GetSystemTimeMs
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetSystemTimeMs_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    database->GetSystemTimeMs();
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ReportEvent_001
 * @tc.desc: ReportEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ReportEvent_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->Init();
    coreObject->InitBundleGroupController();

    BundleActiveEvent event;
    int32_t userId = 0;
    EXPECT_NE(coreObject->ReportEvent(event, userId), ERR_OK);

    userId = 101;
    event.bundleName_ = "com.ohos.launcher";
    EXPECT_EQ(coreObject->ReportEvent(event, userId), ERR_OK);

    event.bundleName_ = "com.ohos.settings";
    event.eventId_ = 15;
    EXPECT_EQ(coreObject->ReportEvent(event, userId), ERR_OK);

    event.eventId_ = 16;
    EXPECT_EQ(coreObject->ReportEvent(event, userId), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_InitBundleGroupController_001
 * @tc.desc: InitBundleGroupController
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_InitBundleGroupController_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->InitBundleGroupController();
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ReportEventToAllUserId_001
 * @tc.desc: ReportEventToAllUserId
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ReportEventToAllUserId_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(coreObject.get()), false);
    coreObject->userStatServices_[userId] = userService;
    userId = 101;
    coreObject->userStatServices_[userId] = nullptr;
    BundleActiveEvent event;
    coreObject->ReportEventToAllUserId(event);

    coreObject->Init();
    coreObject->InitBundleGroupController();
    coreObject->ReportEventToAllUserId(event);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_RestoreToDatabase_001
 * @tc.desc: RestoreToDatabase
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_RestoreToDatabase_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->bundleGroupController_ = std::make_shared<BundleActiveGroupController>(coreObject->debugCore_);
    coreObject->InitBundleGroupController();

    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(coreObject.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    coreObject->userStatServices_[userId] = userService;
    coreObject->RestoreToDatabase(userId);

    userId = 101;
    coreObject->RestoreToDatabase(userId);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ShutDown_001
 * @tc.desc: ShutDown
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ShutDown_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->Init();
    coreObject->InitBundleGroupController();
    coreObject->ShutDown();
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CheckTimeChangeAndGetWallTime_001
 * @tc.desc: CheckTimeChangeAndGetWallTime
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CheckTimeChangeAndGetWallTime_001,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->OnUserRemoved(100);
    coreObject->Init();
    coreObject->InitBundleGroupController();
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(coreObject.get()), false);
    coreObject->userStatServices_[userId] = userService;

    coreObject->CheckTimeChangeAndGetWallTime(userId);
    coreObject->OnUserSwitched(userId);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_QueryAppGroup_001
 * @tc.desc: QueryAppGroup
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_QueryAppGroup_001,
    Function | MediumTest | Level0)
{
    auto groupController = std::make_shared<BundleActiveGroupController>(false);
    int32_t appGroup = 10;
    std::string bundleName = "";
    int32_t userId = 100;
    EXPECT_NE(groupController->QueryAppGroup(appGroup, bundleName, userId), ERR_OK);

    groupController->sptrBundleMgr_ = nullptr;
    bundleName = "defaultBundleName";
    EXPECT_NE(groupController->QueryAppGroup(appGroup, bundleName, userId), ERR_OK);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ControllerReportEvent_001
 * @tc.desc: ControllerReportEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ControllerReportEvent_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->Init();
    coreObject->InitBundleGroupController();

    int64_t bootBasedTimeStamp = 20000000000000;
    BundleActiveEvent event;
    int32_t userId = 100;
    coreObject->bundleGroupController_->bundleGroupEnable_ = false;
    coreObject->bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);

    coreObject->bundleGroupController_->bundleGroupEnable_ = true;
    event.bundleName_ = "com.ohos.camera";
    coreObject->bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);

    event.eventId_ = BundleActiveEvent::NOTIFICATION_SEEN;
    coreObject->bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);

    event.eventId_ = BundleActiveEvent::SYSTEM_INTERACTIVE;
    coreObject->bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_CheckAndUpdateGroup_001
 * @tc.desc: CheckAndUpdateGroup
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_CheckAndUpdateGroup_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->Init();
    coreObject->InitBundleGroupController();

    std::string bundleName = "com.ohos.camera";
    int32_t userId = 100;
    int64_t bootBasedTimeStamp = 20000000000000;
    int32_t newGroup = 30;
    uint32_t reason = GROUP_CONTROL_REASON_TIMEOUT;
    bool isFlush = false;

    int32_t appGroup = 0;
    coreObject->bundleGroupController_->QueryAppGroup(appGroup, bundleName, userId);

    coreObject->bundleGroupController_->SetAppGroup(bundleName, userId, newGroup, reason, bootBasedTimeStamp, isFlush);
    coreObject->bundleGroupController_->CheckAndUpdateGroup(bundleName, userId, bootBasedTimeStamp);

    newGroup = 20;
    reason = GROUP_CONTROL_REASON_CALCULATED;
    coreObject->bundleGroupController_->SetAppGroup(bundleName, userId, newGroup, reason, bootBasedTimeStamp, isFlush);
    coreObject->bundleGroupController_->CheckAndUpdateGroup(bundleName, userId, bootBasedTimeStamp);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_PreservePowerStateInfo_001
 * @tc.desc: PreservePowerStateInfo
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_PreservePowerStateInfo_001,
    Function | MediumTest | Level0)
{
    auto coreObject = std::make_shared<BundleActiveCore>();
    coreObject->Init();
    coreObject->InitBundleGroupController();

    int32_t eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    coreObject->PreservePowerStateInfo(eventId);
    EXPECT_NE(coreObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_UpdateModuleData_001
 * @tc.desc: UpdateModuleData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_UpdateModuleData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    auto moduleRecords = std::map<std::string, std::shared_ptr<BundleActiveModuleRecord>>();
    int64_t timeStamp = 20000000000;
    int32_t userId = 100;
    database->UpdateModuleData(userId, moduleRecords, timeStamp);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_QueryNotificationEventStats_001
 * @tc.desc: QueryNotificationEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_QueryNotificationEventStats_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int32_t eventId = 2;
    int64_t beginTime = 0;
    int64_t endTime = 0;
    auto notificationEventStats = std::map<std::string, BundleActiveEventStats>();
    int32_t userId = 100;
    BUNDLE_ACTIVE_LOGI("database->QueryNotificationEventStats");
    database->QueryNotificationEventStats(eventId, beginTime, endTime, notificationEventStats, userId);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_QueryDeviceEventStats_001
 * @tc.desc: QueryDeviceEventStats
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_QueryDeviceEventStats_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int32_t eventId = 2;
    int64_t beginTime = 0;
    int64_t endTime = 0;
    auto eventStats = std::map<std::string, BundleActiveEventStats>();
    int32_t userId = 100;
    BUNDLE_ACTIVE_LOGI("database->QueryDeviceEventStats");
    database->QueryDeviceEventStats(eventId, beginTime, endTime, eventStats, userId);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_QueryDatabaseEvents_001
 * @tc.desc: QueryDatabaseEvents
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_QueryDatabaseEvents_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int64_t beginTime = 0;
    int64_t endTime = 0;
    std::string bundleName;
    int32_t userId = 100;
    BUNDLE_ACTIVE_LOGI("database->QueryDatabaseEvents");
    database->QueryDatabaseEvents(beginTime, endTime, userId, bundleName);

    bundleName = "com.ohos.camera";
    database->QueryDatabaseEvents(beginTime, endTime, userId, bundleName);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_UpdateEventData_001
 * @tc.desc: UpdateEventData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_UpdateEventData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int32_t databaseType = WEEKLY_DATABASE_INDEX;
    BundleActivePeriodStats stats;
    database->UpdateEventData(databaseType, stats);

    databaseType = DAILY_DATABASE_INDEX;
    database->UpdateEventData(databaseType, stats);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_UpdateBundleUsageData_001
 * @tc.desc: UpdateBundleUsageData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_UpdateBundleUsageData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int32_t databaseType = -1;
    BundleActivePeriodStats stats;
    database->UpdateBundleUsageData(databaseType, stats);

    databaseType = EVENT_DATABASE_INDEX;
    database->UpdateBundleUsageData(databaseType, stats);

    databaseType = DAILY_DATABASE_INDEX;
    database->UpdateBundleUsageData(databaseType, stats);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_RenewTableTime_001
 * @tc.desc: RenewTableTime
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_RenewTableTime_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    int64_t timeDiffMillis = 0;
    database->sortedTableArray_[0] = {-1, 0};
    database->RenewTableTime(timeDiffMillis);

    database->eventTableName_ = "";
    database->RenewTableTime(timeDiffMillis);

    database->eventTableName_ = "defaultTableName";
    database->RenewTableTime(timeDiffMillis);

    database->formRecordsTableName_ = "defaultFormRecordsTableName";
    database->RenewTableTime(timeDiffMillis);

    database->moduleRecordsTableName_ = "defaultModuleRecordsTableName_";
    database->RenewTableTime(timeDiffMillis);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_SetNewIndexWhenTimeChanged_001
 * @tc.desc: SetNewIndexWhenTimeChanged
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_SetNewIndexWhenTimeChanged_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();

    uint32_t databaseType = APP_GROUP_DATABASE_INDEX;
    int64_t tableOldTime = 0;
    int64_t tableNewTime = 20000000000000;
    std::shared_ptr<NativeRdb::RdbStore> rdbStore = nullptr;
    EXPECT_NE(database->SetNewIndexWhenTimeChanged(databaseType, tableOldTime, tableNewTime, rdbStore), ERR_OK);

    rdbStore = database->GetBundleActiveRdbStore(databaseType);
    database->SetNewIndexWhenTimeChanged(databaseType, tableOldTime, tableNewTime, rdbStore);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ReportContinuousTaskEvent_001
 * @tc.desc: ReportContinuousTaskEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ReportContinuousTaskEvent_001,
    Function | MediumTest | Level0)
{
    #ifdef BGTASKMGR_ENABLE
    auto bgtaskObserver = std::make_shared<BundleActiveContinuousTaskObserver>();
    auto continuousTaskCallbackInfo = std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>();
    continuousTaskCallbackInfo->creatorUid_ = 20000000;
    bool isStart = false;
    bgtaskObserver->ReportContinuousTaskEvent(continuousTaskCallbackInfo, isStart);

    isStart = true;
    bgtaskObserver->ReportContinuousTaskEvent(continuousTaskCallbackInfo, isStart);
    EXPECT_NE(bgtaskObserver, nullptr);
    #endif
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_RemoveFormData_001
 * @tc.desc: RemoveFormData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_RemoveFormData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t userId = 100;
    std::string bundleName = "defaultBundleName";
    std::string moduleName = "defaultModuleName";
    std::string formName = "defaultFormName";
    database->InitUsageGroupDatabase(0, true);
    database->RemoveFormData(userId, bundleName, moduleName, formName, 0, 0);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_UpdateFormData_001
 * @tc.desc: UpdateFormData
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_UpdateFormData_001,
    Function | MediumTest | Level0)
{
    auto database = std::make_shared<BundleActiveUsageDatabase>();
    int32_t userId = 100;
    std::string bundleName = "defaultBundleName";
    std::string moduleName = "defaultModuleName";
    std::string formName = "defaultFormName";
    database->InitUsageGroupDatabase(0, true);
    BundleActiveFormRecord formRecord;
    auto rdbStore = database->GetBundleActiveRdbStore(0);
    database->UpdateFormData(userId, bundleName, moduleName, formRecord, rdbStore);
    EXPECT_NE(database, nullptr);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
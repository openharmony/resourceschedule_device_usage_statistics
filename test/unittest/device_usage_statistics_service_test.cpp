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
#include <thread>

#include <gtest/gtest.h>
#include "system_ability_definition.h"

#include "bundle_active_service.h"
#include "bundle_active_user_service.h"
#include "bundle_active_event.h"
#include "app_group_callback_stub.h"
#include "app_group_callback_info.h"
#include "bundle_active_usage_database.h"
#include "bundle_active_user_history.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_log.h"
#include "bundle_active_config_reader.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "nativetoken_kit.h"
#include "bundle_active_report_controller.h"
#include "bundle_active_test_util.h"
#include "bundle_active_util.h"
#include "bundle_active_constant.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {
using namespace Security::AccessToken;
class DeviceUsageStatisticsServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static std::shared_ptr<BundleActiveCore> bundleActiveCore_;
};

std::shared_ptr<BundleActiveCore> DeviceUsageStatisticsServiceTest::bundleActiveCore_ = nullptr;

void DeviceUsageStatisticsServiceTest::SetUpTestCase(void)
{
    static const char *perms[] = {
        "ohos.permission.BUNDLE_ACTIVE_INFO",
    };
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "DeviceUsageStatisticsServiceTest",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
    bundleActiveCore_ = BundleActiveTestUtil::TestInit();
}

void DeviceUsageStatisticsServiceTest::TearDownTestCase(void)
{
    bundleActiveCore_->DeInit();
    BundleActiveTestUtil::TestDeInit();
    int64_t sleepTime = 10;
    std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
}

void DeviceUsageStatisticsServiceTest::SetUp(void)
{
}

void DeviceUsageStatisticsServiceTest::TearDown(void)
{
    int64_t sleepTime = 300;
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}

class TestServiceAppGroupChangeCallback : public AppGroupCallbackStub {
public:
    ErrCode OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo) override;
};

ErrCode TestServiceAppGroupChangeCallback::OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
{
    BUNDLE_ACTIVE_LOGI("TestServiceAppGroupChangeCallback::OnAppGroupChanged!");
    return ERR_OK;
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
        systemAbilityManager->GetSystemAbility(BACKGROUND_TASK_MANAGER_SERVICE_ID);
    EXPECT_NE(remoteObject, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetNameAndIndexForUid_001
 * @tc.desc: get service object
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetNameAndIndexForUid_001,
    Function | MediumTest | Level0)
{
    auto bundleActiveService = std::make_shared<BundleActiveService>();
    int32_t uid = 10;
    int32_t result = bundleActiveService->GetNameAndIndexForUid(uid);
    EXPECT_TRUE(result == -1);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_dump_001
 * @tc.desc: test dump
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_dump_001, Function | MediumTest | Level0)
{
    auto bundleActiveService = std::make_shared<BundleActiveService>();
    bundleActiveService->bundleActiveCore_ = std::make_shared<BundleActiveCore>();
    bundleActiveService->bundleActiveCore_->Init();
    BUNDLE_ACTIVE_LOGI("DeviceUsageStatisticsServiceTest create BundleActiveService!");

    std::vector<std::string> dumpOption{"-A", "Events"};
    std::vector<std::string> dumpInfo;
    bundleActiveService->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "Events", "0", "20000000000000", "100"};
    bundleActiveService->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "PackageUsage"};
    bundleActiveService->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "PackageUsage", "1", "0", "20000000000000", "100"};
    bundleActiveService->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "ModuleUsage"};
    int32_t ret;
    ret = bundleActiveService->ShellDump(dumpOption, dumpInfo);
    EXPECT_TRUE(ret == -1);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "ModuleUsage", "1", "100"};
    bundleActiveService->ShellDump(dumpOption, dumpInfo);

    std::vector<std::u16string> args;
    bundleActiveService->Dump(-1, args);

    args.clear();
    args = {to_utf16("-h")};
    bundleActiveService->Dump(-1, args);

    args.clear();
    args = {to_utf16("-A")};
    bundleActiveService->Dump(-1, args);

    args.clear();
    args = {to_utf16("-D")};
    bundleActiveService->Dump(-1, args);
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
    auto bundleActiveService = std::make_shared<BundleActiveService>();
    std::vector<BundleActiveModuleRecord> results;
    int32_t maxNum = 0;
    ErrCode code = bundleActiveService->QueryModuleUsageRecords(maxNum, results, 100);
    EXPECT_NE(code, 0);

    maxNum = 1001;
    code = bundleActiveService->QueryModuleUsageRecords(maxNum, results, 100);
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
    auto bundleActiveService = std::make_shared<BundleActiveService>();
    BUNDLE_ACTIVE_LOGI("DeviceUsageStatisticsServiceTest create BundleActiveService!");
    sptr<TestServiceAppGroupChangeCallback> observer = new (std::nothrow) TestServiceAppGroupChangeCallback();
    bundleActiveService->bundleActiveCore_ = std::make_shared<BundleActiveCore>();
    EXPECT_EQ(bundleActiveService->RegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(bundleActiveService->RegisterAppGroupCallBack(observer), ERR_OK);
    bundleActiveService->bundleActiveCore_->AddObserverDeathRecipient(observer);

    EXPECT_EQ(bundleActiveService->UnRegisterAppGroupCallBack(observer), ERR_OK);
    EXPECT_NE(bundleActiveService->UnRegisterAppGroupCallBack(observer), ERR_OK);

    observer = nullptr;
    EXPECT_NE(bundleActiveService->RegisterAppGroupCallBack(observer), ERR_OK);

    bundleActiveService->bundleActiveCore_->AddObserverDeathRecipient(observer);
    bundleActiveService->bundleActiveCore_->RemoveObserverDeathRecipient(observer);

    wptr<IRemoteObject> remote = nullptr;
    bundleActiveService->bundleActiveCore_->OnObserverDied(remote);
    bundleActiveService->bundleActiveCore_->OnObserverDiedInner(remote);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_AppGroupCallback_002
 * @tc.desc: AppGroupCallback
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_AppGroupCallback_002,
    Function | MediumTest | Level0)
{
    sptr<TestServiceAppGroupChangeCallback> observer = new (std::nothrow) TestServiceAppGroupChangeCallback();
    Security::AccessToken::AccessTokenID tokenId {};
    bundleActiveCore_->groupChangeObservers_[tokenId] = observer;
    int32_t userId = 100;
    int32_t newGroup = 10;
    int32_t oldGroup = 60;
    int32_t reasonInGroup = 0;
    AppGroupCallbackInfo appGroupCallbackInfo(userId, newGroup, oldGroup, reasonInGroup, "test");
    bundleActiveCore_->OnAppGroupChanged(appGroupCallbackInfo);
    SUCCEED();
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
    bundleActiveCore_->RestoreToDatabase(userId);
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    bundleActiveCore_->userStatServices_[userId] = userService;

    BundleActiveEvent event;
    bundleActiveCore_->ReportEventToAllUserId(event);
    bundleActiveCore_->currentUsedUser_ = userId;
    bundleActiveCore_->OnUserRemoved(userId);
    bundleActiveCore_->OnUserSwitched(userId);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    bundleActiveCore_->userStatServices_[userId] = userService;
    userId = 101;
    bundleActiveCore_->userStatServices_[userId] = nullptr;
    bundleActiveCore_->RestoreAllData();

    BundleActiveEvent event;
    bundleActiveCore_->ReportEventToAllUserId(event);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    BundleActiveEvent event;
    event.eventId_ = BundleActiveEvent::SYSTEM_LOCK;
    bundleActiveCore_->ObtainSystemEventName(event);

    event.eventId_ = BundleActiveEvent::SYSTEM_UNLOCK;
    bundleActiveCore_->ObtainSystemEventName(event);

    event.eventId_ = BundleActiveEvent::SYSTEM_SLEEP;
    bundleActiveCore_->ObtainSystemEventName(event);

    event.eventId_ = BundleActiveEvent::SYSTEM_WAKEUP;
    bundleActiveCore_->ObtainSystemEventName(event);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
 * @tc.name: DeviceUsageStatisticsServiceTest_ReportEvent_001
 * @tc.desc: ReportEvent
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ReportEvent_001,
    Function | MediumTest | Level0)
{
    auto bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    BundleActiveEvent event;
    int32_t userId = 0;
    EXPECT_NE(bundleActiveCore->ReportEvent(event, userId), ERR_OK);

    userId = 101;
    event.bundleName_ = "com.ohos.launcher";
    EXPECT_EQ(bundleActiveCore->ReportEvent(event, userId), ERR_OK);

    event.bundleName_ = "com.ohos.settings";
    event.eventId_ = 15;
    EXPECT_EQ(bundleActiveCore->ReportEvent(event, userId), ERR_OK);

    event.eventId_ = 16;
    EXPECT_EQ(bundleActiveCore->ReportEvent(event, userId), ERR_OK);
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
    bundleActiveCore_->InitBundleGroupController();
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
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    bundleActiveCore_->userStatServices_[userId] = userService;
    userId = 101;
    bundleActiveCore_->userStatServices_[userId] = nullptr;
    BundleActiveEvent event;
    bundleActiveCore_->ReportEventToAllUserId(event);

    bundleActiveCore_->Init();
    bundleActiveCore_->InitBundleGroupController();
    bundleActiveCore_->ReportEventToAllUserId(event);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    coreObject->Init();
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
 * @tc.name: BundleActiveGroupControllerTest_001
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_001,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    bundleActiveCore_->userStatServices_[userId] = userService;
    bundleActiveCore_->OnUserRemoved(userId);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupControllerTest_002
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_002,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    bundleActiveCore_->userStatServices_[userId] = userService;
    BundleActiveGroupController::GetInstance().Init(false);
    BundleActiveGroupController::GetInstance().OnUserSwitched(userId, userId);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupControllerTest_003
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_003,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    bundleActiveCore_->userStatServices_[userId] = userService;
    EXPECT_EQ(BundleActiveGroupController::GetInstance().GetNewGroup("test", userId, timeStamp, 0), -1);
}

/*
 * @tc.name: BundleActiveGroupControllerTest_004
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_004,
    Function | MediumTest | Level0)
{
    int userId = 100;
    BundleActiveEvent event;
    int64_t timeStamp = 20000000000000;
    BundleActiveGroupController::GetInstance().ReportEvent(event, timeStamp, userId);
    SUCCEED();
    BundleActiveGroupController::GetInstance().ReportEvent(event, timeStamp, 19);
    SUCCEED();
    BundleActiveGroupController::GetInstance().ReportEvent(event, timeStamp, 7);
    SUCCEED();
    BundleActiveGroupController::GetInstance().bundleGroupEnable_ = false;
    SUCCEED();
    BundleActiveGroupController::GetInstance().ReportEvent(event, timeStamp, userId);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupControllerTest_005
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_005,
    Function | MediumTest | Level0)
{
    int userId = 100;
    int64_t timeStamp = 20000000000000;
    BundleActiveGroupController::GetInstance().CheckAndUpdateGroup("test", userId, 0, timeStamp);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupControllerTest_006
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_006,
    Function | MediumTest | Level0)
{
    int userId = 100;
    int64_t time = 20000000000000;
    EXPECT_EQ(BundleActiveGroupController::GetInstance().SetAppGroup("test", userId, 0, 0, time, true),
        ERR_NO_APP_GROUP_INFO_IN_DATABASE);
}

/*
 * @tc.name: BundleActiveGroupControllerTest_007
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_007,
    Function | MediumTest | Level0)
{
    int userId = 100;
    EXPECT_EQ(BundleActiveGroupController::GetInstance().IsBundleIdle("test", userId), -1);
}

/*
 * @tc.name: BundleActiveGroupControllerTest_009
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_009,
    Function | MediumTest | Level0)
{
    bundleActiveCore_->InitBundleGroupController();
    int userId = 100;
    EXPECT_EQ(BundleActiveGroupController::GetInstance().IsBundleInstalled("test", userId), false);
    EXPECT_EQ(BundleActiveGroupController::GetInstance().IsBundleInstalled("test", userId), false);
}

/*
 * @tc.name: BundleActiveGroupControllerTest_010
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_010,
    Function | MediumTest | Level0)
{
    int userId = 100;
    int64_t timeStamp = 20000000000000;
    BundleActiveGroupController::GetInstance().ShutDown(timeStamp, userId);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupControllerTest_011
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: IssuesIA9M7I
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_011,
    Function | MediumTest | Level0)
{
    bool isScreenOn = true;
    int64_t timeStamp = 0;
    BundleActiveGroupController::GetInstance().OnScreenChanged(isScreenOn, timeStamp);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveGroupControllerTest_012
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: IssuesIA9M7I
 */
 HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_012,
    Function | MediumTest | Level0)
{
    BundleActiveGroupController::GetInstance().ShutDown(1, 10);
    EXPECT_NE(BundleActiveGroupController::GetInstance().bundleUserHistory_, nullptr);
}

/*
 * @tc.name: BundleActiveGroupControllerTest_013
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: IssuesIA9M7I
 */
 HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_013,
    Function | MediumTest | Level0)
{
    BundleActiveGroupController::GetInstance().OnUserRemoved(10);
    EXPECT_NE(BundleActiveGroupController::GetInstance().bundleUserHistory_, nullptr);
}

/*
 * @tc.name: BundleActiveGroupControllerTest_014
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: IssuesIA9M7I
 */
 HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveGroupControllerTest_014,
    Function | MediumTest | Level0)
{
    BundleActiveGroupController::GetInstance().OnUserSwitched(1, 10);
    EXPECT_NE(BundleActiveGroupController::GetInstance().bundleUserHistory_, nullptr);
}

/*
 * @tc.name: BundleActiveUserHistoryTest_001
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveUserHistoryTest_001,
    Function | MediumTest | Level0)
{
    int64_t bootBasedTimeStamp = 2000;
    std::vector<int64_t> screenTimeLevel;
    std::vector<int64_t> bootFromTimeLevel;
    auto bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootBasedTimeStamp, bundleActiveCore_);
    bundleUserHistory_->GetLevelIndex("test", 0, 20000, screenTimeLevel, bootFromTimeLevel, 0);
}

/*
 * @tc.name: BundleActiveUserHistoryTest_002
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveUserHistoryTest_002,
    Function | MediumTest | Level0)
{
    int64_t bootBasedTimeStamp = 2000;
    auto bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootBasedTimeStamp, bundleActiveCore_);
    bundleUserHistory_->WriteDeviceDuration();
    bundleUserHistory_->OnBundleUninstalled(0, "test", 0, 0);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveUserHistoryTest_003
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveUserHistoryTest_003,
    Function | MediumTest | Level0)
{
    int64_t bootBasedTimeStamp = 2000;
    auto oneBundleUsageHistory = std::make_shared<BundleActivePackageHistory>();
    auto bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootBasedTimeStamp, bundleActiveCore_);
    BundleActiveEvent event;
    bundleUserHistory_->ReportUsage(oneBundleUsageHistory, event, 0, 0, 1000, 2000, 100);
    SUCCEED();
    bundleUserHistory_->ReportUsage(oneBundleUsageHistory, event, 0, 0, 2000, 1000, 100);
    SUCCEED();
    bundleUserHistory_->ReportUsage(oneBundleUsageHistory, event, 10, 0, 1000, 2000, 100);
    SUCCEED();
    bundleUserHistory_->ReportUsage(oneBundleUsageHistory, event, 20, 0, 1000, 2000, 100);
    SUCCEED();
    bundleUserHistory_->ReportUsage(oneBundleUsageHistory, event, 20, 0, 0, 2000, 100);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveUserHistoryTest_004
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveUserHistoryTest_004,
    Function | MediumTest | Level0)
{
    int32_t userId = 100;
    int64_t bootBasedTimeStamp = 2000;
    int32_t newgroup = 0;
    uint32_t groupReason = 0;
    int32_t uid = 0;
    auto bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootBasedTimeStamp, bundleActiveCore_);
    bundleUserHistory_->SetAppGroup("test", userId, uid, bootBasedTimeStamp, newgroup, groupReason, true);
    SUCCEED();
    bundleUserHistory_->SetAppGroup("test", userId, uid, bootBasedTimeStamp, newgroup, groupReason, false);
    SUCCEED();
}

/*
 * @tc.name: BundleActiveUserHistoryTest_005
 * @tc.desc: test the interface
 * @tc.type: FUNC
 * @tc.require: DTS2023121404861
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, BundleActiveUserHistoryTest_005,
    Function | MediumTest | Level0)
{
    int64_t bootBasedTimeStamp = 2000;
    auto bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootBasedTimeStamp, bundleActiveCore_);
    bundleUserHistory_->PrintData(0);
    SUCCEED();
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
    bundleActiveCore_->ShutDown();
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    bundleActiveCore_->OnUserRemoved(100);
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int64_t timeStamp = 20000000000000;
    userService->Init(timeStamp);
    bundleActiveCore_->userStatServices_[userId] = userService;
    bundleActiveCore_->CheckTimeChangeAndGetWallTime(userId);
    bundleActiveCore_->OnUserSwitched(userId);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    int32_t appGroup = 10;
    std::string bundleName = "";
    int32_t userId = 100;
    EXPECT_NE(BundleActiveGroupController::GetInstance().QueryAppGroup(appGroup, bundleName, userId), ERR_OK);

    bundleName = "defaultBundleName";
    EXPECT_NE(BundleActiveGroupController::GetInstance().QueryAppGroup(appGroup, bundleName, userId), ERR_OK);
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
    int64_t bootBasedTimeStamp = 20000000000000;
    BundleActiveEvent event;
    int32_t userId = 100;
    BundleActiveGroupController::GetInstance().bundleGroupEnable_ = false;
    BundleActiveGroupController::GetInstance().ReportEvent(event, bootBasedTimeStamp, userId);

    BundleActiveGroupController::GetInstance().bundleGroupEnable_ = true;
    event.bundleName_ = "com.ohos.camera";
    BundleActiveGroupController::GetInstance().ReportEvent(event, bootBasedTimeStamp, userId);

    event.eventId_ = BundleActiveEvent::NOTIFICATION_SEEN;
    BundleActiveGroupController::GetInstance().ReportEvent(event, bootBasedTimeStamp, userId);

    event.eventId_ = BundleActiveEvent::SYSTEM_INTERACTIVE;
    BundleActiveGroupController::GetInstance().ReportEvent(event, bootBasedTimeStamp, userId);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    std::string bundleName = "com.ohos.camera";
    int32_t userId = 100;
    int64_t bootBasedTimeStamp = 20000000000000;
    int32_t newGroup = 30;
    uint32_t reason = GROUP_CONTROL_REASON_TIMEOUT;
    bool isFlush = false;

    int32_t appGroup = 0;
    BundleActiveGroupController::GetInstance().QueryAppGroup(appGroup, bundleName, userId);

    BundleActiveGroupController::GetInstance().SetAppGroup(bundleName, userId, newGroup, reason,
        bootBasedTimeStamp, isFlush);
    BundleActiveGroupController::GetInstance().CheckAndUpdateGroup(bundleName, 0, userId, bootBasedTimeStamp);

    newGroup = 20;
    reason = GROUP_CONTROL_REASON_CALCULATED;
    BundleActiveGroupController::GetInstance().SetAppGroup(bundleName, userId, newGroup, reason,
        bootBasedTimeStamp, isFlush);
    BundleActiveGroupController::GetInstance().CheckAndUpdateGroup(bundleName, 0, userId, bootBasedTimeStamp);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    int32_t eventId = BundleActiveEvent::ABILITY_FOREGROUND;
    bundleActiveCore_->PreservePowerStateInfo(eventId);
    EXPECT_NE(bundleActiveCore_, nullptr);
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
    auto continuousTaskCallbackInfo = std::make_shared<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>();
    continuousTaskCallbackInfo->creatorUid_ = 20000000;
    bgtaskObserver->OnContinuousTaskStart(continuousTaskCallbackInfo);
    bgtaskObserver->OnContinuousTaskStop(continuousTaskCallbackInfo);
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
    database->RemoveFormData(userId, bundleName, moduleName, formName, 0, 0, 0);
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
    std::vector<NativeRdb::ValuesBucket> formValueBuckets;
    database->InitUsageGroupDatabase(0, true);
    BundleActiveFormRecord formRecord;
    auto rdbStore = database->GetBundleActiveRdbStore(0);
    database->UpdateFormData(userId, bundleName, moduleName, formRecord, rdbStore, formValueBuckets);
    EXPECT_NE(database, nullptr);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_DeleteUninstalledBundleStats_001
 * @tc.desc: onstart
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_DeleteUninstalledBundleStats_001,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int appIndex = 1;
    std::vector<std::shared_ptr<BundleActivePeriodStats>> curStats;
    std::shared_ptr<BundleActivePeriodStats> stats = std::make_shared<BundleActivePeriodStats>();
    stats->bundleStats_["test0"] = std::make_shared<BundleActivePackageStats>();
    stats->bundleStats_["test1"] = std::make_shared<BundleActivePackageStats>();
    stats->bundleStats_["test2"] = std::make_shared<BundleActivePackageStats>();
    curStats.push_back(stats);
    userService->currentStats_ = curStats;
    userService->DeleteUninstalledBundleStats("test", 0, appIndex);
    appIndex = 0;
    userService->DeleteUninstalledBundleStats("test", 0, appIndex);
    userService->OnUserRemoved();
    userService->NotifyNewUpdate();
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_DeleteUninstalledBundleStats_002
 * @tc.desc: onstart
 * @tc.type: FUNC
 * @tc.require: issuesI9Q9ZJ
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_DeleteUninstalledBundleStats_002,
    Function | MediumTest | Level0)
{
    int userId = 100;
    auto userService = std::make_shared<BundleActiveUserService>(userId, *(bundleActiveCore_.get()), false);
    int appIndex = 1;
    std::vector<std::shared_ptr<BundleActivePeriodStats>> curStats;
    std::shared_ptr<BundleActivePeriodStats> stats = std::make_shared<BundleActivePeriodStats>();
    stats->bundleStats_["test0"] = std::make_shared<BundleActivePackageStats>();
    stats->bundleStats_["test1"] = std::make_shared<BundleActivePackageStats>();
    stats->bundleStats_["test2"] = std::make_shared<BundleActivePackageStats>();

    BundleActiveEvent event;
    event.bundleName_ = "test";
    event.uid_ = 0;
    stats->events_.Insert(event);
    BundleActiveEvent event2;
    event.bundleName_ = "test";
    event.uid_ = 1;
    stats->events_.Insert(event);
    curStats.push_back(stats);
    userService->moduleRecords_["test0"] = std::make_shared<BundleActiveModuleRecord>();
    userService->moduleRecords_["test1"] = std::make_shared<BundleActiveModuleRecord>();
    userService->moduleRecords_["test2"] = std::make_shared<BundleActiveModuleRecord>();
    userService->currentStats_ = curStats;
    userService->DeleteUninstalledBundleStats("test", 100, appIndex);
    appIndex = 0;
    userService->DeleteUninstalledBundleStats("test", 100, appIndex);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ConfigReader_001
 * @tc.desc: ConfigReader
 * @tc.type: FUNC
 * @tc.require: issuesIBCE1G
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ConfigReader_001,
    Function | MediumTest | Level0)
{
    auto bundleActiveConfigReader = std::make_shared<BundleActiveConfigReader>();
    EXPECT_NE(bundleActiveConfigReader, nullptr);
    bundleActiveConfigReader->LoadConfig();
    EXPECT_NE(bundleActiveConfigReader->GetApplicationUsePeriodicallyConfig().minUseTimes, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ConfigReader_002
 * @tc.desc: ConfigReader
 * @tc.type: FUNC
 * @tc.require: issuesIBCE1G
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ConfigReader_002,
    Function | MediumTest | Level0)
{
    auto bundleActiveConfigReader = std::make_shared<BundleActiveConfigReader>();
    EXPECT_NE(bundleActiveConfigReader, nullptr);
    const char *path = "test";
    bundleActiveConfigReader->LoadApplicationUsePeriodically(path);
    EXPECT_EQ(bundleActiveConfigReader->GetApplicationUsePeriodicallyConfig().minUseTimes, 0);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ConfigReader_003
 * @tc.desc: ConfigReader
 * @tc.type: FUNC
 * @tc.require: issuesIBCE1G
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ConfigReader_003,
    Function | MediumTest | Level0)
{
    auto bundleActiveConfigReader = std::make_shared<BundleActiveConfigReader>();
    EXPECT_NE(bundleActiveConfigReader, nullptr);
    const char *path = "test";
    Json::Value root;
    bool result = bundleActiveConfigReader->GetJsonFromFile(path, root);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_ConfigReader_004
 * @tc.desc: ConfigReader
 * @tc.type: FUNC
 * @tc.require: issuesIBCE1G
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_ConfigReader_004,
    Function | MediumTest | Level0)
{
    auto bundleActiveConfigReader = std::make_shared<BundleActiveConfigReader>();
    EXPECT_NE(bundleActiveConfigReader, nullptr);
    std::string partialPath = "test";
    std::string fullPath = "";
    bool result = bundleActiveConfigReader->ConvertFullPath(partialPath, fullPath);
    EXPECT_EQ(result, false);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_MergePackageStats_001
 * @tc.desc: MergePackageStats
 * @tc.type: FUNC
 * @tc.require: IC0GWV
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_MergePackageStats_001,
    Function | MediumTest | Level0)
{
    std::vector<BundleActivePackageStats> bundleActivePackageStatsVector;
    BundleActivePackageStats bundleActivePackageStats;
    bundleActivePackageStats.bundleName_ = "test";
    bundleActivePackageStats.uid_ = 1;
    bundleActivePackageStatsVector.push_back(bundleActivePackageStats);
    BundleActivePackageStats bundleActivePackageStats2;
    bundleActivePackageStats2.bundleName_ = "test";
    bundleActivePackageStats2.uid_ = 1;
    bundleActivePackageStatsVector.push_back(bundleActivePackageStats2);
    BundleActivePackageStats bundleActivePackageStats3;
    bundleActivePackageStats3.bundleName_ = "test";
    bundleActivePackageStats3.uid_ = 2;
    bundleActivePackageStatsVector.push_back(bundleActivePackageStats3);
    auto bundleActiveService = std::make_shared<BundleActiveService>();
    auto MergeResult = bundleActiveService->MergePackageStats(bundleActivePackageStatsVector);
    EXPECT_EQ(MergeResult.size(), 2);
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_IsUsedOverOneWeek_001
 * @tc.desc: IsUsedOverOneWeek
 * @tc.type: FUNC
 * @tc.require: issuesICCZ27
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_IsUsedOverOneWeek_001,
    Function | MediumTest | Level0)
{
    std::string bundleName = "test";
    int32_t userId = 100;
    BundleActiveGroupController::GetInstance().bundleUserHistory_  = nullptr;
    EXPECT_FALSE(BundleActiveGroupController::GetInstance().IsUsedOverOneWeek(bundleName, userId));
    BundleActiveGroupController::GetInstance().CreateUserHistory(0, bundleActiveCore_);
    EXPECT_FALSE(BundleActiveGroupController::GetInstance().IsUsedOverOneWeek(bundleName, userId));
    auto userHistory = BundleActiveGroupController::GetInstance().bundleUserHistory_->GetUserHistory(userId, true);
    EXPECT_NE(userHistory, nullptr);
    std::shared_ptr<BundleActivePackageHistory> usageHistoryInserted =
        std::make_shared<BundleActivePackageHistory>();
    usageHistoryInserted->bundleName_ = bundleName;
    
    (*userHistory)[bundleName] = usageHistoryInserted;
    EXPECT_FALSE(BundleActiveGroupController::GetInstance().IsUsedOverOneWeek(bundleName, userId));
    int64_t curTime = BundleActiveUtil::GetSystemTimeMs();
    int64_t testTime = 100000;
    usageHistoryInserted->bundlefirstUseTimeStamp_ = curTime - (ONE_WEEK_TIME - testTime);
    (*userHistory)[bundleName] = usageHistoryInserted;
    EXPECT_TRUE(BundleActiveGroupController::GetInstance().IsUsedOverOneWeek(bundleName, userId));
}

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetFirstUseTime_001
 * @tc.desc: GetFirstUseTime
 * @tc.type: FUNC
 * @tc.require: issuesICCZ27
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetFirstUseTime_001,
    Function | MediumTest | Level0)
{
    int64_t bootBasedTimeStamp = 2000;
    auto bundleUserHistory = std::make_shared<BundleActiveUserHistory>(bootBasedTimeStamp, bundleActiveCore_);
    std::string bundleName = "com.ohos.camera";
    int32_t userId = 100;
    EXPECT_EQ(bundleUserHistory->GetFirstUseTime(bundleName, userId), MAX_END_TIME);
    auto userHistoryMap = bundleUserHistory->GetUserHistory(userId, true);
    (*userHistoryMap)[bundleName] = nullptr;
    EXPECT_EQ(bundleUserHistory->GetFirstUseTime(bundleName, userId), MAX_END_TIME);
    (*userHistoryMap)[bundleName] = std::make_shared<BundleActivePackageHistory>();
    EXPECT_EQ(bundleUserHistory->GetFirstUseTime(bundleName, userId), MAX_END_TIME);
    (*userHistoryMap)[bundleName]->bundlefirstUseTimeStamp_ = 100;
    EXPECT_EQ(bundleUserHistory->GetFirstUseTime(bundleName, userId), 100);
}

}  // namespace DeviceUsageStats
}  // namespace OHOS
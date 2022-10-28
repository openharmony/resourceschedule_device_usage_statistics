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

/*
 * @tc.name: DeviceUsageStatisticsServiceTest_GetServiceObject_001
 * @tc.desc: get service object
 * @tc.type: FUNC
 * @tc.require: issuesI5SOZY
 */
HWTEST_F(DeviceUsageStatisticsServiceTest, DeviceUsageStatisticsServiceTest_GetServiceObject_001, Function | MediumTest | Level0)
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
    auto service = std::make_shared<BundleActiveService>();
    BUNDLE_ACTIVE_LOGI("DeviceUsageStatisticsServiceTest create BundleActiveService!");

    std::vector<std::string> dumpOption{"-A", "Events"};
    std::vector<std::string> dumpInfo;
    service->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "PackageUsage"};
    service->ShellDump(dumpOption, dumpInfo);

    dumpOption.clear();
    dumpInfo.clear();
    dumpOption = {"-A", "ModuleUsage"};
    service->ShellDump(dumpOption, dumpInfo);

    std::vector<std::u16string> args;
    service->Dump(-1, args);

    args.clear();
    args = {to_utf16("-h")};
    service->Dump(-1, args);

    args.clear();
    args = {to_utf16("-A")};
    service->Dump(-1, args);

    args.clear();
    args = {to_utf16("-D")};
    service->Dump(-1, args);
    EXPECT_TRUE(service!=nullptr);
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
    auto service = std::make_shared<BundleActiveService>();
    std::vector<BundleActiveModuleRecord> results;
    int32_t maxNum = 0;
    ErrCode errCode = service->QueryModuleUsageRecords(maxNum, results, 100);
    EXPECT_NE(errCode, 0);

    maxNum = 1001;
    errCode = service->QueryModuleUsageRecords(maxNum, results, 100);
    EXPECT_NE(errCode, 0);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
/*
 * Copyright (c) 2025  Huawei Device Co., Ltd.
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
#include "bundle_active_event_reporter.h"
#include "file_ex.h"

using namespace testing::ext;

namespace OHOS {
namespace DeviceUsageStats {

class BundleActiveEventReporterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void BundleActiveEventReporterTest::SetUpTestCase(void)
{
}

void BundleActiveEventReporterTest::TearDownTestCase(void)
{
}

void BundleActiveEventReporterTest::SetUp(void)
{
}

void BundleActiveEventReporterTest::TearDown(void)
{
}

/*
 * @tc.name: ReportFileSizeEvent_001
 * @tc.desc: test the interface of bundle_active_event_reporter
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveEventReporterTest, ReportFileSizeEvent_001, Function | MediumTest | Level0)
{
    BundleActiveEventReporter::GetInstance().isTaskSubmit_ = false;
    BundleActiveEventReporter::GetInstance().ReportFileSizeEvent();
    BundleActiveEventReporter::GetInstance().ReportFileSizeEvent();
    EXPECT_TRUE(BundleActiveEventReporter::GetInstance().isTaskSubmit_);
}

/*
 * @tc.name: ReportFileSizeDaily_001
 * @tc.desc: test the interface of bundle_active_event_reporter
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveEventReporterTest, ReportFileSizeDaily_001, Function | MediumTest | Level0)
{
    BundleActiveEventReporter::GetInstance().fileSizeRecorderName_ = "test";
    BundleActiveEventReporter::GetInstance().ReportFileSizeDaily();
    BundleActiveEventReporter::GetInstance().fileSizeRecorderName_ =
        "/data/service/el1/public/bundle_usage/file_size_report_time";
    BundleActiveEventReporter::GetInstance().ReportFileSizeDaily();
    BundleActiveEventReporter::GetInstance().ReportFileSizeDaily();
    SaveStringToFile(BundleActiveEventReporter::GetInstance().fileSizeRecorderName_, std::to_string(-1));
    BundleActiveEventReporter::GetInstance().ReportFileSizeDaily();
    SaveStringToFile(BundleActiveEventReporter::GetInstance().fileSizeRecorderName_, std::to_string(1));
    BundleActiveEventReporter::GetInstance().ReportFileSizeDaily();
    std::string lastReportTime;
    LoadStringFromFile(BundleActiveEventReporter::GetInstance().fileSizeRecorderName_, lastReportTime);
    EXPECT_TRUE(!lastReportTime.empty());
}

/*
 * @tc.name: ReportFileSizeDaily_002
 * @tc.desc: test the interface of bundle_active_event_reporter
 * @tc.type: FUNC
 * @tc.require: issuesIC2FBU
 */
HWTEST_F(BundleActiveEventReporterTest, ReportFileSizeDaily_002, Function | MediumTest | Level0)
{
    BundleActiveEventReporter::GetInstance().ReportFileSizeInner();
    std::string lastReportTime;
    LoadStringFromFile(BundleActiveEventReporter::GetInstance().fileSizeRecorderName_, lastReportTime);
    EXPECT_TRUE(!lastReportTime.empty());
}
}
}
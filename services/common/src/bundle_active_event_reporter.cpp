/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "bundle_active_event_reporter.h"
#include "file_ex.h"
#include "hisysevent.h"
#include "bundle_active_log.h"
#include "bundle_active_util.h"
#include "bundle_active_constant.h"


namespace OHOS {
namespace DeviceUsageStats {
namespace {
static const int64_t FIRST_REPORT_TIME = 10 * 1000 * 1000;
static const int64_t ONE_DAY_MICRO_SECOND = static_cast<int64_t>(24) * 60 * 60 * 1000 * 1000;
static const char* DATA_FILE_PATH = "/data";
static const char* FILE_SIZE_REPORTER_RECORDER = "/data/service/el1/public/bundle_usage/file_size_report_time";
}

IMPLEMENT_SINGLE_INSTANCE(BundleActiveEventReporter);

void BundleActiveEventReporter::ReportFileSizeEvent()
{
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    if (!isTaskSubmit_) {
        fileSizeRecorderName_ = std::string(FILE_SIZE_REPORTER_RECORDER);
        SubmitDelayTask(FIRST_REPORT_TIME);
        isTaskSubmit_ = true;
    }
}

void BundleActiveEventReporter::SubmitDelayTask(int64_t delayTime)
{
// LCOV_EXCL_START
    ffrt::submit([]() {
        BundleActiveEventReporter::GetInstance().ReportFileSizeDaily();
        }, ffrt::task_attr().delay(delayTime));
// LCOV_EXCL_STOP
}

// LCOV_EXCL_START
void BundleActiveEventReporter::ReportFileSizeDaily()
{
    std::string lastReportTime;
    LoadStringFromFile(fileSizeRecorderName_, lastReportTime);
    if (lastReportTime.empty()) {
        ReportFileSizeInner();
    } else {
        int64_t lastReportTimeValue = BundleActiveUtil::StringToInt64(lastReportTime);
        if (lastReportTimeValue <= 0) {
            ReportFileSizeInner();
        } else {
            int64_t nowTime = BundleActiveUtil::GetSteadyTime();
            if (nowTime - lastReportTimeValue < ONE_DAY_MICRO_SECOND) {
                int64_t nextReportTime = ONE_DAY_MICRO_SECOND - (nowTime - lastReportTimeValue);
                SubmitDelayTask(nextReportTime);
            } else {
                ReportFileSizeInner();
            }
        }
    }
}

void BundleActiveEventReporter::ReportFileSizeInner()
{
    BUNDLE_ACTIVE_LOGI("ReportFileSizeInner call");
    std::lock_guard<ffrt::mutex> autoLock(mutex_);
    std::vector<std::string> filesPath;
    std::vector<uint64_t> filesPathSize;
    filesPath.emplace_back(BUNDLE_ACTIVE_DATABASE_DIR);
    filesPathSize.emplace_back(BundleActiveUtil::GetFolderOrFileSize(BUNDLE_ACTIVE_DATABASE_DIR));
    double remainSize = BundleActiveUtil::GetDeviceValidSize(DATA_FILE_PATH);
    HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::FILEMANAGEMENT, "USER_DATA_SIZE",
        HiviewDFX::HiSysEvent::EventType::STATISTIC,
        "COMPONENT_NAME", "device_usage_statistics",
        "PARTITION_NAME", DATA_FILE_PATH,
        "REMAIN_PARTITION_SIZE", remainSize,
        "FILE_OR_FOLDER_PATH", filesPath,
        "FILE_OR_FOLDER_SIZE", filesPathSize);
    int64_t nowMicroTime = BundleActiveUtil::GetNowMicroTime();
    SaveStringToFile(fileSizeRecorderName_, std::to_string(nowMicroTime));
    SubmitDelayTask(ONE_DAY_MICRO_SECOND);
}
// LCOV_EXCL_STOP
} // namespace ResourceSchedule
} // namespace OHOS
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_ACTIVE_USER_HISTORY_H
#define BUNDLE_ACTIVE_USER_HISTORY_H

#include <map>
#include <memory>
#include <string>

#include "bundle_active_package_history.h"
#include "bundle_active_group_common.h"
#include "bundle_active_usage_database.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace DeviceUsageStatsGroupConst;

class BundleActiveUserHistory {
public:
    std::map<int, std::shared_ptr<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>>> userHistory_;
    int64_t bootBasedTimeStamp_;
    int64_t bootBasedDuration_;
    int64_t screenOnTimeStamp_;
    int64_t ScreenOnDuration_;
    BundleActiveUsageDatabase database_;
    BundleActiveUserHistory(const int64_t bootBasedTimeStamp);
    std::shared_ptr<BundleActivePackageHistory> GetUsageHistoryForBundle(const std::string& bundleName,
        const int userId, const int64_t bootBasedTimeStamp, const bool& create);
    std::shared_ptr<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>> GetUserHistory(
            const int userId, const bool& create);
    std::shared_ptr<BundleActivePackageHistory> GetUsageHistoryInUserHistory(std::shared_ptr<std::map<std::string,
        std::shared_ptr<BundleActivePackageHistory>>> oneUserHistory, std::string bundleName,
        int64_t bootBasedTimeStamp, bool create);
    int64_t GetBootBasedTimeStamp(int64_t bootBasedTimeStamp);
    int64_t GetScreenOnTimeStamp(int64_t bootBasedTimeStamp);
    void ReportUsage(std::shared_ptr<BundleActivePackageHistory> oneBundleUsageHistory, const std::string& bundleName,
        const int newGroup, const uint32_t groupReason, const int64_t bootBasedTimeStamp,
        const int64_t timeUntilNextCheck);
    void SetBundleGroup(const std::string& bundleName, const int userId, const int64_t bootBasedTimeStamp,
        int newGroup, uint32_t groupReason, const bool& resetTimeout);
    int GetLevelIndex(const std::string& bundleName, const int userId, const int64_t bootBasedTimeStamp,
        const int64_t screenTimeLevel[4], const int64_t bootFromTimeLevel[4]);
    void WriteDeviceDuration();
    void WriteBundleUsage(const int userId);
    void printdata(int userId);
    void UpdateBootBasedAndScreenTime(const bool& isScreenOn, const int64_t bootBasedTimeStamp,
        const bool& isShutdown = false);
private:
    bool isScreenOn_;
};
}
}
#endif
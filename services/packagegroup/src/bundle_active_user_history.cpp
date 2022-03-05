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

#include "bundle_active_user_history.h"
#include "bundle_active_group_common.h"
#include "bundle_active_constant.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace DeviceUsageStatsGroupConst;
using namespace std;

void BundleActiveUserHistory::WriteDeviceDuration()
{
    database_.PutDurationData(bootBasedDuration_, ScreenOnDuration_);
}

void BundleActiveUserHistory::WriteBundleUsage(const int userId)
{
    BUNDLE_ACTIVE_LOGI("WriteBundleUsage called");
    auto userHistory = GetUserHistory(userId, false);
    if (userHistory == nullptr) {
        BUNDLE_ACTIVE_LOGI("WriteBundleUsage called, no existed user history, return");
        return;
    }
    database_.PutBundleHistoryData(userId, userHistory);
}

BundleActiveUserHistory::BundleActiveUserHistory(const int64_t bootBasedTimeStamp)
{
    bootBasedTimeStamp_ = bootBasedTimeStamp;
    screenOnTimeStamp_ = bootBasedTimeStamp;
    database_.InitUsageGroupInfo(APP_GROUP_DATABASE_INDEX);
    auto bootAndScreenOnDuraton = database_.GetDurationData();
    bootBasedDuration_ = bootAndScreenOnDuraton.first;
    ScreenOnDuration_ = bootAndScreenOnDuraton.second;
    isScreenOn_ = false;
}

int BundleActiveUserHistory::GetLevelIndex(const string& bundleName, const int userId,
    const int64_t bootBasedTimeStamp, const int64_t screenTimeLevel[4], const int64_t bootFromTimeLevel[4])
{
    auto oneUserHistory = GetUserHistory(userId, false);
    if (oneUserHistory == nullptr) {
        return -1;
    }
    auto oneBundleHistory = GetUsageHistoryInUserHistory(oneUserHistory, bundleName, bootBasedTimeStamp, false);
    if (oneBundleHistory == nullptr) {
        return -1;
    }
    int64_t screenDiff = GetScreenOnTimeStamp(bootBasedTimeStamp) - oneBundleHistory->lastScreenUsedTimeStamp_;
    int64_t bootFromDiff = GetBootBasedTimeStamp(bootBasedTimeStamp) - oneBundleHistory->lastBootFromUsedTimeStamp_;
    BUNDLE_ACTIVE_LOGI("screendiff is %{public}lld, bootfromdiff is %{public}lld, bundle name is %{public}s",
        screenDiff, bootFromDiff, bundleName.c_str());
    for (int i = 3; i >= 0; i--) {
        if (screenDiff >= screenTimeLevel[i] && bootFromDiff >= bootFromTimeLevel[i]) {
            return i;
        }
    }
    return -1;
}

int64_t BundleActiveUserHistory::GetBootBasedTimeStamp(int64_t bootBasedTimeStamp)
{
    return bootBasedTimeStamp - bootBasedTimeStamp_ + bootBasedDuration_;
}

int64_t BundleActiveUserHistory::GetScreenOnTimeStamp(int64_t bootBasedTimeStamp)
{
    int64_t result = ScreenOnDuration_;
    if (isScreenOn_) {
        result += bootBasedTimeStamp - screenOnTimeStamp_;
    }
    return result;
}

shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> BundleActiveUserHistory::GetUserHistory(
    const int userId, const bool& create)
{
    auto it = userHistory_.find(userId);
    if (it == userHistory_.end() && create) {
        shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> usageHistoryInserted =
            database_.GetBundleHistoryData(userId);
        if (usageHistoryInserted == nullptr) {
            BUNDLE_ACTIVE_LOGI("GetUserHistory READ FROM DATABASE FAILD");
            usageHistoryInserted =
                make_shared<map<string, shared_ptr<BundleActivePackageHistory>>>();
        }
        BUNDLE_ACTIVE_LOGI("GetUserHistory usageHistoryInserted not null");
        userHistory_[userId] = usageHistoryInserted;
    }
    return userHistory_[userId];
}

shared_ptr<BundleActivePackageHistory> BundleActiveUserHistory::GetUsageHistoryInUserHistory(
    shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> oneUserHistory,
    string bundleName, int64_t bootBasedTimeStamp, bool create)
{
    if (oneUserHistory == nullptr) {
        return nullptr;
    }
    auto it = oneUserHistory->find(bundleName);
    if (it == oneUserHistory->end() && create) {
        shared_ptr<BundleActivePackageHistory> usageHistoryInserted =
            make_shared<BundleActivePackageHistory>();
        usageHistoryInserted->lastBootFromUsedTimeStamp_ = GetBootBasedTimeStamp(bootBasedTimeStamp);
        usageHistoryInserted->lastScreenUsedTimeStamp_ = GetScreenOnTimeStamp(bootBasedTimeStamp);
        usageHistoryInserted->currentGroup_ = ACTIVE_GROUP_NEVER;
        usageHistoryInserted->reasonInGroup_ = GROUP_CONTROL_REASON_DEFAULT;
        usageHistoryInserted->bundleAliveTimeoutTimeStamp_ = 0;
        usageHistoryInserted->bundleDailyTimeoutTimeStamp_ = 0;
        (*oneUserHistory)[bundleName] = usageHistoryInserted;
    }
    return (*oneUserHistory)[bundleName];
}

shared_ptr<BundleActivePackageHistory> BundleActiveUserHistory::GetUsageHistoryForBundle(
    const string& bundleName, const int userId, const int64_t bootBasedTimeStamp, const bool& create)
{
    auto oneUserHistory = GetUserHistory(userId, create);
    if (oneUserHistory == nullptr) {
        return nullptr;
    }
    auto oneBundleHistory = GetUsageHistoryInUserHistory(oneUserHistory, bundleName, bootBasedTimeStamp, create);
    if (oneBundleHistory == nullptr) {
        return nullptr;
    }
    return oneBundleHistory;
}

void BundleActiveUserHistory::ReportUsage(shared_ptr<BundleActivePackageHistory> oneBundleUsageHistory,
    const string& bundleName, const int newGroup, const uint32_t groupReason, const int64_t bootBasedTimeStamp,
    const int64_t timeUntilNextCheck)
{
    if (timeUntilNextCheck > bootBasedTimeStamp) {
        int64_t nextCheckTimeStamp = bootBasedDuration_ + (timeUntilNextCheck - bootBasedTimeStamp_);
        if (newGroup == ACTIVE_GROUP_ALIVE) {
            oneBundleUsageHistory->bundleAliveTimeoutTimeStamp_ = max(nextCheckTimeStamp,
                oneBundleUsageHistory->bundleAliveTimeoutTimeStamp_);
        } else if (newGroup == ACTIVE_GROUP_DAILY) {
            oneBundleUsageHistory->bundleDailyTimeoutTimeStamp_ = max(nextCheckTimeStamp,
                oneBundleUsageHistory->bundleDailyTimeoutTimeStamp_);
        } else {
            return;
        }
    }
    if (bootBasedTimeStamp > 0) {
        oneBundleUsageHistory->lastBootFromUsedTimeStamp_ = bootBasedDuration_ +
            (bootBasedTimeStamp - bootBasedTimeStamp_);
        oneBundleUsageHistory->lastScreenUsedTimeStamp_ = GetScreenOnTimeStamp(bootBasedTimeStamp);
    }
    if (oneBundleUsageHistory->currentGroup_ > newGroup) {
        oneBundleUsageHistory->currentGroup_ = newGroup;
    }
    oneBundleUsageHistory->reasonInGroup_ = GROUP_CONTROL_REASON_USAGE | groupReason;
}

void BundleActiveUserHistory::SetBundleGroup(const string& bundleName, const int userId,
    const int64_t bootBasedTimeStamp, int newGroup, uint32_t groupReason, const bool& resetTimeout)
{
    BUNDLE_ACTIVE_LOGI("set %{public}s to group %{public}d, reason is %{public}d",
        bundleName.c_str(), newGroup, groupReason);
    shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> userBundleHistory =
        GetUserHistory(userId, false);
    if (userBundleHistory == nullptr) {
        return;
    }
    shared_ptr<BundleActivePackageHistory> oneBundleHistory = GetUsageHistoryInUserHistory(
        userBundleHistory, bundleName, bootBasedTimeStamp, false);
    if (oneBundleHistory == nullptr) {
        return;
    }
    oneBundleHistory->currentGroup_ = newGroup;
    oneBundleHistory->reasonInGroup_ = groupReason;
    int64_t setTimeStamp = GetBootBasedTimeStamp(bootBasedTimeStamp);
    if (resetTimeout) {
        oneBundleHistory->bundleAliveTimeoutTimeStamp_ = setTimeStamp;
        oneBundleHistory->bundleDailyTimeoutTimeStamp_ = setTimeStamp;
    }
}

void BundleActiveUserHistory::UpdateBootBasedAndScreenTime(const bool& isScreenOn, const int64_t bootBasedTimeStamp,
    const bool& isShutdown)
{
    if (isScreenOn_ == isScreenOn && isShutdown == false) {
        return;
    }
    isScreenOn_ = isScreenOn;
    if (isScreenOn_) {
        screenOnTimeStamp_ = bootBasedTimeStamp;
    } else {
        ScreenOnDuration_ += bootBasedTimeStamp - screenOnTimeStamp_;
        bootBasedDuration_ += bootBasedTimeStamp - bootBasedTimeStamp_;
        bootBasedTimeStamp_ = bootBasedTimeStamp;
    }
    database_.PutDurationData(bootBasedDuration_, ScreenOnDuration_);
}

void BundleActiveUserHistory::printdata(int userId)
{
    auto oneUserHistory = GetUserHistory(userId, false);
    BUNDLE_ACTIVE_LOGI("printdata screen is %{public}d", isScreenOn_);
    if (oneUserHistory == nullptr) {
        return;
    }
    for (auto oneBundleUsage : (*oneUserHistory)) {
        BUNDLE_ACTIVE_LOGI("bundle name is %{public}s, lastBootFromUsedTimeStamp_ is %{public}lld, "
            "lastScreenUsedTimeStamp_ is %{public}lld, currentGroup_ is %{public}d, reasonInGroup_ is %{public}d, "
            "daily time out %{public}lld, alive time out %{public}lld", oneBundleUsage.first.c_str(),
            oneBundleUsage.second->lastBootFromUsedTimeStamp_, oneBundleUsage.second->lastScreenUsedTimeStamp_,
            oneBundleUsage.second->currentGroup_, oneBundleUsage.second->reasonInGroup_,
            oneBundleUsage.second->bundleDailyTimeoutTimeStamp_, oneBundleUsage.second->bundleAliveTimeoutTimeStamp_);
    }
}
}
}
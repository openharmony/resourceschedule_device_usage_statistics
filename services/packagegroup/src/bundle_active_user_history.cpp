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
#include "bundle_active_core.h"
#include "app_group_callback_info.h"
#include "bundle_active_user_history.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace DeviceUsageStatsGroupConst;
using namespace std;

BundleActivePackageHistory::BundleActivePackageHistory()
{
    lastBootFromUsedTimeStamp_ = 0;
    lastScreenUsedTimeStamp_ = 0;
    lastGroupCalculatedTimeStamp_ = 0;
    currentGroup_ = DeviceUsageStatsGroupConst::ACTIVE_GROUP_NEVER;
    reasonInGroup_ = DeviceUsageStatsGroupConst::GROUP_CONTROL_REASON_DEFAULT;
    bundleAliveTimeoutTimeStamp_ = 0;
    bundleDailyTimeoutTimeStamp_ = 0;
    lastCalculatedGroup_ = ACTIVE_GROUP_NEVER;
    isChanged_ = false;
};

void BundleActiveUserHistory::WriteDeviceDuration()
{
    database_.PutDurationData(bootBasedDuration_, ScreenOnDuration_);
}

void BundleActiveUserHistory::WriteBundleUsage(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("WriteBundleUsage called");
    auto userHistory = GetUserHistory(userId, false);
    if (userHistory == nullptr) {
        BUNDLE_ACTIVE_LOGI("WriteBundleUsage called, no existed user history, return");
        return;
    }
    database_.PutBundleHistoryData(userId, userHistory);
}

void BundleActiveUserHistory::OnBundleUninstalled(const int32_t userId, const std::string bundleName)
{
    database_.OnPackageUninstalled(userId, bundleName);
}

BundleActiveUserHistory::BundleActiveUserHistory(const int64_t bootBasedTimeStamp,
    const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    bootBasedTimeStamp_ = bootBasedTimeStamp;
    screenOnTimeStamp_ = bootBasedTimeStamp;
    database_.InitUsageGroupDatabase(APP_GROUP_DATABASE_INDEX, false);
    auto bootAndScreenOnDuraton = database_.GetDurationData();
    bootBasedDuration_ = bootAndScreenOnDuraton.first;
    ScreenOnDuration_ = bootAndScreenOnDuraton.second;
    isScreenOn_ = false;
    if (bundleActiveCore) {
        bundleActiveCore_ = bundleActiveCore;
    }
}

int32_t BundleActiveUserHistory::GetLevelIndex(const string& bundleName, const int32_t userId,
    const int64_t bootBasedTimeStamp, const std::vector<int64_t> screenTimeLevel,
    const std::vector<int64_t> bootFromTimeLevel)
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
    BUNDLE_ACTIVE_LOGI("screendiff is %{public}lld, bootfromdiff is %{public}lld, bundle name is %{public}s,"
        "userid is %{public}d",
        (long long)screenDiff, (long long)bootFromDiff, bundleName.c_str(), userId);
    for (int32_t i = 3; i >= 0; i--) {
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
    const int32_t userId, const bool create)
{
    auto it = userHistory_.find(userId);
    if ((it == userHistory_.end()) && create) {
        shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> usageHistoryInserted =
            database_.GetBundleHistoryData(userId);
        if (!usageHistoryInserted) {
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
    string bundleName, int64_t bootBasedTimeStamp, const bool create)
{
    if (!oneUserHistory) {
        return nullptr;
    }
    auto it = oneUserHistory->find(bundleName);
    if ((it == oneUserHistory->end()) && create) {
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
    const string& bundleName, const int32_t userId, const int64_t bootBasedTimeStamp, const bool create)
{
    auto oneUserHistory = GetUserHistory(userId, create);
    if (!oneUserHistory) {
        return nullptr;
    }
    auto oneBundleHistory = GetUsageHistoryInUserHistory(oneUserHistory, bundleName, bootBasedTimeStamp, create);
    if (!oneBundleHistory) {
        return nullptr;
    }
    return oneBundleHistory;
}

void BundleActiveUserHistory::ReportUsage(shared_ptr<BundleActivePackageHistory> oneBundleUsageHistory,
    const string& bundleName, const int32_t newGroup, const uint32_t groupReason, const int64_t bootBasedTimeStamp,
    const int64_t timeUntilNextCheck, const int32_t userId)
{
    if ((oneBundleUsageHistory->reasonInGroup_ & GROUP_CONTROL_REASON_MASK) == GROUP_CONTROL_REASON_FORCED) {
        return;
    }
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
    int32_t oldGroup = oneBundleUsageHistory->currentGroup_;
    if (oneBundleUsageHistory->currentGroup_ > newGroup) {
        oneBundleUsageHistory->currentGroup_ = newGroup;
    }
    oneBundleUsageHistory->reasonInGroup_ = GROUP_CONTROL_REASON_USAGE | groupReason;
    oneBundleUsageHistory->isChanged_ = true;
    BUNDLE_ACTIVE_LOGI("RegisterAppGroupCallBack will ReportUsage");
    bool isGroupChanged = (oldGroup == newGroup) ? true : false;
    if (!isGroupChanged) {
        AppGroupCallbackInfo callbackInfo(
            userId, oldGroup, newGroup, oneBundleUsageHistory->reasonInGroup_, bundleName);
        BUNDLE_ACTIVE_LOGI("RegisterAppGroupCallBack AppGroupCallbackInfo build success");
        if (!bundleActiveCore_.expired()) {
            BUNDLE_ACTIVE_LOGI("RegisterAppGroupCallBack will callback!");
            bundleActiveCore_.lock()->OnAppGroupChanged(callbackInfo);
        }
    }
}

int32_t BundleActiveUserHistory::SetAppGroup(const string& bundleName, const int32_t userId,
    const int64_t bootBasedTimeStamp, int32_t newGroup, uint32_t groupReason, const bool isFlush)
{
    std::lock_guard<std::mutex> lock(setGroupMutex_);
    BUNDLE_ACTIVE_LOGI("set %{public}s to group %{public}d, reason is %{public}d, userId is %{public}d",
        bundleName.c_str(), newGroup, groupReason, userId);
    shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> userBundleHistory = GetUserHistory(userId, false);
    if (!userBundleHistory) {
        return ERR_GET_BUNDLE_USED_HISTORY_FAILED;
    }
    shared_ptr<BundleActivePackageHistory> oneBundleHistory = GetUsageHistoryInUserHistory(userBundleHistory,
        bundleName, bootBasedTimeStamp, false);
    if (!oneBundleHistory) {
        return ERR_GET_BUNDLE_USED_HISTORY_FAILED;
    }
    if (oneBundleHistory->currentGroup_ == newGroup && oneBundleHistory->reasonInGroup_ == groupReason) {
        BUNDLE_ACTIVE_LOGI("%{public}s group and reason is same as before, not update", bundleName.c_str());
        return ERR_REPEAT_SET_APP_GROUP;
    }
    int32_t oldGroup = oneBundleHistory->currentGroup_;
    oneBundleHistory->currentGroup_ = newGroup;
    oneBundleHistory->reasonInGroup_ = groupReason;
    oneBundleHistory->isChanged_ = true;
    BUNDLE_ACTIVE_LOGI("SetAppGroup set success");
    if (isFlush) {
        WriteBundleUsage(userId);
    }

    bool isGroupChanged = (oldGroup == newGroup) ? true : false;
    if (!isGroupChanged) {
        AppGroupCallbackInfo callbackInfo(
            userId, oldGroup, newGroup, oneBundleHistory->reasonInGroup_, bundleName);
        if (!bundleActiveCore_.expired()) {
            bundleActiveCore_.lock()->OnAppGroupChanged(callbackInfo);
        }
    }
    return ERR_OK;
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

void BundleActiveUserHistory::PrintData(int32_t userId)
{
    auto oneUserHistory = GetUserHistory(userId, false);
    BUNDLE_ACTIVE_LOGI("PrintData screen is %{public}d", isScreenOn_);
    if (oneUserHistory == nullptr) {
        return;
    }
    for (auto oneBundleUsage : (*oneUserHistory)) {
        BUNDLE_ACTIVE_LOGI("bundle name is %{public}s, lastBootFromUsedTimeStamp_ is %{public}lld, "
            "lastScreenUsedTimeStamp_ is %{public}lld, currentGroup_ is %{public}d, reasonInGroup_ is %{public}d, "
            "daily time out %{public}lld, alive time out %{public}lld", oneBundleUsage.first.c_str(),
            (long long)oneBundleUsage.second->lastBootFromUsedTimeStamp_,
            (long long)oneBundleUsage.second->lastScreenUsedTimeStamp_,
            oneBundleUsage.second->currentGroup_, oneBundleUsage.second->reasonInGroup_,
            (long long)oneBundleUsage.second->bundleDailyTimeoutTimeStamp_,
            (long long)oneBundleUsage.second->bundleAliveTimeoutTimeStamp_);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


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

#include "time_service_client.h"

#include "bundle_active_user_history.h"
#include "bundle_active_group_handler.h"
#include "ibundle_active_service.h"
#include "bundle_active_group_controller.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace DeviceUsageStatsGroupConst;
BundleActiveGroupHandlerObject::BundleActiveGroupHandlerObject()
{
        bundleName_ = "";
        userId_ = -1;
}


BundleActiveGroupController::BundleActiveGroupController(const bool debug)
{
    timeoutForDirectlyUse_ = debug ? THREE_MINUTE : ONE_HOUR;
    timeoutForNotifySeen_ = debug ? ONE_MINUTE : TWELVE_HOUR;
    timeoutForSystemInteraction_ = debug ? ONE_MINUTE : TEN_MINUTE;
    screenTimeLevel_ = {0, 0, debug ? TWO_MINUTE : ONE_HOUR, debug ? FOUR_MINUTE : TWO_HOUR};
    bootTimeLevel_ = {0, debug ? TWO_MINUTE : TWELVE_HOUR, debug ? FOUR_MINUTE : TWENTY_FOUR_HOUR,
        debug ? SIXTEEN_MINUTE : FOURTY_EIGHT_HOUR};
    eventIdMatchReason_ = {
        {BundleActiveEvent::ABILITY_FOREGROUND, GROUP_EVENT_REASON_FOREGROUND},
        {BundleActiveEvent::ABILITY_BACKGROUND, GROUP_EVENT_REASON_BACKGROUND},
        {BundleActiveEvent::SYSTEM_INTERACTIVE, GROUP_EVENT_REASON_SYSTEM},
        {BundleActiveEvent::USER_INTERACTIVE, GROUP_EVENT_REASON_USER_INTERACTION},
        {BundleActiveEvent::NOTIFICATION_SEEN, GROUP_EVENT_REASON_NOTIFY_SEEN},
        {BundleActiveEvent::LONG_TIME_TASK_STARTTED, GROUP_EVENT_REASON_LONG_TIME_TASK_STARTTED},
    };
}

void BundleActiveGroupController::RestoreDurationToDatabase()
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->WriteDeviceDuration();
}

void BundleActiveGroupController::RestoreToDatabase(const int32_t userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->WriteBundleUsage(userId);
}

void BundleActiveGroupController::OnUserRemoved(const int32_t userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->userHistory_.erase(userId);
    if (!activeGroupHandler_.expired()) {
        activeGroupHandler_.lock()->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE);
    }
}

void BundleActiveGroupController::OnUserSwitched(const int32_t userId, const int32_t currentUsedUser)
{
    BUNDLE_ACTIVE_LOGI("last time check for user %{public}d", currentUsedUser);
    CheckEachBundleState(currentUsedUser);
    bundleUserHistory_->WriteBundleUsage(currentUsedUser);
    std::lock_guard<std::mutex> lock(mutex_);
    if (!activeGroupHandler_.expired()) {
        activeGroupHandler_.lock()->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE);
        activeGroupHandler_.lock()->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_BUNDLE_STATE);
    }
    PeriodCheckBundleState(userId);
}

void BundleActiveGroupController::OnScreenChanged(const bool& isScreenOn, const int64_t bootFromTimeStamp)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->UpdateBootBasedAndScreenTime(isScreenOn, bootFromTimeStamp);
}

void BundleActiveGroupController::SetHandlerAndCreateUserHistory(
    const std::shared_ptr<BundleActiveGroupHandler>& groupHandler, const int64_t bootFromTimeStamp,
    const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    if (bundleUserHistory_ == nullptr) {
        BUNDLE_ACTIVE_LOGI("SetHandlerAndCreateUserHistory bundleUserHistory_ is null, "
            "called constructor, bootstamp is %{public}lld", (long long)bootFromTimeStamp);
        bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootFromTimeStamp, bundleActiveCore);
    }
    OnScreenChanged(IsScreenOn(), bootFromTimeStamp);
    activeGroupHandler_ = groupHandler;
}

void BundleActiveGroupController::OnBundleUninstalled(const int32_t userId, const std::string bundleName)
{
    std::lock_guard<std::mutex> lock(mutex_);
    BUNDLE_ACTIVE_LOGI("OnBundleUninstalled called, userId is %{public}d, bundlename is %{public}s",
        userId, bundleName.c_str());
    auto oneUserHistory = bundleUserHistory_->GetUserHistory(userId, false);
    if (oneUserHistory == nullptr) {
        return;
    }
    oneUserHistory->erase(bundleName);
    bundleUserHistory_->OnBundleUninstalled(userId, bundleName);
}

bool BundleActiveGroupController::GetBundleMgrProxy()
{
    if (!sptrBundleMgr_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            BUNDLE_ACTIVE_LOGE("Failed to get system ability mgr.");
            return false;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (!remoteObject) {
                BUNDLE_ACTIVE_LOGE("Failed to get bundle manager service.");
                return false;
        }
        sptrBundleMgr_ = iface_cast<IBundleMgr>(remoteObject);
        if (!sptrBundleMgr_) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability, sptrBundleMgr_");
            return false;
        }
        auto object = sptrBundleMgr_->AsObject();
        if (!object) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability");
            return false;
        }
    }
    return true;
}

void BundleActiveGroupController::PeriodCheckBundleState(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("PeriodCheckBundleState called");
    if (!activeGroupHandler_.expired()) {
        BundleActiveGroupHandlerObject tmpGroupHandlerObj;
        tmpGroupHandlerObj.userId_ = userId;
        std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveGroupHandlerObject>(tmpGroupHandlerObj);
        auto handlerEvent = AppExecFwk::InnerEvent::Get(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE,
            handlerobjToPtr);
        activeGroupHandler_.lock()->SendEvent(handlerEvent, FIVE_SECOND);
    }
}

bool BundleActiveGroupController::CheckEachBundleState(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("CheckEachBundleState called, userid is %{public}d", userId);
    std::vector<ApplicationInfo> allBundlesForUser;
    if (!GetBundleMgrProxy()) {
        BUNDLE_ACTIVE_LOGE("CheckEachBundleState get bundle manager proxy failed!");
        return false;
    }
    sptrBundleMgr_->GetApplicationInfos(flag, userId, allBundlesForUser);
    std::vector<std::string> bundleNamesOfUser;
    for (auto oneBundle : allBundlesForUser) {
        bundleNamesOfUser.push_back(oneBundle.bundleName);
    }
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    for (auto oneBundleName : bundleNamesOfUser) {
        CheckAndUpdateGroup(oneBundleName, userId, bootBasedTimeStamp);
    }
    return true;
}

void BundleActiveGroupController::CheckIdleStatsOneTime()
{
    auto handlerEvent = AppExecFwk::InnerEvent::Get(
        BundleActiveGroupHandler::MSG_ONE_TIME_CHECK_BUNDLE_STATE);
    if (!activeGroupHandler_.expired()) {
        activeGroupHandler_.lock()->SendEvent(handlerEvent);
    }
}

int32_t BundleActiveGroupController::GetNewGroup(const std::string& bundleName, const int32_t userId,
    const int64_t bootBasedTimeStamp)
{
    int32_t groupIndex = bundleUserHistory_->GetLevelIndex(bundleName, userId, bootBasedTimeStamp, screenTimeLevel_,
        bootTimeLevel_);
    if (groupIndex < 0) {
        return -1;
    }
    return LEVEL_GROUP[groupIndex];
}

bool BundleActiveGroupController::calculationTimeOut(
    const std::shared_ptr<BundleActivePackageHistory>& oneBundleHistory, const int64_t bootBasedTimeStamp)
{
    if (oneBundleHistory == nullptr) {
        return false;
    }
    int64_t lastGroupCalculatedTimeStamp = oneBundleHistory->lastGroupCalculatedTimeStamp_;
    return lastGroupCalculatedTimeStamp > 0 && bundleUserHistory_->GetBootBasedTimeStamp(bootBasedTimeStamp)
        - lastGroupCalculatedTimeStamp > timeoutCalculated_;
}

void BundleActiveGroupController::ReportEvent(const BundleActiveEvent& event, const int64_t bootBasedTimeStamp,
    const int32_t userId)
{
    if (bundleGroupEnable_ == false) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsBundleInstalled(event.bundleName_, userId) == false) {
        BUNDLE_ACTIVE_LOGE("Report an uninstalled package event, return!");
        return;
    }
    int32_t eventId = event.eventId_;
    auto item  = eventIdMatchReason_.find(eventId);
    if (item != eventIdMatchReason_.end()) {
        std::shared_ptr<BundleActivePackageHistory> bundleUsageHistory = bundleUserHistory_->GetUsageHistoryForBundle(
            event.bundleName_, userId, bootBasedTimeStamp, true);
        if (bundleUsageHistory == nullptr) {
            return;
        }
        int64_t timeUntilNextCheck;
        uint32_t eventReason = item->second;
        switch (eventId) {
            case BundleActiveEvent::NOTIFICATION_SEEN:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_DAILY,
                    eventReason, 0, bootBasedTimeStamp + timeoutForNotifySeen_, userId);
                timeUntilNextCheck = timeoutForNotifySeen_;
                break;
            case BundleActiveEvent::SYSTEM_INTERACTIVE:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_ALIVE,
                    eventReason, 0, bootBasedTimeStamp + timeoutForSystemInteraction_, userId);
                timeUntilNextCheck = timeoutForSystemInteraction_;
                break;
            default:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_ALIVE,
                    eventReason, bootBasedTimeStamp, bootBasedTimeStamp + timeoutForDirectlyUse_, userId);
                timeUntilNextCheck = timeoutForDirectlyUse_;
                break;
        }
        BundleActiveGroupHandlerObject tmpGroupHandlerObj;
        tmpGroupHandlerObj.userId_ = userId;
        tmpGroupHandlerObj.bundleName_ = event.bundleName_;
        std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveGroupHandlerObject>(tmpGroupHandlerObj);
        auto handlerEvent = AppExecFwk::InnerEvent::Get(BundleActiveGroupHandler::MSG_CHECK_BUNDLE_STATE,
            handlerobjToPtr);
        if (!activeGroupHandler_.expired()) {
            activeGroupHandler_.lock()->SendEvent(handlerEvent, timeUntilNextCheck);
        }
    }
}

void BundleActiveGroupController::CheckAndUpdateGroup(const std::string& bundleName, const int32_t userId,
    const int64_t bootBasedTimeStamp)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName, userId, bootBasedTimeStamp, true);
    if (oneBundleHistory == nullptr) {
        return;
    }
    uint32_t groupReason = oneBundleHistory->reasonInGroup_;
    uint32_t oldGroupControlReason = groupReason & GROUP_CONTROL_REASON_MASK;
    if (oldGroupControlReason == GROUP_CONTROL_REASON_FORCED) {
        BUNDLE_ACTIVE_LOGI("%{public}s is forced set, return", bundleName.c_str());
        return;
    }
    int32_t oldGroup = oneBundleHistory->currentGroup_;
    int32_t newGroup = std::max(oldGroup, ACTIVE_GROUP_ALIVE);
    if (oldGroupControlReason == GROUP_CONTROL_REASON_DEFAULT ||
        oldGroupControlReason == GROUP_CONTROL_REASON_USAGE ||
        oldGroupControlReason == GROUP_CONTROL_REASON_TIMEOUT) {
        newGroup = GetNewGroup(bundleName, userId, bootBasedTimeStamp);
        if (newGroup < 0) {
            return;
        }
        groupReason = GROUP_CONTROL_REASON_TIMEOUT;
    }
    int64_t bootBasedTimeStampAdjusted = bundleUserHistory_->GetBootBasedTimeStamp(bootBasedTimeStamp);
    bool notTimeout = false;
    if (newGroup >= ACTIVE_GROUP_ALIVE && oneBundleHistory->bundleAliveTimeoutTimeStamp_ >
        bootBasedTimeStampAdjusted) {
        newGroup = ACTIVE_GROUP_ALIVE;
        groupReason = GROUP_CONTROL_REASON_USAGE | GROUP_EVENT_REASON_ALIVE_NOT_TIMEOUT;
        notTimeout = true;
    } else if (newGroup >= ACTIVE_GROUP_DAILY && oneBundleHistory->bundleDailyTimeoutTimeStamp_ >
        bootBasedTimeStampAdjusted) {
        newGroup = ACTIVE_GROUP_DAILY;
        groupReason = GROUP_CONTROL_REASON_USAGE | GROUP_EVENT_REASON_ALIVE_TIMEOUT;
        notTimeout = true;
    }
    if (oldGroup < newGroup || notTimeout) {
        BUNDLE_ACTIVE_LOGI("CheckAndUpdateGroup called SetAppGroup");
        bundleUserHistory_->SetAppGroup(bundleName, userId, bootBasedTimeStamp, newGroup, groupReason, false);
    }
}

ErrCode BundleActiveGroupController::SetAppGroup(const std::string& bundleName, const int32_t userId,
    int32_t newGroup, uint32_t reason, const int64_t bootBasedTimeStamp, const bool isFlush)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsBundleInstalled(bundleName, userId)) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName, userId, bootBasedTimeStamp, true);
    if (!oneBundleHistory) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    return bundleUserHistory_->SetAppGroup(bundleName, userId, bootBasedTimeStamp, newGroup, reason, isFlush);
}

int32_t BundleActiveGroupController::IsBundleIdle(const std::string& bundleName, const int32_t userId)
{
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    if (IsBundleInstalled(bundleName, userId) == false) {
        return -1;
    }
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(
        bundleName, userId, bootBasedTimeStamp, false);
    if (oneBundleHistory == nullptr) {
        return 1;
    } else if (oneBundleHistory->currentGroup_ >= ACTIVE_GROUP_RARE) {
        BUNDLE_ACTIVE_LOGI("IsBundleIdle, bundle group is %{public}d", oneBundleHistory->currentGroup_);
        return 1;
    } else {
        BUNDLE_ACTIVE_LOGI("IsBundleIdle, bundle group is %{public}d", oneBundleHistory->currentGroup_);
        return 0;
    }
}

ErrCode BundleActiveGroupController::QueryAppGroup(int32_t& appGroup,
    const std::string& bundleName, const int32_t userId)
{
    if (bundleName.empty()) {
        BUNDLE_ACTIVE_LOGE("bundleName can not get by userId");
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    if (!IsBundleInstalled(bundleName, userId)) {
        BUNDLE_ACTIVE_LOGI("QueryAppGroup is not bundleInstalled");
        return ERR_APPLICATION_IS_NOT_INSTALLED;
    }
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(
        bundleName, userId, bootBasedTimeStamp, false);
    if (!oneBundleHistory) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    BUNDLE_ACTIVE_LOGI("QueryAppGroup group is %{public}d", oneBundleHistory->currentGroup_);
    appGroup = oneBundleHistory->currentGroup_;
    return ERR_OK;
}

bool BundleActiveGroupController::IsBundleInstalled(const std::string& bundleName, const int32_t userId)
{
    ApplicationInfo bundleInfo;
    if (!sptrBundleMgr_) {
        return false;
    }
    bool getInfoIsSuccess = sptrBundleMgr_->GetApplicationInfo(bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO,
        userId, bundleInfo);
    if (getInfoIsSuccess == false) {
        BUNDLE_ACTIVE_LOGE("IsBundleInstalled bundle is not installed!");
        return false;
    }
    return true;
}

void BundleActiveGroupController::ShutDown(const int64_t bootBasedTimeStamp, const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("ShutDown called");
    CheckEachBundleState(userId);
    bundleUserHistory_->UpdateBootBasedAndScreenTime(false, bootBasedTimeStamp, true);
}

bool BundleActiveGroupController::IsScreenOn()
{
    bool result = PowerMgrClient::GetInstance().IsScreenOn();
    BUNDLE_ACTIVE_LOGI("IsScreenOn() is %{public}d", result);
    return result;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


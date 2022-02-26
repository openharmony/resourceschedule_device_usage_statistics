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

void BundleActiveGroupController::RestoreDurationToDatabase()
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->WriteDeviceDuration();
}

void BundleActiveGroupController::RestoreToDatabase(const int userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->WriteBundleUsage(userId);
}

void BundleActiveGroupController::OnUserRemoved(const int userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->userHistory_.erase(userId);
}

void BundleActiveGroupController::OnScreenChanged(const bool& isScreenOn, const int64_t bootFromTimeStamp)
{
    std::lock_guard<std::mutex> lock(mutex_);
    bundleUserHistory_->UpdateBootBasedAndScreenTime(isScreenOn, bootFromTimeStamp);
}

void BundleActiveGroupController::SetHandlerAndCreateUserHistory(
    const std::shared_ptr<BundleActiveGroupHandler>& groupHandler, const int64_t bootFromTimeStamp)
{
    if (bundleUserHistory_ == nullptr) {
        BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::SetHandlerAndCreateUserHistory bundleUserHistory_ is null, "
            "called constructor, bootstamp is %{public}lld", bootFromTimeStamp);
        bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootFromTimeStamp);
    }
    OnScreenChanged(IsScreenOn(), bootFromTimeStamp);
    activeGroupHandler_ = groupHandler;
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
        if ((!sptrBundleMgr_) || (!sptrBundleMgr_->AsObject())) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability");
            return false;
        }
    }
    return true;
}

void BundleActiveGroupController::PeriodCheckBundleState(const int userId)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::PeriodCheckBundleState called");
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

bool BundleActiveGroupController::CheckEachBundleState(const int userId)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::CheckEachBundleState called, userid is %{public}d", userId);
    std::vector<ApplicationInfo> allBundlesForUser;
    if (!GetBundleMgrProxy()) {
        BUNDLE_ACTIVE_LOGE("BundleActiveGroupController::CheckEachBundleState get bundle manager proxy failed!");
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
    bundleUserHistory_->printdata(userId);
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

int BundleActiveGroupController::GetNewGroup(const std::string& bundleName, const int userId,
    const int64_t bootBasedTimeStamp)
{
    int groupIndex = bundleUserHistory_->GetLevelIndex(bundleName, userId, bootBasedTimeStamp, SCREEN_TIME_LEVEL,
        BOOT_TIME_LEVEL);
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

int BundleActiveGroupController::EventToGroupReason(const int eventId)
{
    switch (eventId) {
        case BundleActiveEvent::ABILITY_FOREGROUND:
            return GROUP_EVENT_REASON_FOREGROUND;
        case BundleActiveEvent::ABILITY_BACKGROUND:
            return GROUP_EVENT_REASON_BACKGROUND;
        case BundleActiveEvent::SYSTEM_INTERACTIVE:
            return GROUP_EVENT_REASON_SYSTEM;
        case BundleActiveEvent::USER_INTERACTIVE:
            return GROUP_EVENT_REASON_USER_INTERACTION;
        case BundleActiveEvent::NOTIFICATION_SEEN:
            return GROUP_EVENT_REASON_NOTIFY_SEEN;
        case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
            return GROUP_EVENT_REASON_LONG_TIME_TASK_STARTTED;
        default:
            return 0;
    }
}

void BundleActiveGroupController::ReportEvent(const BundleActiveEvent& event, const int64_t bootBasedTimeStamp,
    const int userId)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::ReportEvent called");
    if (bundleGroupEnable_ == false) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsBundleInstalled(event.bundleName_, userId) == false) {
        return;
    }
    int eventId = event.eventId_;
    if (eventId == BundleActiveEvent::ABILITY_FOREGROUND ||
        eventId == BundleActiveEvent::ABILITY_BACKGROUND ||
        eventId == BundleActiveEvent::SYSTEM_INTERACTIVE ||
        eventId == BundleActiveEvent::USER_INTERACTIVE ||
        eventId == BundleActiveEvent::NOTIFICATION_SEEN ||
        eventId == BundleActiveEvent::LONG_TIME_TASK_STARTTED) {
        std::shared_ptr<BundleActivePackageHistory> bundleUsageHistory = bundleUserHistory_->GetUsageHistoryForBundle(
            event.bundleName_, userId, bootBasedTimeStamp, true);
        if (bundleUsageHistory == nullptr) {
            return;
        }
        int64_t timeUntilNextCheck;
        int eventReason = EventToGroupReason(eventId);
        switch (eventId) {
            case BundleActiveEvent::NOTIFICATION_SEEN:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_DAILY,
                    eventReason, 0, bootBasedTimeStamp + timeoutForNotifySeen_);
                timeUntilNextCheck = timeoutForNotifySeen_;
                break;
            case BundleActiveEvent::SYSTEM_INTERACTIVE:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_ALIVE,
                    eventReason, 0, bootBasedTimeStamp + timeoutForSystemInteraction_);
                timeUntilNextCheck = timeoutForSystemInteraction_;
                break;
            default:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_ALIVE,
                    eventReason, bootBasedTimeStamp, bootBasedTimeStamp + timeoutForDirectlyUse_);
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

void BundleActiveGroupController::CheckAndUpdateGroup(const std::string& bundleName, const int userId,
    const int64_t bootBasedTimeStamp)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName, userId, bootBasedTimeStamp, true);
    if (oneBundleHistory == nullptr) {
        return;
    }
    int groupReason = oneBundleHistory->reasonInGroup_;
    int oldGroupControlReason = groupReason & GROUP_CONTROL_REASON_MASK;
    if (oldGroupControlReason == GROUP_CONTROL_REASON_FORCED) {
        return;
    }
    int oldGroup = oneBundleHistory->currentGroup_;
    int newGroup = std::max(oldGroup, ACTIVE_GROUP_ALIVE);
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
    if (newGroup >= ACTIVE_GROUP_ALIVE && oneBundleHistory->bundleAliveTimeoutTimeStamp_ >
        bootBasedTimeStampAdjusted) {
        newGroup = ACTIVE_GROUP_ALIVE;
        groupReason = oneBundleHistory->reasonInGroup_;
    } else if (newGroup >= ACTIVE_GROUP_DAILY && oneBundleHistory->bundleDailyTimeoutTimeStamp_ >
        bootBasedTimeStampAdjusted) {
        newGroup = ACTIVE_GROUP_DAILY;
        groupReason = (newGroup == oldGroup) ? oneBundleHistory->reasonInGroup_ : GROUP_CONTROL_REASON_USAGE |
            GROUP_EVENT_REASON_ALIVE_TIMEOUT;
    }
    if (oldGroup < newGroup) {
        BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::CheckAndUpdateGroup called SetBundleGroup");
        bundleUserHistory_->SetBundleGroup(bundleName, userId, bootBasedTimeStamp, newGroup, groupReason, false);
    }
}

void BundleActiveGroupController::SetBundleGroup(const std::string& bundleName, const int userId, int newGroup,
    int reason, const int64_t bootBasedTimeStamp, const bool& resetTimeout)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (IsBundleInstalled(bundleName, userId) == false) {
        return;
    }
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName, userId, bootBasedTimeStamp, true);
    if (oneBundleHistory->currentGroup_ < ACTIVE_GROUP_ALIVE) {
        return;
    }
    int64_t bootBasedTimeStampAdjusted = bundleUserHistory_->GetBootBasedTimeStamp(bootBasedTimeStamp);
    if (newGroup > ACTIVE_GROUP_ALIVE && oneBundleHistory->bundleAliveTimeoutTimeStamp_ >
        bootBasedTimeStampAdjusted) {
        newGroup = ACTIVE_GROUP_ALIVE;
        reason = oneBundleHistory->reasonInGroup_;
    } else if (newGroup > ACTIVE_GROUP_DAILY && oneBundleHistory->bundleDailyTimeoutTimeStamp_ >
        bootBasedTimeStampAdjusted) {
        newGroup = ACTIVE_GROUP_DAILY;
        if (oneBundleHistory->currentGroup_ != newGroup) {
            reason = GROUP_CONTROL_REASON_USAGE | GROUP_CONTROL_REASON_TIMEOUT;
        } else {
            reason = oneBundleHistory->reasonInGroup_;
        }
    }
    bundleUserHistory_->SetBundleGroup(bundleName, userId, bootBasedTimeStamp, newGroup, reason, false);
}

int BundleActiveGroupController::IsBundleIdle(const std::string& bundleName, const int userId)
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
        BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::IsBundleIdle, bundle group is %{public}d",
            oneBundleHistory->currentGroup_);
        return 1;
    } else {
        BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::IsBundleIdle, bundle group is %{public}d",
            oneBundleHistory->currentGroup_);
        return 0;
    }
}

int BundleActiveGroupController::QueryPackageGroup(const int userId, const std::string& bundleName)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::QueryPackageGroup called");
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    if (IsBundleInstalled(bundleName, userId) == false) {
        return -1;
    }
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(
        bundleName, userId, bootBasedTimeStamp, false);
    if (oneBundleHistory == nullptr) {
        return -1;
    } else {
        return oneBundleHistory->currentGroup_;
    }
}

bool BundleActiveGroupController::IsBundleInstalled(const std::string& bundleName, const int userId)
{
    ApplicationInfo bundleInfo;
    if (sptrBundleMgr_ != nullptr && sptrBundleMgr_->GetApplicationInfo(
        bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, bundleInfo) == false) {
        BUNDLE_ACTIVE_LOGE("BundleActiveGroupController::IsBundleInstalled bundle is not installed!");
        return false;
    }
    return true;
}

void BundleActiveGroupController::ShutDown(const int64_t bootBasedTimeStamp)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::ShutDown called");
    bundleUserHistory_->UpdateBootBasedAndScreenTime(false, bootBasedTimeStamp, true);
}

bool BundleActiveGroupController::IsScreenOn()
{
    bool result = PowerMgrClient::GetInstance().IsScreenOn();
    BUNDLE_ACTIVE_LOGI("BundleActiveGroupController::IsScreenOn() is %{public}d", result);
    return result;
}
}
}
/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "bundle_active_log.h"
#include "bundle_active_user_history.h"
#include "bundle_active_group_handler.h"
#include "ibundle_active_service.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_util.h"
#include "bundle_active_bundle_mgr_helper.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace DeviceUsageStatsGroupConst;
    const int32_t MAIN_APP_INDEX = 0;

BundleActiveGroupController& BundleActiveGroupController::GetInstance()
{
    static BundleActiveGroupController instance;
    return instance;
}
BundleActiveGroupHandlerObject::BundleActiveGroupHandlerObject()
{
    bundleName_ = "";
    userId_ = -1;
    uid_ = -1;
}


void BundleActiveGroupController::Init(const bool debug)
{
    std::lock_guard<ffrt::mutex> lock(initMutex_);
    if (isInit_) {
        return;
    }
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
    activeGroupHandler_ = std::make_shared<BundleActiveGroupHandler>(debug);
    activeGroupHandler_->Init();
    isInit_ = true;
}

void BundleActiveGroupController::DeInit()
{
    std::lock_guard<ffrt::mutex> lock(initMutex_);
    if (!isInit_) {
        return;
    }
    isInit_ = false;
    activeGroupHandler_->DeInit();
}


std::shared_ptr<BundleActiveGroupHandler> BundleActiveGroupController::GetBundleGroupHandler()
{
    std::lock_guard<ffrt::mutex> lock(initMutex_);
    if (!isInit_) {
        return nullptr;
    }
    return activeGroupHandler_;
}

void BundleActiveGroupController::RestoreDurationToDatabase()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    bundleUserHistory_->WriteDeviceDuration();
}

void BundleActiveGroupController::RestoreToDatabase(const int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    bundleUserHistory_->WriteBundleUsage(userId);
}

void BundleActiveGroupController::OnUserRemoved(const int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    bundleUserHistory_->userHistory_.erase(userId);
    if (activeGroupHandler_ != nullptr) {
        activeGroupHandler_->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE);
    }
}

void BundleActiveGroupController::SetBundleGroupEnable(bool bundleGroupEnable)
{
    bundleGroupEnable_ = bundleGroupEnable;
}

bool BundleActiveGroupController::GetBundleGroupEnable()
{
    return bundleGroupEnable_;
}

void BundleActiveGroupController::OnUserSwitched(const int32_t userId, const int32_t currentUsedUser)
{
    BUNDLE_ACTIVE_LOGI("last time check for user %{public}d", currentUsedUser);
    CheckEachBundleState(currentUsedUser);
    bundleUserHistory_->WriteBundleUsage(currentUsedUser);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (activeGroupHandler_ != nullptr) {
        activeGroupHandler_->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE);
        activeGroupHandler_->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_DEFAULT_BUNDLE_STATE);
        activeGroupHandler_->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_NOTIFICATION_SEEN_BUNDLE_STATE);
        activeGroupHandler_->RemoveEvent(BundleActiveGroupHandler::MSG_CHECK_SYSTEM_INTERACTIVE_BUNDLE_STATE);
    }
    PeriodCheckBundleState(userId);
}

void BundleActiveGroupController::OnScreenChanged(const bool& isScreenOn, const int64_t bootFromTimeStamp)
{
    if (activeGroupHandler_ != nullptr) {
        activeGroupHandler_->PostTask([isScreenOn, bootFromTimeStamp]() {
            std::lock_guard<ffrt::mutex> lock(BundleActiveGroupController::GetInstance().mutex_);
            BundleActiveGroupController::GetInstance().bundleUserHistory_->UpdateBootBasedAndScreenTime(isScreenOn,
                bootFromTimeStamp);
        });
    }
}

void BundleActiveGroupController::CreateUserHistory(const int64_t bootFromTimeStamp,
    const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    if (bundleUserHistory_ == nullptr) {
        BUNDLE_ACTIVE_LOGI("SetHandlerAndCreateUserHistory bundleUserHistory_ is null, "
            "called constructor, bootstamp is %{public}lld", (long long)bootFromTimeStamp);
        bundleUserHistory_ = std::make_shared<BundleActiveUserHistory>(bootFromTimeStamp, bundleActiveCore);
    }
    OnScreenChanged(IsScreenOn(), bootFromTimeStamp);
}

void BundleActiveGroupController::OnBundleUninstalled(const int32_t userId, const std::string& bundleName,
    const int32_t uid, const int32_t appIndex)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    BUNDLE_ACTIVE_LOGI("OnBundleUninstalled called, userId is %{public}d, bundlename is %{public}s",
        userId, bundleName.c_str());
    auto oneUserHistory = bundleUserHistory_->GetUserHistory(userId, false);
    if (oneUserHistory == nullptr) {
        return;
    }
    DeleteUsageGroupCache(oneUserHistory, bundleName, uid, appIndex);
    bundleUserHistory_->OnBundleUninstalled(userId, bundleName, uid, appIndex);
}

void BundleActiveGroupController::DeleteUsageGroupCache(
    const std::shared_ptr<std::map<std::string, std::shared_ptr<BundleActivePackageHistory>>>& userHostory,
    const std::string& bundleName, const int32_t uid, const int32_t appIndex)
{
    if (appIndex != MAIN_APP_INDEX) {
        std::string moduleKey = BundleActiveUtil::GetBundleUsageKey(bundleName, uid);
        userHostory->erase(moduleKey);
    }
    for (auto it = userHostory->begin(); it != userHostory->end();) {
        if (it->first.find(bundleName) != std::string::npos) {
            it = userHostory->erase(it);
        } else {
            it++;
        }
    }
}

void BundleActiveGroupController::PeriodCheckBundleState(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("PeriodCheckBundleState called");
    if (activeGroupHandler_ != nullptr) {
        BundleActiveGroupHandlerObject tmpGroupHandlerObj;
        tmpGroupHandlerObj.userId_ = userId;
        std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveGroupHandlerObject>(tmpGroupHandlerObj);
        activeGroupHandler_->SendEvent(BundleActiveGroupHandler::MSG_CHECK_DEFAULT_BUNDLE_STATE,
            handlerobjToPtr, FIVE_SECOND);
    }
}

bool BundleActiveGroupController::CheckEachBundleState(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("CheckEachBundleState called, userid is %{public}d", userId);
    std::vector<BundleActiveApplication> allBundlesForUser;
    BundleActiveBundleMgrHelper::GetInstance()->GetApplicationInfos(flag, userId, allBundlesForUser);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    for (auto oneBundle : allBundlesForUser) {
        CheckAndUpdateGroup(oneBundle.bundleName, userId, oneBundle.uid, bootBasedTimeStamp);
    }
    return true;
}

void BundleActiveGroupController::CheckIdleStatsOneTime()
{
    BundleActiveGroupHandlerObject tmpGroupHandlerObj;
    std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
        std::make_shared<BundleActiveGroupHandlerObject>(tmpGroupHandlerObj);
    if (activeGroupHandler_ != nullptr) {
        activeGroupHandler_->SendEvent(BundleActiveGroupHandler::MSG_ONE_TIME_CHECK_BUNDLE_STATE,
            handlerobjToPtr);
    }
}

int32_t BundleActiveGroupController::GetNewGroup(const std::string& bundleName, const int32_t userId,
    const int64_t bootBasedTimeStamp, const int32_t uid)
{
    int32_t groupIndex = bundleUserHistory_->GetLevelIndex(bundleName, userId, bootBasedTimeStamp, screenTimeLevel_,
        bootTimeLevel_, uid);
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
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (IsBundleInstalled(event.bundleName_, userId) == false) {
        BUNDLE_ACTIVE_LOGE("Report an uninstalled package event, return!");
        return;
    }
    int32_t eventId = event.eventId_;
    auto item  = eventIdMatchReason_.find(eventId);
    if (item != eventIdMatchReason_.end()) {
        std::shared_ptr<BundleActivePackageHistory> bundleUsageHistory = bundleUserHistory_->GetUsageHistoryForBundle(
            event.bundleName_, userId, bootBasedTimeStamp, true, event.uid_);
        if (bundleUsageHistory == nullptr) {
            return;
        }
        int64_t timeUntilNextCheck;
        uint32_t eventReason = item->second;
        int32_t checkBundleMsgEventId = BundleActiveGroupHandler::MSG_CHECK_DEFAULT_BUNDLE_STATE;
        switch (eventId) {
            case BundleActiveEvent::NOTIFICATION_SEEN:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_DAILY,
                    eventReason, 0, bootBasedTimeStamp + timeoutForNotifySeen_, userId, event.uid_);
                timeUntilNextCheck = timeoutForNotifySeen_;
                checkBundleMsgEventId = BundleActiveGroupHandler::MSG_CHECK_NOTIFICATION_SEEN_BUNDLE_STATE;
                break;
            case BundleActiveEvent::SYSTEM_INTERACTIVE:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_ALIVE,
                    eventReason, 0, bootBasedTimeStamp + timeoutForSystemInteraction_, userId, event.uid_);
                timeUntilNextCheck = timeoutForSystemInteraction_;
                checkBundleMsgEventId = BundleActiveGroupHandler::MSG_CHECK_SYSTEM_INTERACTIVE_BUNDLE_STATE;
                break;
            default:
                bundleUserHistory_->ReportUsage(bundleUsageHistory, event.bundleName_, ACTIVE_GROUP_ALIVE,
                    eventReason, bootBasedTimeStamp, bootBasedTimeStamp + timeoutForDirectlyUse_, userId, event.uid_);
                timeUntilNextCheck = timeoutForDirectlyUse_;
                break;
        }
        SendCheckBundleMsg(event, userId, timeUntilNextCheck, checkBundleMsgEventId);
    }
}

void BundleActiveGroupController::SendCheckBundleMsg(const BundleActiveEvent& event, const int32_t& userId,
    const int64_t& timeUntilNextCheck, const int64_t& checkBundleMsgEventId)
{
    BundleActiveGroupHandlerObject tmpGroupHandlerObj;
    tmpGroupHandlerObj.userId_ = userId;
    tmpGroupHandlerObj.bundleName_ = event.bundleName_;
    tmpGroupHandlerObj.uid_ = event.uid_;
    std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
        std::make_shared<BundleActiveGroupHandlerObject>(tmpGroupHandlerObj);
    if (activeGroupHandler_ != nullptr) {
        activeGroupHandler_->SendCheckBundleMsg(checkBundleMsgEventId, handlerobjToPtr, timeUntilNextCheck);
    }
}

void BundleActiveGroupController::CheckAndUpdateGroup(const std::string& bundleName, const int32_t userId,
    const int32_t uid, const int64_t bootBasedTimeStamp)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName, userId,
        bootBasedTimeStamp, true, uid);
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
        newGroup = GetNewGroup(bundleName, userId, bootBasedTimeStamp, uid);
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
        BUNDLE_ACTIVE_LOGD("CheckAndUpdateGroup called SetAppGroup");
        bundleUserHistory_->SetAppGroup(bundleName, userId, bootBasedTimeStamp, newGroup,
            groupReason, false, uid);
    }
}

ErrCode BundleActiveGroupController::SetAppGroup(const std::string& bundleName, const int32_t userId,
    int32_t newGroup, uint32_t reason, const int64_t bootBasedTimeStamp, const bool isFlush)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!IsBundleInstalled(bundleName, userId)) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    auto iter = bundleUserHistory_->userHistory_.find(userId);
    if (iter == bundleUserHistory_->userHistory_.end()) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    auto packageHistoryMap = iter->second;
    int32_t result = 0;
    int32_t tempResult = 0;
    for (auto packageHistoryIter : *packageHistoryMap) {
        if (packageHistoryIter.first.find(bundleName) == std::string::npos || packageHistoryIter.second == nullptr) {
            continue;
        }
        auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName,
            userId, bootBasedTimeStamp, true, packageHistoryIter.second->uid_);
        if (!oneBundleHistory) {
            continue;
        }
        tempResult = bundleUserHistory_->SetAppGroup(bundleName, userId,
            packageHistoryIter.second->uid_, bootBasedTimeStamp, newGroup, reason, isFlush);
        if (tempResult != ERR_OK) {
            result = tempResult;
        }
    }
    return result;
}

int32_t BundleActiveGroupController::IsBundleIdle(const std::string& bundleName, const int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    if (IsBundleInstalled(bundleName, userId) == false) {
        return -1;
    }
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    auto iter = bundleUserHistory_->userHistory_.find(userId);
    if (iter == bundleUserHistory_->userHistory_.end()) {
        return -1;
    }
    auto packageHistoryMap = iter->second;
    int32_t IsBundleIdle = 1;
    for (auto packageHistoryIter : *packageHistoryMap) {
        if (packageHistoryIter.first.find(bundleName) == std::string::npos || packageHistoryIter.second == nullptr) {
            continue;
        }
        auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName,
            userId, bootBasedTimeStamp, false, packageHistoryIter.second->uid_);
        if (!oneBundleHistory) {
            continue;
        }
        if (oneBundleHistory->currentGroup_ <= ACTIVE_GROUP_RARE) {
            IsBundleIdle = 0;
        }
    }
    return IsBundleIdle;
}

ErrCode BundleActiveGroupController::QueryAppGroup(int32_t& appGroup,
    const std::string& bundleName, const int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
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
    auto iter = bundleUserHistory_->userHistory_.find(userId);
    if (iter == bundleUserHistory_->userHistory_.end()) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    appGroup = ACTIVE_GROUP_NEVER;
    auto packageHistoryMap = iter->second;
    for (auto packageHistoryIter : *packageHistoryMap) {
        if (packageHistoryIter.first.find(bundleName) == std::string::npos || packageHistoryIter.second == nullptr) {
            continue;
        }
        auto oneBundleHistory = bundleUserHistory_->GetUsageHistoryForBundle(bundleName,
            userId, bootBasedTimeStamp, false, packageHistoryIter.second->uid_);
        if (!oneBundleHistory) {
            continue;
        }
        BUNDLE_ACTIVE_LOGI("QueryAppGroup group is %{public}d", oneBundleHistory->currentGroup_);
        appGroup = std::min(oneBundleHistory->currentGroup_, appGroup);
    }
    return ERR_OK;
}

bool BundleActiveGroupController::IsBundleInstalled(const std::string& bundleName, const int32_t userId)
{
    ApplicationInfo bundleInfo;
    bool getInfoIsSuccess = BundleActiveBundleMgrHelper::GetInstance()->GetApplicationInfo(
        bundleName, ApplicationFlag::GET_BASIC_APPLICATION_INFO, userId, bundleInfo);
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
    bool result = true;
#ifdef DEVICE_USAGES_STATISTICS_POWERMANGER_ENABLE
    result = PowerMgrClient::GetInstance().IsScreenOn();
#endif
    BUNDLE_ACTIVE_LOGI("IsScreenOn() is %{public}d", result);
    return result;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


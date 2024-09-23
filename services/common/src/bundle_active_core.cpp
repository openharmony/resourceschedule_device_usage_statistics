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
#include "bundle_active_core.h"
#include "accesstoken_kit.h"
#include "time_service_client.h"

#include "bundle_active_log.h"
#include "bundle_active_event.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_group_common.h"
#include "bundle_active_bundle_mgr_helper.h"
#include "bundle_active_constant.h"
#include "ffrt_inner.h"


namespace OHOS {
namespace DeviceUsageStats {
#ifndef OS_ACCOUNT_PART_ENABLED
const int32_t DEFAULT_OS_ACCOUNT_ID = 0; // 0 is the default id when there is no os_account part
#endif // OS_ACCOUNT_PART_ENABLED
constexpr int32_t BUNDLE_UNINSTALL_DELAY_TIME = 5 * 1000 * 1000;

BundleActiveReportHandlerObject::BundleActiveReportHandlerObject()
{
        userId_ = -1;
        bundleName_ = "";
}

BundleActiveReportHandlerObject::BundleActiveReportHandlerObject(const int32_t userId, const std::string bundleName)
{
    userId_ = userId;
    bundleName_ = bundleName;
}

BundleActiveReportHandlerObject::BundleActiveReportHandlerObject(const BundleActiveReportHandlerObject& orig)
{
    event_ = orig.event_;
    userId_ = orig.userId_;
    bundleName_ = orig.bundleName_;
    uid_ = orig.uid_;
    appIndex_ = orig.appIndex_;
}

BundleActiveCore::BundleActiveCore()
{
    systemTimeShot_ = -1;
    realTimeShot_ = -1;
    currentUsedUser_ = -1;
    if (DEBUG_ON) {
        flushInterval_ = TWO_MINUTE;
        debugCore_ = true;
    } else {
        flushInterval_ = THIRTY_MINUTE;
        debugCore_ = false;
    }
}

BundleActiveCore::~BundleActiveCore()
{
}

void BundleActiveCommonEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    std::string action = data.GetWant().GetAction();
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_OFF ||
        action == CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        HandleScreenEvent();
    } else if (action == CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        HandleUserRemoveEvent(data);
    } else if (action == CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        HandleUserSwitchEvent(data);
    } else if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED ||
        action == CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED) {
        HandlePackageRemoveEvent(data);
    } else if (action == COMMON_EVENT_UNLOCK_SCREEN || action == COMMON_EVENT_LOCK_SCREEN) {
        HandleLockEvent(data);
    } else if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) {
        HandlePackageAddEvent(data);
    }
}

void BundleActiveCommonEventSubscriber::HandleScreenEvent()
{
    if (!bundleActiveReportHandler_.expired()) {
        sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
        auto groupController = activeGroupController_.lock();
        if (groupController == nullptr) {
            return;
        }
        bool isScreenOn = groupController->IsScreenOn();
        BUNDLE_ACTIVE_LOGI("screen state change to %{public}d", isScreenOn);
        groupController->OnScreenChanged(isScreenOn, timer->GetBootTimeMs());
    }
}

void BundleActiveCommonEventSubscriber::HandleUserRemoveEvent(const CommonEventData &data)
{
    if (!bundleActiveReportHandler_.expired()) {
        int32_t userId = data.GetCode();
        BUNDLE_ACTIVE_LOGI("remove user id %{public}d", userId);
        BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        auto reportHandler = bundleActiveReportHandler_.lock();
        if (reportHandler == nullptr) {
            return;
        }
        reportHandler->SendEvent(BundleActiveReportHandler::MSG_REMOVE_USER, handlerobjToPtr);
    }
}

void BundleActiveCommonEventSubscriber::HandleUserSwitchEvent(const CommonEventData &data)
{
    if (!bundleActiveReportHandler_.expired()) {
        int32_t userId = data.GetCode();
        BUNDLE_ACTIVE_LOGI("OnReceiveEvent receive switched user event, user id is %{public}d", userId);
        BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        auto reportHandler = bundleActiveReportHandler_.lock();
        if (reportHandler == nullptr) {
            return;
        }
        reportHandler->SendEvent(BundleActiveReportHandler::MSG_SWITCH_USER, handlerobjToPtr);
    }
}

void BundleActiveCommonEventSubscriber::HandlePackageRemoveEvent(const CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    int32_t userId = data.GetWant().GetIntParam("userId", 0);
    std::string bundleName = data.GetWant().GetElement().GetBundleName();
    BUNDLE_ACTIVE_LOGI("action is %{public}s, userID is %{public}d, bundlename is %{public}s",
        action.c_str(), userId, bundleName.c_str());
    if (!bundleActiveReportHandler_.expired()) {
        BundleActiveReportHandlerObject tmpHandlerObject(userId, bundleName);
        tmpHandlerObject.uid_ = data.GetWant().GetIntParam("uid", -1);
        tmpHandlerObject.appIndex_ = data.GetWant().GetIntParam("appIndex", -1);
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        auto reportHandler = bundleActiveReportHandler_.lock();
        if (reportHandler == nullptr) {
            return;
        }
        reportHandler->SendEvent(BundleActiveReportHandler::MSG_BUNDLE_UNINSTALLED,
            handlerobjToPtr);
    }
}

void BundleActiveCommonEventSubscriber::HandlePackageAddEvent(const CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    int32_t userId = data.GetWant().GetIntParam("userId", 0);
    std::string bundleName = data.GetWant().GetElement().GetBundleName();
    BUNDLE_ACTIVE_LOGI("action is %{public}s, userID is %{public}d, bundlename is %{public}s",
        action.c_str(), userId, bundleName.c_str());
    if (!bundleActiveReportHandler_.expired()) {
        BundleActiveReportHandlerObject tmpHandlerObject(userId, bundleName);
        tmpHandlerObject.uid_ = data.GetWant().GetIntParam("uid", -1);
        tmpHandlerObject.appIndex_ = data.GetWant().GetIntParam("appIndex", -1);
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        auto reportHandler = bundleActiveReportHandler_.lock();
        if (reportHandler == nullptr) {
            return;
        }
        reportHandler->SendEvent(BundleActiveReportHandler::MSG_BUNDLE_INSTALLED,
            handlerobjToPtr);
    }
}

void BundleActiveCommonEventSubscriber::HandleLockEvent(const CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    int32_t userId = data.GetWant().GetIntParam("userId", 0);
    BUNDLE_ACTIVE_LOGI("action is %{public}s, userID is %{public}d", action.c_str(), userId);
    if (bundleActiveReportHandler_.expired()) {
        return;
    }
    BundleActiveReportHandlerObject tmpHandlerObject(-1, "");
    BundleActiveEvent newEvent;
    tmpHandlerObject.event_ = newEvent;
    if (action == COMMON_EVENT_UNLOCK_SCREEN) {
        tmpHandlerObject.event_.eventId_ = BundleActiveEvent::SYSTEM_UNLOCK;
    } else {
        tmpHandlerObject.event_.eventId_ = BundleActiveEvent::SYSTEM_LOCK;
    }
    tmpHandlerObject.userId_ = userId;
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
    auto handlerobjToPtr = std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
    bundleActiveReportHandler_.lock()->SendEvent(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
}

void BundleActiveCore::RegisterSubscriber()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_BUNDLE_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(COMMON_EVENT_UNLOCK_SCREEN);
    matchingSkills.AddEvent(COMMON_EVENT_LOCK_SCREEN);
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    commonEventSubscriber_ = std::make_shared<BundleActiveCommonEventSubscriber>(subscriberInfo,
        bundleGroupController_, handler_);
    bool subscribeResult = CommonEventManager::SubscribeCommonEvent(commonEventSubscriber_);
    BUNDLE_ACTIVE_LOGD("Register for events result is %{public}d", subscribeResult);
}

void BundleActiveCore::UnRegisterSubscriber()
{
    CommonEventManager::UnSubscribeCommonEvent(commonEventSubscriber_);
}

void BundleActiveCore::Init()
{
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    do {
        realTimeShot_ = timer->GetBootTimeMs();
        systemTimeShot_ = GetSystemTimeMs();
    } while (realTimeShot_ == -1 && systemTimeShot_ == -1);
    realTimeShot_ = timer->GetBootTimeMs();
    systemTimeShot_ = GetSystemTimeMs();
    bundleGroupController_ = std::make_shared<BundleActiveGroupController>(debugCore_);
    BUNDLE_ACTIVE_LOGD("system time shot is %{public}lld", (long long)systemTimeShot_);
}

void BundleActiveCore::InitBundleGroupController()
{
    BUNDLE_ACTIVE_LOGD("InitBundleGroupController called");
    bundleGroupHandler_ = std::make_shared<BundleActiveGroupHandler>(debugCore_);
    if (bundleGroupHandler_ == nullptr) {
        return;
    }
    if (bundleGroupController_ != nullptr) {
        bundleGroupHandler_->Init(bundleGroupController_);
        bundleGroupController_->SetHandlerAndCreateUserHistory(bundleGroupHandler_, realTimeShot_, shared_from_this());
        BUNDLE_ACTIVE_LOGI("Init Set group controller and handler done");
    } else {
        return;
    }
    RegisterSubscriber();
    std::vector<int32_t> activatedOsAccountIds;
    bundleGroupController_->bundleGroupEnable_ = true;
    GetAllActiveUser(activatedOsAccountIds);
    if (activatedOsAccountIds.size() == 0) {
        BUNDLE_ACTIVE_LOGI("query activated account failed, no account activated");
        return;
    }
    for (uint32_t i = 0; i < activatedOsAccountIds.size(); i++) {
        bundleGroupController_->PeriodCheckBundleState(activatedOsAccountIds[i]);
    }
}

void BundleActiveCore::SetHandler(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
    handler_ = reportHandler;
}

std::shared_ptr<BundleActiveUserService> WEAK_FUNC BundleActiveCore::GetUserDataAndInitializeIfNeeded(
    const int32_t userId, const int64_t timeStamp, const bool debug)
{
    BUNDLE_ACTIVE_LOGD("GetUserDataAndInitializeIfNeeded called");
    std::map<int, std::shared_ptr<BundleActiveUserService>>::iterator it = userStatServices_.find(userId);
    if (it == userStatServices_.end()) {
        BUNDLE_ACTIVE_LOGI("first initialize user service");
        std::shared_ptr<BundleActiveUserService> service = std::make_shared<BundleActiveUserService>(userId, *this,
            debug);
        service->Init(timeStamp);
        userStatServices_[userId] = service;
        BUNDLE_ACTIVE_LOGI("service is not null");
        return service;
    }
    return it->second;
}

void BundleActiveCore::OnBundleUninstalled(const int32_t userId, const std::string& bundleName,
    const int32_t uid, const int32_t appIndex)
{
    BUNDLE_ACTIVE_LOGD("OnBundleUninstalled CALLED");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return;
    }
    auto service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (service == nullptr) {
        return;
    }
    AddbundleUninstalledUid(uid);
    DelayRemoveBundleUninstalledUid(uid);
    service->DeleteUninstalledBundleStats(bundleName, uid, appIndex);
    bundleGroupController_->OnBundleUninstalled(userId, bundleName, uid, appIndex);
}

void BundleActiveCore::OnBundleInstalled(const int32_t userId, const std::string& bundleName,
    const int32_t uid, const int32_t appIndex)
{
    BUNDLE_ACTIVE_LOGD("OnBundleInstalled CALLED");
    std::lock_guard<ffrt::mutex> lock(bundleUninstalledMutex_);
    if (bundleUninstalledSet_.find(uid) == bundleUninstalledSet_.end()) {
        return;
    }
    bundleUninstalledSet_.erase(uid);
    if (taskMap_.find(uid) == taskMap_.end()) {
        return;
    }
    ffrt::skip(taskMap_[uid]);
    taskMap_.erase(uid);
}

void BundleActiveCore::AddbundleUninstalledUid(const int32_t uid)
{
    std::lock_guard<ffrt::mutex> lock(bundleUninstalledMutex_);
    bundleUninstalledSet_.insert(uid);
}

void BundleActiveCore::DelayRemoveBundleUninstalledUid(const int32_t uid)
{
    auto bundleActiveCore = shared_from_this();
    auto task = ffrt::submit_h([bundleActiveCore, uid]() {
        std::lock_guard<ffrt::mutex> lock(bundleActiveCore->bundleUninstalledMutex_);
        bundleActiveCore->bundleUninstalledSet_.erase(uid);
        if (bundleActiveCore->taskMap_.find(uid) == bundleActiveCore->taskMap_.end()) {
            return;
        }
        bundleActiveCore->taskMap_.erase(uid);
        }, {}, {}, ffrt::task_attr().delay(BUNDLE_UNINSTALL_DELAY_TIME));
    taskMap_[uid] = std::move(task);
}

bool BundleActiveCore::isUninstalledApp(const int32_t uid)
{
    std::lock_guard<ffrt::mutex> lock(bundleUninstalledMutex_);
    if (bundleUninstalledSet_.find(uid) != bundleUninstalledSet_.end()) {
        return true;
    }
    return false;
}

void BundleActiveCore::OnStatsChanged(const int32_t userId)
{
    if (!handler_.expired()) {
        BundleActiveReportHandlerObject tmpHandlerObject;
        tmpHandlerObject.userId_ = userId;
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        if (handler_.lock()->HasEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK) == false) {
            BUNDLE_ACTIVE_LOGI("OnStatsChanged send flush to disk event for user %{public}d", userId);
            handler_.lock()->SendEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK, handlerobjToPtr, flushInterval_);
        }
    }
}

void BundleActiveCore::RestoreAllData()
{
    for (const auto& it : userStatServices_) {
        std::shared_ptr<BundleActiveUserService> service = it.second;
        if (service == nullptr) {
            BUNDLE_ACTIVE_LOGI("service in BundleActiveCore::RestoreToDatabaseLocked() is null");
            return;
        }
        BUNDLE_ACTIVE_LOGI("userid is %{public}d ", service->userId_);
        service->RestoreStats(true);
        if (bundleGroupController_ != nullptr && bundleGroupController_->bundleUserHistory_ != nullptr) {
            bundleGroupController_->RestoreToDatabase(it.first);
        }
    }
    if (bundleGroupController_ != nullptr) {
        bundleGroupController_->RestoreDurationToDatabase();
    }
    if (!handler_.expired()) {
        BUNDLE_ACTIVE_LOGI("RestoreAllData remove flush to disk event");
        handler_.lock()->RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
    }
}

void BundleActiveCore::RestoreToDatabase(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("RestoreToDatabase called");
    BundleActiveEvent event;
    event.eventId_ = BundleActiveEvent::FLUSH;
    event.timeStamp_ = GetSystemTimeMs();
    event.abilityId_ = "";
    auto it = userStatServices_.find(userId);
    if (it != userStatServices_.end()) {
        it->second->ReportEvent(event);
    }
    RestoreToDatabaseLocked(userId);
}

void BundleActiveCore::RestoreToDatabaseLocked(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("RestoreToDatabaseLocked called");
    auto it = userStatServices_.find(userId);
    if (it != userStatServices_.end()) {
        it->second->RestoreStats(false);
    }
    if (bundleGroupController_ != nullptr) {
        bundleGroupController_->RestoreDurationToDatabase();
    }
    if (!handler_.expired()) {
        BUNDLE_ACTIVE_LOGI("RestoreToDatabaseLocked remove flush to disk event");
        handler_.lock()->RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
    }
}

void BundleActiveCore::PreservePowerStateInfo(const int32_t eventId)
{
    if (!handler_.expired()) {
        int32_t userId = -1;
        std::vector<int32_t> currentActiveUser;
        BundleActiveCore::GetAllActiveUser(currentActiveUser);
        if (currentActiveUser.size() == 1) {
            userId = currentActiveUser.front();
        }
        BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
        BundleActiveEvent newEvent;
        tmpHandlerObject.event_ = newEvent;
        tmpHandlerObject.event_.eventId_ = eventId;
        sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
        tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        handler_.lock()->SendEvent(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
    }
}

void BundleActiveCore::ShutDown()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    BUNDLE_ACTIVE_LOGD("ShutDown called");
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t timeStamp = timer->GetBootTimeMs();
    BundleActiveEvent event(BundleActiveEvent::SHUTDOWN, timeStamp);
    event.bundleName_ = BundleActiveEvent::DEVICE_EVENT_PACKAGE_NAME;
    std::vector<int32_t> activatedOsAccountIds;
    GetAllActiveUser(activatedOsAccountIds);
    if (activatedOsAccountIds.size() == 0) {
        BUNDLE_ACTIVE_LOGI("query activated account failed, no account activated");
        return;
    }
    for (uint32_t i = 0; i < activatedOsAccountIds.size(); i++) {
        bundleGroupController_->ShutDown(timeStamp, activatedOsAccountIds[i]);
    }
    ReportEventToAllUserId(event);
    RestoreAllData();
}

void BundleActiveCore::OnStatsReload()
{
    BUNDLE_ACTIVE_LOGD("OnStatsReload called");
    bundleGroupController_->CheckIdleStatsOneTime();
}

void BundleActiveCore::OnSystemUpdate(int32_t userId)
{
}

int64_t WEAK_FUNC BundleActiveCore::CheckTimeChangeAndGetWallTime(int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("CheckTimeChangeAndGetWallTime called, userId is %{public}d", userId);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t actualSystemTime = GetSystemTimeMs();
    int64_t actualRealTime = timer->GetBootTimeMs();
    int64_t expectedSystemTime = (actualRealTime - realTimeShot_) + systemTimeShot_;
    int64_t diffSystemTime = actualSystemTime - expectedSystemTime;
    if (actualSystemTime == -1 || actualRealTime == -1) {
        return ERR_TIME_OPERATION_FAILED;
    }
    BUNDLE_ACTIVE_LOGD("asystime is %{public}lld, artime is %{public}lld, esystime is %{public}lld, "
        "diff is %{public}lld", (long long)actualSystemTime,
        (long long)actualRealTime, (long long)expectedSystemTime, (long long)diffSystemTime);
    if (std::abs(diffSystemTime) > TIME_CHANGE_THRESHOLD_MILLIS) {
        // 时区变换逻辑
        auto it = userStatServices_.find(userId);
        if (it != userStatServices_.end()) {
            BundleActiveEvent event;
            event.eventId_ = BundleActiveEvent::FLUSH;
            event.timeStamp_ = expectedSystemTime;
            event.abilityId_ = "";
            it->second->ReportEvent(event);
            it->second->RestoreStats(true);
            it->second->RenewTableTime(expectedSystemTime, actualSystemTime);
            it->second->LoadActiveStats(actualSystemTime, true, true);
            it->second->LoadModuleAndFormStats();
            if (!handler_.expired()) {
                BUNDLE_ACTIVE_LOGI("CheckTimeChangeAndGetWallTime remove flush to disk event");
                handler_.lock()->RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
            }
        }
        realTimeShot_ = actualRealTime;
        systemTimeShot_ = actualSystemTime;
    }
    return actualSystemTime;
}

void BundleActiveCore::ConvertToSystemTimeLocked(BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGD("ConvertToSystemTimeLocked called");
    event.timeStamp_ = std::max((int64_t)0, event.timeStamp_ - realTimeShot_) + systemTimeShot_;
}

void BundleActiveCore::OnUserRemoved(const int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("OnUserRemoved called");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    auto it = userStatServices_.find(userId);
    if (it == userStatServices_.end()) {
        return;
    }
    userStatServices_[userId]->OnUserRemoved();
    userStatServices_[userId].reset();
    userStatServices_.erase(userId);
    bundleGroupController_->OnUserRemoved(userId);
}

void BundleActiveCore::OnUserSwitched(const int32_t userId)
{
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    auto it = userStatServices_.find(currentUsedUser_);
    if (it != userStatServices_.end()) {
        BUNDLE_ACTIVE_LOGI("restore old user id %{public}d data when switch user", currentUsedUser_);
        BundleActiveEvent event;
        event.eventId_ = BundleActiveEvent::FLUSH;
        int64_t actualRealTime = timer->GetBootTimeMs();
        event.timeStamp_ = (actualRealTime - realTimeShot_) + systemTimeShot_;
        event.abilityId_ = "";
        it->second->ReportEvent(event);
        it->second->RestoreStats(true);
    }
    std::vector<int32_t> activatedOsAccountIds;
    GetAllActiveUser(activatedOsAccountIds);
    if (activatedOsAccountIds.size() == 0) {
        BUNDLE_ACTIVE_LOGI("query activated account failed, no account activated");
        return;
    }
    for (uint32_t i = 0; i < activatedOsAccountIds.size(); i++) {
        BUNDLE_ACTIVE_LOGI("start to period check for userId %{public}d", activatedOsAccountIds[i]);
        bundleGroupController_->OnUserSwitched(activatedOsAccountIds[i], currentUsedUser_);
    }
    currentUsedUser_ = userId;
    OnStatsChanged(userId);
}

int32_t BundleActiveCore::ReportEvent(BundleActiveEvent& event, int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("FLUSH interval is %{public}lld, debug is %{public}d", (long long)flushInterval_, debugCore_);
    ObtainSystemEventName(event);
    event.PrintEvent(debugCore_);
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (userId == 0 || userId == -1) {
        return -1;
    }
    if (currentUsedUser_ == -1) {
        currentUsedUser_ = userId;
        BUNDLE_ACTIVE_LOGI("last used id change to %{public}d", currentUsedUser_);
    }
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    if (BundleActiveBundleMgrHelper::GetInstance()->IsLauncherApp(event.bundleName_, userId)) {
        BUNDLE_ACTIVE_LOGI("launcher event, only update app group");
        bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);
        return 0;
    }
    BUNDLE_ACTIVE_LOGD("report event called, bundle name %{public}s time %{public}lld userId %{public}d, "
        "eventid %{public}d, in lock range", event.bundleName_.c_str(),
        (long long)event.timeStamp_, userId, event.eventId_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return -1;
    }
    ConvertToSystemTimeLocked(event);
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (service == nullptr) {
        BUNDLE_ACTIVE_LOGE("get user data service failed!");
        return -1;
    }
    if (event.eventId_ == BundleActiveEvent::FORM_IS_CLICKED ||
        event.eventId_ == BundleActiveEvent::FORM_IS_REMOVED) {
        service->ReportFormEvent(event);
        return 0;
    }
    service->ReportModuleEvent(event);
    service->ReportEvent(event);
    bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);
    return 0;
}

void BundleActiveCore::ObtainSystemEventName(BundleActiveEvent& event)
{
    switch (event.eventId_) {
        case BundleActiveEvent::SYSTEM_LOCK:
            event.bundleName_ = OPERATION_SYSTEM_LOCK;
            break;
        case BundleActiveEvent::SYSTEM_UNLOCK:
            event.bundleName_ = OPERATION_SYSTEM_UNLOCK;
            break;
        case BundleActiveEvent::SYSTEM_SLEEP:
            event.bundleName_ = OPERATION_SYSTEM_SLEEP;
            break;
        case BundleActiveEvent::SYSTEM_WAKEUP:
            event.bundleName_ = OPERATION_SYSTEM_WAKEUP;
            break;
        default:
            break;
    }
}

int32_t BundleActiveCore::ReportEventToAllUserId(BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGD("ReportEventToAllUserId called");
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return -1;
    }
    if (userStatServices_.empty()) {
        return DEFAULT_USER_ID;
    }
    for (const auto& it : userStatServices_) {
        ConvertToSystemTimeLocked(event);
        std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(it.first, timeNow,
            debugCore_);
        if (service == nullptr) {
            BUNDLE_ACTIVE_LOGE("get user data service failed!");
            return -1;
        }
        BUNDLE_ACTIVE_LOGI("ReportEventToAllUserId SERVICE user ID IS userId %{public}d", service->userId_);
        service->ReportForShutdown(event);
        return 0;
    }
    return 0;
}

ErrCode BundleActiveCore::QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& packageStats,
    const int32_t userId, const int32_t intervalType, const int64_t beginTime, const int64_t endTime,
    std::string bundleName)
{
    BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfos called");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return ERR_TIME_OPERATION_FAILED;
    }

    BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfos begin time is %{public}lld, end time is %{public}lld, "
        "intervaltype is %{public}d", (long long)beginTime, (long long)endTime, intervalType);
    if (beginTime > timeNow || beginTime >= endTime) {
        BUNDLE_ACTIVE_LOGI("QueryBundleStatsInfos time span illegal");
        return ERR_QUERY_TIME_OUT_OF_RANGE;
    }

    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (service == nullptr) {
        BUNDLE_ACTIVE_LOGI("QueryBundleStatsInfos service is null, failed");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    return service->QueryBundleStatsInfos(packageStats, intervalType, beginTime, endTime, userId, bundleName);
}

ErrCode BundleActiveCore::QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvent, const int32_t userId,
    const int64_t beginTime, const int64_t endTime, std::string bundleName)
{
    BUNDLE_ACTIVE_LOGD("QueryBundleEvents called");
    std::lock_guard<ffrt::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return ERR_TIME_OPERATION_FAILED;
    }
    if (beginTime > timeNow || beginTime >= endTime) {
        return ERR_QUERY_TIME_OUT_OF_RANGE;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (service == nullptr) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    auto item = service->QueryBundleEvents(bundleActiveEvent, beginTime, endTime, userId, bundleName);
    return item;
}

ErrCode BundleActiveCore::QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
    int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return ERR_TIME_OPERATION_FAILED;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (!service) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    return service->QueryModuleUsageRecords(maxNum, results);
}

ErrCode BundleActiveCore::QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return ERR_TIME_OPERATION_FAILED;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (!service) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    return service->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);
}

ErrCode BundleActiveCore::QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == ERR_TIME_OPERATION_FAILED) {
        return ERR_TIME_OPERATION_FAILED;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow, debugCore_);
    if (!service) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    return service->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);
}

ErrCode BundleActiveCore::SetAppGroup(
    const std::string& bundleName, const int32_t newGroup, const int32_t userId, const bool isFlush)
{
    int32_t newReason = GROUP_CONTROL_REASON_FORCED;
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    return bundleGroupController_->SetAppGroup(
        bundleName, userId, newGroup, newReason, bootBasedTimeStamp, isFlush);
}

ErrCode BundleActiveCore::QueryAppGroup(int32_t& appGroup, const std::string& bundleName, const int32_t userId)
{
    return bundleGroupController_->QueryAppGroup(appGroup, bundleName, userId);
}

int32_t BundleActiveCore::IsBundleIdle(const std::string& bundleName, const int32_t userId)
{
    return bundleGroupController_->IsBundleIdle(bundleName, userId);
}

void BundleActiveCore::GetAllActiveUser(std::vector<int32_t>& activatedOsAccountIds)
{
#ifdef OS_ACCOUNT_PART_ENABLED
    if (AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds) != ERR_OK) {
        BUNDLE_ACTIVE_LOGI("query activated account failed");
        return;
    }
#else // OS_ACCOUNT_PART_ENABLED
    activatedOsAccountIds.clear();
    activatedOsAccountIds.push_back(DEFAULT_OS_ACCOUNT_ID);
    BUNDLE_ACTIVE_LOGI("os account part is not enabled, use default id.");
#endif // OS_ACCOUNT_PART_ENABLED
    if (activatedOsAccountIds.size() == 0) {
        BUNDLE_ACTIVE_LOGI("GetAllActiveUser size is 0");
        return;
    }
}

int64_t BundleActiveCore::GetSystemTimeMs()
{
    time_t now;
    (void)time(&now);  // unit is seconds.
    if (static_cast<int64_t>(now) < 0) {
        BUNDLE_ACTIVE_LOGE("Get now time error");
        return 0;
    }
    auto tarEndTimePoint = std::chrono::system_clock::from_time_t(now);
    auto tarDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tarEndTimePoint.time_since_epoch());
    int64_t tarDate = tarDuration.count();
    if (tarDate < 0) {
        BUNDLE_ACTIVE_LOGE("tarDuration is less than 0.");
        return -1;
    }
    return static_cast<int64_t>(tarDate);
}

void BundleActiveCore::OnAppGroupChanged(const AppGroupCallbackInfo& callbackInfo)
{
    std::shared_ptr<BundleActiveCore> bundleActiveCore = shared_from_this();
    AccessToken::HapTokenInfo tokenInfo = AccessToken::HapTokenInfo();
    bundleGroupHandler_->PostTask([bundleActiveCore, callbackInfo, tokenInfo]() {
        bundleActiveCore->NotifOberserverGroupChanged(callbackInfo, tokenInfo);
    });
}

void BundleActiveCore::NotifOberserverGroupChanged(const AppGroupCallbackInfo& callbackInfo,
    AccessToken::HapTokenInfo tokenInfo)
{
    std::lock_guard<ffrt::recursive_mutex> lock(callbackMutex_);
    for (const auto &item : groupChangeObservers_) {
        auto observer = item.second;
        if (!observer) {
            continue;
        }
        BUNDLE_ACTIVE_LOGI(
            "RegisterAppGroupCallBack will OnAppGroupChanged!,oldGroup is %{public}d, newGroup is %{public}d",
            callbackInfo.GetOldGroup(), callbackInfo.GetNewGroup());
        if (AccessToken::AccessTokenKit::GetTokenType(item.first) == AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
            observer->OnAppGroupChanged(callbackInfo);
        } else if (AccessToken::AccessTokenKit::GetTokenType(item.first) ==
                    AccessToken::ATokenTypeEnum::TOKEN_HAP) {
            AccessToken::AccessTokenKit::GetHapTokenInfo(item.first, tokenInfo);
            if (tokenInfo.userID == callbackInfo.GetUserId()) {
                observer->OnAppGroupChanged(callbackInfo);
            }
        }
    }
}

ErrCode BundleActiveCore::RegisterAppGroupCallBack(const AccessToken::AccessTokenID& tokenId,
    const sptr<IAppGroupCallback> &observer)
{
    std::lock_guard<ffrt::recursive_mutex> lock(callbackMutex_);
    if (!observer) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    auto item = groupChangeObservers_.find(tokenId);
    if (item != groupChangeObservers_.end()) {
        return ERR_REPEAT_REGISTER_OR_DEREGISTER_GROUP_CALLBACK;
    }
    groupChangeObservers_.emplace(tokenId, observer);
    AddObserverDeathRecipient(observer);
    BUNDLE_ACTIVE_LOGD("RegisterAppGroupCallBack number is %{public}d", static_cast<int>(groupChangeObservers_.size()));
    return ERR_OK;
}

ErrCode BundleActiveCore::UnRegisterAppGroupCallBack(const AccessToken::AccessTokenID& tokenId,
    const sptr<IAppGroupCallback> &observer)
{
    std::lock_guard<ffrt::recursive_mutex> lock(callbackMutex_);
    auto item = groupChangeObservers_.find(tokenId);
    if (item == groupChangeObservers_.end()) {
        BUNDLE_ACTIVE_LOGI("UnRegisterAppGroupCallBack observer is not exist, return");
        return ERR_REPEAT_REGISTER_OR_DEREGISTER_GROUP_CALLBACK;
    }
    RemoveObserverDeathRecipient(item->second);
    groupChangeObservers_.erase(tokenId);
    return ERR_OK;
}

void BundleActiveCore::AddObserverDeathRecipient(const sptr<IAppGroupCallback> &observer)
{
    std::lock_guard<ffrt::recursive_mutex> lock(callbackMutex_);
    if (!observer) {
        BUNDLE_ACTIVE_LOGI("observer nullptr.");
        return;
    }
    auto object = observer->AsObject();
    if (!object) {
        BUNDLE_ACTIVE_LOGI("observer->AsObject() nullptr.");
        return;
    }
    auto it = recipientMap_.find(observer->AsObject());
    if (it != recipientMap_.end()) {
        BUNDLE_ACTIVE_LOGI("This death recipient has been added.");
        return;
    }
    sptr<RemoteDeathRecipient> deathRecipient = new (std::nothrow) RemoteDeathRecipient(
        [this](const wptr<IRemoteObject> &remote) { this->OnObserverDied(remote); });
    if (!deathRecipient) {
        BUNDLE_ACTIVE_LOGI("create death recipient failed");
        return ;
    }
    observer->AsObject()->AddDeathRecipient(deathRecipient);
    recipientMap_.emplace(observer->AsObject(), deathRecipient);
}
void BundleActiveCore::RemoveObserverDeathRecipient(const sptr<IAppGroupCallback> &observer)
{
    std::lock_guard<ffrt::recursive_mutex> lock(callbackMutex_);
    if (!observer) {
        return;
    }
    auto object = observer->AsObject();
    if (!object) {
        return ;
    }
    auto iter = recipientMap_.find(observer->AsObject());
    if (iter != recipientMap_.end()) {
        iter->first->RemoveDeathRecipient(iter->second);
        recipientMap_.erase(iter);
    }
}

void BundleActiveCore::OnObserverDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        BUNDLE_ACTIVE_LOGE("remote object is null.");
        return;
    }
    std::shared_ptr<BundleActiveCore> bundleActiveCore = shared_from_this();
    bundleGroupHandler_->PostSyncTask([bundleActiveCore, &remote]() {
        bundleActiveCore->OnObserverDiedInner(remote);
    });
}

void BundleActiveCore::OnObserverDiedInner(const wptr<IRemoteObject> &remote)
{
    sptr<IRemoteObject> objectProxy = remote.promote();
    if (remote == nullptr || !objectProxy) {
        BUNDLE_ACTIVE_LOGE("get remote object failed");
        return;
    }
    std::lock_guard<ffrt::recursive_mutex> lock(callbackMutex_);
    for (const auto& item : groupChangeObservers_) {
        if (!(item.second)) {
            continue;
        }
        auto object = (item.second)->AsObject();
        if (!object) {
            continue;
        }
        if (object == objectProxy) {
            groupChangeObservers_.erase(item.first);
            break;
        }
    }
    recipientMap_.erase(objectProxy);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


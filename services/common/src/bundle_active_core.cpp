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

#include "bundle_active_event.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_group_common.h"
#include "bundle_active_constant.h"
#include "bundle_active_core.h"

namespace OHOS {
namespace DeviceUsageStats {
const std::string LAUNCHER_BUNDLE_NAME = "com.ohos.launcher";
BundleActiveReportHandlerObject::BundleActiveReportHandlerObject()
{
        userId_ = -1;
        bundleName_ = "";
}

BundleActiveReportHandlerObject::BundleActiveReportHandlerObject(const int userId, const std::string bundleName)
{
    userId_ = userId;
    bundleName_ = bundleName;
}

BundleActiveReportHandlerObject::BundleActiveReportHandlerObject(const BundleActiveReportHandlerObject& orig)
{
    event_.bundleName_ = orig.event_.bundleName_;
    event_.abilityName_ = orig.event_.abilityName_;
    event_.abilityId_ = orig.event_.abilityId_;
    event_.timeStamp_ = orig.event_.timeStamp_;
    event_.eventId_ = orig.event_.eventId_;
    event_.continuousTaskAbilityName_ = orig.event_.continuousTaskAbilityName_;
    userId_ = orig.userId_;
    bundleName_ = orig.bundleName_;
}

BundleActiveCore::BundleActiveCore()
{
    systemTimeShot_ = -1;
    realTimeShot_ = -1;
    lastUsedUser_ = -1;
}

BundleActiveCore::~BundleActiveCore()
{
}

void BundleActiveCommonEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string action = data.GetWant().GetAction();
    BUNDLE_ACTIVE_LOGI("OnReceiveEvent action is %{public}s", action.c_str());
    auto want = data.GetWant();
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_OFF ||
        action == CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        if (!activeGroupController_.expired()) {
            sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
            bool isScreenOn = activeGroupController_.lock()->IsScreenOn();
            BUNDLE_ACTIVE_LOGI("OnReceiveEvent Screen state changed "
                "received, screen state chante to %{public}d", isScreenOn);
            activeGroupController_.lock()->OnScreenChanged(isScreenOn, timer->GetBootTimeMs());
        }
    } else if (action == CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        if (!bundleActiveReportHandler_.expired()) {
            int32_t userId = data.GetCode();
            BUNDLE_ACTIVE_LOGI("remove user id %{public}d", userId);
            BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_REMOVE_USER, handlerobjToPtr);
            bundleActiveReportHandler_.lock()->SendEvent(event);
        }
    } else if (action == CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        int32_t userId = data.GetCode();
        BUNDLE_ACTIVE_LOGI("OnReceiveEvent receive switched user event, user id is %{public}d", userId);
        BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_SWITCH_USER, handlerobjToPtr);
        bundleActiveReportHandler_.lock()->SendEvent(event, DELAY_WHEN_USER_SWITCH);
    } else if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED ||
        action == CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED) {
        int32_t userId = data.GetWant().GetIntParam("userId", 0);
        std::string bundleName = data.GetWant().GetElement().GetBundleName();
        BUNDLE_ACTIVE_LOGI("action is %{public}s, userID is %{public}d, bundlename is %{public}s",
            action.c_str(), userId, bundleName.c_str());
        if (!bundleActiveReportHandler_.expired()) {
            BundleActiveReportHandlerObject tmpHandlerObject(userId, bundleName);
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_BUNDLE_UNINSTALLED,
                handlerobjToPtr);
            bundleActiveReportHandler_.lock()->SendEvent(BundleActiveReportHandler::MSG_BUNDLE_UNINSTALLED,
                handlerobjToPtr);
        }
    }
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
    CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    commonEventSubscriber_ = std::make_shared<BundleActiveCommonEventSubscriber>(subscriberInfo,
        bundleGroupController_, handler_);
    bool subscribeResult = CommonEventManager::SubscribeCommonEvent(commonEventSubscriber_);
    BUNDLE_ACTIVE_LOGI("Register for events result is %{public}d", subscribeResult);
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
    bundleGroupController_ = std::make_shared<BundleActiveGroupController>();
    BUNDLE_ACTIVE_LOGI("system time shot is %{public}lld", systemTimeShot_);
}

void BundleActiveCore::InitBundleGroupController()
{
    BUNDLE_ACTIVE_LOGI("InitBundleGroupController called");
    std::string threadName = "bundle_active_group_handler";
    auto runner = AppExecFwk::EventRunner::Create(threadName);
    if (runner == nullptr) {
        BUNDLE_ACTIVE_LOGE("report handler is null");
        return;
    }
    bundleGroupHandler_ = std::make_shared<BundleActiveGroupHandler>(runner);
    if (bundleGroupHandler_ == nullptr) {
        return;
    }
    if (bundleGroupController_ != nullptr && bundleGroupHandler_ != nullptr) {
        bundleGroupHandler_->Init(bundleGroupController_);
        bundleGroupController_->SetHandlerAndCreateUserHistory(bundleGroupHandler_, realTimeShot_);
        BUNDLE_ACTIVE_LOGI("Init Set group controller and handler done");
    }
    RegisterSubscriber();
    std::vector<int> activatedOsAccountIds;
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

std::shared_ptr<BundleActiveUserService> BundleActiveCore::GetUserDataAndInitializeIfNeeded(const int userId,
    const int64_t timeStamp)
{
    BUNDLE_ACTIVE_LOGI("GetUserDataAndInitializeIfNeeded called");
    std::map<int, std::shared_ptr<BundleActiveUserService>>::iterator it = userStatServices_.find(userId);
    if (it == userStatServices_.end()) {
        BUNDLE_ACTIVE_LOGI("first initialize user service");
        std::shared_ptr<BundleActiveUserService> service = std::make_shared<BundleActiveUserService>(userId, *this);
        service->Init(timeStamp);
        userStatServices_[userId] = service;
        if (service == nullptr) {
            BUNDLE_ACTIVE_LOGE("service is null");
            return nullptr;
        }
        BUNDLE_ACTIVE_LOGI("service is not null");
        return service;
    }
    return it->second;
}
void BundleActiveCore::OnBundleUninstalled(const int userId, const std::string& bundleName)
{
    BUNDLE_ACTIVE_LOGI("OnBundleUninstalled CALLED");
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == -1) {
        return;
    }
    auto service = GetUserDataAndInitializeIfNeeded(userId, timeNow);
    if (service == nullptr) {
        return;
    }
    service->DeleteUninstalledBundleStats(bundleName);
    bundleGroupController_->OnBundleUninstalled(userId, bundleName);
}

void BundleActiveCore::OnStatsChanged(const int userId)
{
    if (!handler_.expired()) {
        BundleActiveReportHandlerObject tmpHandlerObject;
        tmpHandlerObject.userId_ = userId;
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_FLUSH_TO_DISK, handlerobjToPtr);
        if (handler_.lock()->HasInnerEvent(static_cast<int64_t>(userId)) == false) {
            BUNDLE_ACTIVE_LOGI("OnStatsChanged send flush to disk event for user %{public}d", userId);
            handler_.lock()->SendEvent(event, FLUSH_INTERVAL);
        }
    }
}

void BundleActiveCore::RestoreAllData()
{
    for (std::map<int, std::shared_ptr<BundleActiveUserService>>::iterator it = userStatServices_.begin();
        it != userStatServices_.end(); it++) {
        std::shared_ptr<BundleActiveUserService> service = it->second;
        if (service == nullptr) {
            BUNDLE_ACTIVE_LOGI("service in BundleActiveCore::RestoreToDatabaseLocked() is null");
        }
        BUNDLE_ACTIVE_LOGI("userid is %{public}d ", service->userId_);
        service->RestoreStats(true);
        if (bundleGroupController_ != nullptr && bundleGroupController_->bundleUserHistory_ != nullptr) {
            bundleGroupController_->RestoreToDatabase(it->first);
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

void BundleActiveCore::RestoreToDatabase(const int userId)
{
    BUNDLE_ACTIVE_LOGI("RestoreToDatabase called");
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
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

void BundleActiveCore::RestoreToDatabaseLocked(const int userId)
{
    BUNDLE_ACTIVE_LOGI("RestoreToDatabaseLocked called");
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

void BundleActiveCore::ShutDown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    BUNDLE_ACTIVE_LOGI("ShutDown called");
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t timeStamp = timer->GetBootTimeMs();
    BundleActiveEvent event(BundleActiveEvent::SHUTDOWN, timeStamp);
    event.bundleName_ = BundleActiveEvent::DEVICE_EVENT_PACKAGE_NAME;
    std::vector<int> activatedOsAccountIds;
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
    BUNDLE_ACTIVE_LOGI("OnStatsReload called");
    bundleGroupController_->CheckIdleStatsOneTime();
}

void BundleActiveCore::OnSystemUpdate(int userId)
{
}

int64_t BundleActiveCore::CheckTimeChangeAndGetWallTime(int userId)
{
    BUNDLE_ACTIVE_LOGI("CheckTimeChangeAndGetWallTime called, userId is %{public}d", userId);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t actualSystemTime = GetSystemTimeMs();
    int64_t actualRealTime = timer->GetBootTimeMs();
    int64_t expectedSystemTime = (actualRealTime - realTimeShot_) + systemTimeShot_;
    int64_t diffSystemTime = actualSystemTime - expectedSystemTime;
    if (actualSystemTime == -1 || actualRealTime == -1) {
        return -1;
    }
    BUNDLE_ACTIVE_LOGI("asystime is %{public}lld, artime is %{public}lld, esystime is %{public}lld, "
        "diff is %{public}lld",
        actualSystemTime, actualRealTime, expectedSystemTime, diffSystemTime);
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
    BUNDLE_ACTIVE_LOGI("ConvertToSystemTimeLocked called");
    event.timeStamp_ = std::max((int64_t)0, event.timeStamp_ - realTimeShot_) + systemTimeShot_;
}

void BundleActiveCore::OnUserRemoved(const int userId)
{
    BUNDLE_ACTIVE_LOGI("OnUserRemoved called");
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = userStatServices_.find(userId);
    if (it == userStatServices_.end()) {
        return;
    }
    userStatServices_[userId]->OnUserRemoved();
    userStatServices_[userId].reset();
    userStatServices_.erase(userId);
    bundleGroupController_->OnUserRemoved(userId);
}

void BundleActiveCore::OnUserSwitched(const int userId)
{
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    auto it = userStatServices_.find(lastUsedUser_);
    if (it != userStatServices_.end()) {
        if (it != userStatServices_.end()) {
            BUNDLE_ACTIVE_LOGI("restore old user id %{public}d data when switch user", lastUsedUser_);
            BundleActiveEvent event;
            event.eventId_ = BundleActiveEvent::FLUSH;
            int64_t actualRealTime = timer->GetBootTimeMs();
            event.timeStamp_ = (actualRealTime - realTimeShot_) + systemTimeShot_;
            event.abilityId_ = "";
            it->second->ReportEvent(event);
            it->second->RestoreStats(true);
        }
    }
    if (!handler_.expired()) {
        BUNDLE_ACTIVE_LOGI("OnUserSwitched remove flush to disk event");
        handler_.lock()->RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
    }
    std::vector<int> activatedOsAccountIds;
    GetAllActiveUser(activatedOsAccountIds);
    if (activatedOsAccountIds.size() == 0) {
        BUNDLE_ACTIVE_LOGI("query activated account failed, no account activated");
        return;
    }
    for (uint32_t i = 0; i < activatedOsAccountIds.size(); i++) {
        BUNDLE_ACTIVE_LOGI("start to period check for userId %{public}d", activatedOsAccountIds[i]);
        bundleGroupController_->OnUserSwitched(activatedOsAccountIds[i], lastUsedUser_);
    }
    lastUsedUser_ = userId;
}

int BundleActiveCore::ReportEvent(BundleActiveEvent& event, const int userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (userId == 0 || userId == -1) {
        return -1;
    }
    if (lastUsedUser_ == -1) {
        lastUsedUser_ = userId;
        BUNDLE_ACTIVE_LOGI("last used id change to %{public}d", lastUsedUser_);
    }

    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    if (event.bundleName_ == LAUNCHER_BUNDLE_NAME) {
        BUNDLE_ACTIVE_LOGI("launcher event, only update app group");
        bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);
        return 0;
    }

    BUNDLE_ACTIVE_LOGI("report event called  bundle name %{public}s time %{public}lld userId %{public}d, "
        "eventid %{public}d, in lock range", event.bundleName_.c_str(), event.timeStamp_, userId, event.eventId_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == -1) {
        return -1;
    }
    ConvertToSystemTimeLocked(event);
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow);
    if (service == nullptr) {
        BUNDLE_ACTIVE_LOGE("get user data service failed!");
        return -1;
    }
    service->ReportEvent(event);
    bundleGroupController_->ReportEvent(event, bootBasedTimeStamp, userId);
    return 0;
}

int BundleActiveCore::ReportEventToAllUserId(BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("ReportEventToAllUserId called");
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
    if (timeNow == -1) {
        return -1;
    }
    if (userStatServices_.empty()) {
        std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(DEFAULT_USER_ID, timeNow);
    }
    for (std::map<int, std::shared_ptr<BundleActiveUserService>>::iterator it = userStatServices_.begin();
        it != userStatServices_.end(); it++) {
        ConvertToSystemTimeLocked(event);
        std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(it->first, timeNow);
        if (service == nullptr) {
            BUNDLE_ACTIVE_LOGE("get user data service failed!");
            return -1;
        }
        BUNDLE_ACTIVE_LOGI("ReportEventToAllUserId SERVICE user ID IS userId %{public}d",
            service->userId_);
        service->ReportForShutdown(event);
        return 0;
    }
    return 0;
}

std::vector<BundleActivePackageStats> BundleActiveCore::QueryPackageStats(const int userId, const int intervalType,
    const int64_t beginTime, const int64_t endTime, std::string bundleName)
{
    BUNDLE_ACTIVE_LOGI("QueryPackageStats called");
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<BundleActivePackageStats> result;
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == -1) {
        return result;
    }
    BUNDLE_ACTIVE_LOGI("QueryPackageStats begin time is %{public}lld, end time is %{public}lld, "
        "intervaltype is %{public}d", beginTime, endTime, intervalType);
    if (beginTime > timeNow || beginTime >= endTime) {
        BUNDLE_ACTIVE_LOGI("QueryPackageStats time span illegal");
        return result;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow);
    if (service == nullptr) {
        BUNDLE_ACTIVE_LOGI("QueryPackageStats service is null, failed");
        return result;
    }
    result = service->QueryPackageStats(intervalType, beginTime, endTime, userId, bundleName);
    return result;
}

std::vector<BundleActiveEvent> BundleActiveCore::QueryEvents(const int userId, const int64_t beginTime,
    const int64_t endTime, std::string bundleName)
{
    BUNDLE_ACTIVE_LOGI("QueryEvents called");
    std::vector<BundleActiveEvent> result;
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime(userId);
    if (timeNow == -1) {
        return result;
    }
    if (beginTime > timeNow || beginTime >= endTime) {
        return result;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow);
    if (service == nullptr) {
        return result;
    }
    result = service->QueryEvents(beginTime, endTime, userId, bundleName);
    return result;
}

void BundleActiveCore::SetBundleGroup(const std::string& bundleName, const int newGroup, const int userId)
{
    int newReason = GROUP_CONTROL_REASON_FORCED;
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
    bundleGroupController_->SetBundleGroup(bundleName, userId, newGroup, newReason, bootBasedTimeStamp, false);
}

int BundleActiveCore::QueryPackageGroup(const int userId, const std::string bundleName)
{
    return bundleGroupController_->QueryPackageGroup(userId, bundleName);
}

int BundleActiveCore::IsBundleIdle(const std::string& bundleName, const int userId)
{
    return bundleGroupController_->IsBundleIdle(bundleName, userId);
}

void BundleActiveCore::GetAllActiveUser(std::vector<int>& activatedOsAccountIds)
{
    if (AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds) != ERR_OK) {
        BUNDLE_ACTIVE_LOGI("query activated account failed");
        return;
    }
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
}
}

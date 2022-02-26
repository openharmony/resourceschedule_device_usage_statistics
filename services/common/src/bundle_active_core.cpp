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
BundleActiveReportHandlerObject::BundleActiveReportHandlerObject(const BundleActiveReportHandlerObject& orig)
{
    event_.bundleName_ = orig.event_.bundleName_;
    event_.abilityName_ = orig.event_.abilityName_;
    event_.abilityId_ = orig.event_.abilityId_;
    event_.timeStamp_ = orig.event_.timeStamp_;
    event_.eventId_ = orig.event_.eventId_;
    event_.isidle_ = orig.event_.isidle_;
    userId_ = orig.userId_;
    bundleName_ = orig.bundleName_;
}

BundleActiveCore::BundleActiveCore()
{
}

BundleActiveCore::~BundleActiveCore()
{
}

void BundleActiveCommonEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string action = data.GetWant().GetAction();
    BUNDLE_ACTIVE_LOGI("BundleActiveCommonEventSubscriber::OnReceiveEvent action is %{public}s", action.c_str());
    auto want = data.GetWant();
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_OFF ||
        action == CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        if (!activeGroupController_.expired()) {
            sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
            bool isScreenOn = activeGroupController_.lock()->IsScreenOn();
            BUNDLE_ACTIVE_LOGI("BundleActiveCommonEventSubscriber::OnReceiveEvent Screen state changed "
                "received, screen state chante to %{public}d", isScreenOn);
            activeGroupController_.lock()->OnScreenChanged(isScreenOn, timer->GetBootTimeMs());
        }
    } else if (action == CommonEventSupport::COMMON_EVENT_USER_REMOVED) {
        if (!bundleActiveReportHandler_.expired()) {
            int32_t userId = data.GetCode();
            BUNDLE_ACTIVE_LOGI("remove user id %{public}d", userId);
            BundleActiveReportHandlerObject tmpHandlerObject;
            tmpHandlerObject.userId_ = userId;
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_REMOVE_USER, handlerobjToPtr);
            bundleActiveReportHandler_.lock()->SendEvent(event);
        }
    } else if (action == CommonEventSupport::COMMON_EVENT_USER_ADDED) {
        int32_t userId = data.GetCode();
        BUNDLE_ACTIVE_LOGI("BundleActiveCommonEventSubscriber::OnReceiveEvent receive add user event, "
            "user id is %{public}d", userId);
        if (!activeGroupController_.expired() && userId >= 0) {
            activeGroupController_.lock()->PeriodCheckBundleState(userId);
        }
    } else if (action == CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED ||
        action == CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED) {
        int32_t userId = data.GetWant().GetIntParam("userId", 0);
        std::string bundleName = data.GetWant().GetElement().GetBundleName();
        BUNDLE_ACTIVE_LOGI("action is %{public}s, userID is %{public}d, bundlename is %{public}s",
            action.c_str(), userId, bundleName.c_str());
        if (!bundleActiveReportHandler_.expired()) {
            BUNDLE_ACTIVE_LOGI("BundleActiveCommonEventSubscriber::OnReceiveEvent gggggg");
            BundleActiveReportHandlerObject tmpHandlerObject;
            tmpHandlerObject.bundleName_ = bundleName;
            tmpHandlerObject.userId_ = userId;
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            BUNDLE_ACTIVE_LOGI("BundleActiveCommonEventSubscriber::OnReceiveEvent fffff");
            auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_BUNDLE_UNINSTALLED,
                handlerobjToPtr);
            BUNDLE_ACTIVE_LOGI("BundleActiveCommonEventSubscriber::OnReceiveEvent hhhhh");
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
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_USER_ADDED);
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
        systemTimeShot_ = timer->GetWallTimeMs();
    } while (realTimeShot_ == -1 && systemTimeShot_ == -1);
    realTimeShot_ = timer->GetBootTimeMs();
    systemTimeShot_ = timer->GetWallTimeMs();
    bundleGroupController_ = std::make_shared<BundleActiveGroupController>();
    BUNDLE_ACTIVE_LOGI("system time shot is %{public}lld", systemTimeShot_);
}

void BundleActiveCore::InitBundleGroupController()
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::InitBundleGroupController called");
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
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::Init Set group controller and handler done");
    }
    RegisterSubscriber();
    std::vector<OHOS::AccountSA::OsAccountInfo> osAccountInfos;
    GetAllActiveUser(osAccountInfos);
    bundleGroupController_->bundleGroupEnable_ = true;
    for (int i = 0; i < osAccountInfos.size(); i++) {
        bundleGroupController_->PeriodCheckBundleState(osAccountInfos[i].GetLocalId());
    }
}

void BundleActiveCore::SetHandler(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
    handler_ = reportHandler;
}

std::shared_ptr<BundleActiveUserService> BundleActiveCore::GetUserDataAndInitializeIfNeeded(const int userId,
    const int64_t timeStamp)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::GetUserDataAndInitializeIfNeeded called");
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
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::OnBundleUninstalled CALLED");
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
    auto service = GetUserDataAndInitializeIfNeeded(userId, timeNow);
    if (service == nullptr) {
        return;
    }
    service->DeleteUninstalledBundleStats(bundleName);
}

void BundleActiveCore::OnStatsChanged()
{
    auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
    if (!handler_.expired()) {
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::OnStatsChanged send flush to disk event");
        handler_.lock()->SendEvent(event, FLUSH_INTERVAL);
    }
}

void BundleActiveCore::RestoreToDatabase()
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::RestoreToDatabase called");
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    BundleActiveEvent event;
    event.eventId_ = BundleActiveEvent::FLUSH;
    event.timeStamp_ = timer->GetBootTimeMs();
    event.abilityId_ = "";
    ReportEventToAllUserId(event);
    RestoreToDatabaseLocked();
}

void BundleActiveCore::RestoreToDatabaseLocked()
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::RestoreToDatabaseLocked called");
    for (std::map<int, std::shared_ptr<BundleActiveUserService>>::iterator it = userStatServices_.begin();
        it != userStatServices_.end(); it++) {
        std::shared_ptr<BundleActiveUserService> service = it->second;
        if (service == nullptr) {
            BUNDLE_ACTIVE_LOGI("service in BundleActiveCore::RestoreToDatabaseLocked() is null");
        }
        BUNDLE_ACTIVE_LOGI("userid is %{public}d ", service->userId_);
        service->RestoreStats();
        if (bundleGroupController_ != nullptr && bundleGroupController_->bundleUserHistory_ != nullptr) {
            bundleGroupController_->RestoreToDatabase(it->first);
        }
    }
    if (bundleGroupController_ != nullptr) {
        bundleGroupController_->RestoreDurationToDatabase();
    }
    if (!handler_.expired()) {
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::RestoreToDatabaseLocked remove flush to disk event");
        handler_.lock()->RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
    }
}

void BundleActiveCore::ShutDown()
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::ShutDown called");
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t timeStamp = timer->GetBootTimeMs();
    BundleActiveEvent event(BundleActiveEvent::SHUTDOWN, timeStamp);
    event.bundleName_ = BundleActiveEvent::DEVICE_EVENT_PACKAGE_NAME;
    bundleGroupController_->ShutDown(timeStamp);
    ReportEventToAllUserId(event);
    RestoreToDatabaseLocked();
}

void BundleActiveCore::OnStatsReload()
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::OnStatsReload called");
    bundleGroupController_->CheckIdleStatsOneTime();
}

void BundleActiveCore::OnSystemUpdate(int userId)
{
}

int64_t BundleActiveCore::CheckTimeChangeAndGetWallTime()
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::CheckTimeChangeAndGetWallTime called");
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t actualSystemTime = timer->GetWallTimeMs();
    int64_t actualRealTime = timer->GetBootTimeMs();
    int64_t expectedSystemTime = (actualRealTime - realTimeShot_) + systemTimeShot_;
    int64_t diffSystemTime = actualSystemTime - expectedSystemTime;
    if (std::abs(diffSystemTime) > TIME_CHANGE_THRESHOLD_MILLIS) {
        // 时区变换逻辑
        BUNDLE_ACTIVE_LOGI("time changed!");
        for (std::map<int, std::shared_ptr<BundleActiveUserService>>::iterator it = userStatServices_.begin();
            it != userStatServices_.end(); it++) {
            BUNDLE_ACTIVE_LOGI("BundleActiveCore::CheckTimeChangeAndGetWallTime in update time loop");
            std::shared_ptr<BundleActiveUserService> service = it->second;
            service->RenewTableTime(expectedSystemTime, actualSystemTime);
        }
        realTimeShot_ = actualRealTime;
        systemTimeShot_ = actualSystemTime;
    }
    return actualSystemTime;
}

void BundleActiveCore::ConvertToSystemTimeLocked(BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::ConvertToSystemTimeLocked called");
    event.timeStamp_ = std::max((int64_t)0, event.timeStamp_ - realTimeShot_) + systemTimeShot_;
}

void BundleActiveCore::OnUserRemoved(const int userId)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::OnUserRemoved called");
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

int BundleActiveCore::ReportEvent(BundleActiveEvent& event, const int userId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::ReportEvent called");
    BUNDLE_ACTIVE_LOGI("report event called  bundle name %{public}s time %{public}lld userId %{public}d, "
        "eventid %{public}d, in lock range", event.bundleName_.c_str(), event.timeStamp_, userId, event.eventId_);
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
    int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
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
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::ReportEventToAllUserId called");
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
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
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::ReportEventToAllUserId SERVICE user ID IS userId %{public}d",
            service->userId_);
        service->ReportForFlushAndShutdown(event);
        return 0;
    }
    return 0;
}

std::vector<BundleActivePackageStats> BundleActiveCore::QueryPackageStats(const int userId, const int intervalType,
    const int64_t beginTime, const int64_t endTime, std::string bundleName)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::QueryPackageStats called");
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<BundleActivePackageStats> result;
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::QueryPackageStats begin time is %{public}lld, end time is %{public}lld, "
        "intervaltype is %{public}d", beginTime, endTime, intervalType);
    if (beginTime > timeNow || beginTime >= endTime) {
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::QueryPackageStats time span illegal");
        return result;
    }
    std::shared_ptr<BundleActiveUserService> service = GetUserDataAndInitializeIfNeeded(userId, timeNow);
    if (service == nullptr) {
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::QueryPackageStats service is null, failed");
        return result;
    }
    result = service->QueryPackageStats(intervalType, beginTime, endTime, userId, bundleName);
    return result;
}

std::vector<BundleActiveEvent> BundleActiveCore::QueryEvents(const int userId, const int64_t beginTime,
    const int64_t endTime, std::string bundleName)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveCore::QueryEvents called");
    std::vector<BundleActiveEvent> result;
    std::lock_guard<std::mutex> lock(mutex_);
    int64_t timeNow = CheckTimeChangeAndGetWallTime();
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

void BundleActiveCore::GetAllActiveUser(std::vector<OHOS::AccountSA::OsAccountInfo> &osAccountInfos)
{
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::QueryAllCreatedOsAccounts(osAccountInfos);
    if (ret != ERR_OK) {
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::GetAllActiveUser failed");
        return;
    }
    if (osAccountInfos.size() == 0) {
        BUNDLE_ACTIVE_LOGI("BundleActiveCore::GetAllActiveUser size is 0");
        return;
    }
}
}
}
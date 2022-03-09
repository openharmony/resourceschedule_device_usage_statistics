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
#include "power_mgr_client.h"
#include "unistd.h"
#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "bundle_active_event.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
static const int PERIOD_BEST_JS = 0;
static const int PERIOD_YEARLY_JS = 4;
static const int PERIOD_BEST_SERVICE = 4;
static const int DELAY_TIME = 2000;
REGISTER_SYSTEM_ABILITY_BY_ID(BundleActiveService, DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID, true);
using namespace OHOS::Security::AccessToken;
using AccessTokenKit = OHOS::Security::AccessToken::AccessTokenKit;
const std::string NEEDED_PERMISSION = "ohos.permission.BUNDLE_ACTIVE_INFO";

void BundleActiveService::OnStart()
{
    BUNDLE_ACTIVE_LOGI("OnStart() called");
    runner_ = AppExecFwk::EventRunner::Create("device_usage_stats_init_handler");
    if (runner_ == nullptr) {
        BUNDLE_ACTIVE_LOGI("BundleActiveService runner create failed!");
        return;
    }
    handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner_);
    if (handler_ == nullptr) {
        BUNDLE_ACTIVE_LOGI("BundleActiveService handler create failed!");
        return;
    }

    InitNecessaryState();
    int ret = Publish(this);
    if (!ret) {
        BUNDLE_ACTIVE_LOGE("[Server] OnStart, Register SystemAbility[1907] FAIL.");
        return;
    }
    BUNDLE_ACTIVE_LOGI("[Server] OnStart, Register SystemAbility[1907] SUCCESS.");
    return;
}

void BundleActiveService::InitNecessaryState()
{
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr
        || systemAbilityManager->CheckSystemAbility(APP_MGR_SERVICE_ID) == nullptr
        || systemAbilityManager->CheckSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID) == nullptr
        || systemAbilityManager->CheckSystemAbility(POWER_MANAGER_SERVICE_ID) == nullptr
        || systemAbilityManager->CheckSystemAbility(COMMON_EVENT_SERVICE_ID) == nullptr
        || systemAbilityManager->CheckSystemAbility(BACKGROUND_TASK_MANAGER_SERVICE_ID) == nullptr
        || systemAbilityManager->CheckSystemAbility(TIME_SERVICE_ID) == nullptr) {
        BUNDLE_ACTIVE_LOGI("request system service is not ready yet!");
        auto task = [this]() { this->InitNecessaryState(); };
        handler_->PostTask(task, DELAY_TIME);
        return;
    }

    if (bundleActiveCore_ == nullptr) {
        bundleActiveCore_ = std::make_shared<BundleActiveCore>();
        bundleActiveCore_->Init();
    }
    if (reportHandler_ == nullptr) {
        std::string threadName = "bundle_active_report_handler";
        auto runner = AppExecFwk::EventRunner::Create(threadName);
        if (runner == nullptr) {
            BUNDLE_ACTIVE_LOGE("report handler is null");
            return;
        }
        reportHandler_ = std::make_shared<BundleActiveReportHandler>(runner);
        if (reportHandler_ == nullptr) {
            return;
        }
        reportHandler_->Init(bundleActiveCore_);
    }
    if (reportHandler_ != nullptr && bundleActiveCore_ != nullptr) {
        BUNDLE_ACTIVE_LOGI("core and handler is not null");
        bundleActiveCore_->SetHandler(reportHandler_);
    } else {
        return;
    }
    try {
        shutdownCallback_ = new BundleActiveShutdownCallbackService(bundleActiveCore_);
    } catch (const std::bad_alloc &e) {
        BUNDLE_ACTIVE_LOGE("Memory allocation failed");
        return;
    }
    auto& powerManagerClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    powerManagerClient.RegisterShutdownCallback(shutdownCallback_);
    InitAppStateSubscriber(reportHandler_);
    InitContinuousSubscriber(reportHandler_);
    bundleActiveCore_->InitBundleGroupController();
    SubscribeAppState();
    SubscribeContinuousTask();
}

OHOS::sptr<OHOS::AppExecFwk::IAppMgr> BundleActiveService::GetAppManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
    return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(object);
}

void BundleActiveService::InitAppStateSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
    if (appStateObserver_ == nullptr) {
        appStateObserver_ = std::make_shared<BundleActiveAppStateObserver>();
        appStateObserver_->Init(reportHandler);
    }
}

void BundleActiveService::InitContinuousSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
    if (continuousTaskObserver_ == nullptr) {
        continuousTaskObserver_ = std::make_shared<BundleActiveContinuousTaskObserver>();
        continuousTaskObserver_->Init(reportHandler);
    }
}

bool BundleActiveService::SubscribeAppState()
{
    BUNDLE_ACTIVE_LOGI("SubscribeAppState called");
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (appStateObserver_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeAppState appstateobserver is null, return");
        return false;
    }
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_.get());
    if (err != 0) {
        BUNDLE_ACTIVE_LOGE("RegisterApplicationStateObserver failed. err:%{public}d", err);
        appStateObserver_ = nullptr;
        return false;
    }
    BUNDLE_ACTIVE_LOGI("RegisterApplicationStateObserver success.");
    return true;
}

bool BundleActiveService::SubscribeContinuousTask()
{
    if (continuousTaskObserver_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeContinuousTask continuousTaskObserver_ is null, return");
        return false;
    }
    if (OHOS::BackgroundTaskMgr::BackgroundTaskMgrHelper::SubscribeBackgroundTask(*continuousTaskObserver_)
        != OHOS::ERR_OK) {
        BUNDLE_ACTIVE_LOGE("SubscribeBackgroundTask failed.");
        return false;
    }
    return true;
}

void BundleActiveService::OnStop()
{
    if (shutdownCallback_ != nullptr) {
        auto& powerManagerClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
        powerManagerClient.UnRegisterShutdownCallback(shutdownCallback_);
        delete shutdownCallback_;
        shutdownCallback_ = nullptr;
        return;
    }
    BUNDLE_ACTIVE_LOGI("[Server] OnStop");
}


int BundleActiveService::ReportEvent(std::string& bundleName, std::string& abilityName, std::string abilityId,
    const std::string& continuousTask, const int userId, const int eventId)
{
    BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
    tmpHandlerObject.event_.bundleName_ = bundleName;
    tmpHandlerObject.event_.abilityName_ = abilityName;
    tmpHandlerObject.event_.abilityId_ = abilityId;
    tmpHandlerObject.event_.eventId_ = eventId;
    tmpHandlerObject.event_.continuousTaskAbilityName_ = continuousTask;
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
    std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
        std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
    auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
    reportHandler_->SendEvent(event);
    return 0;
}

bool BundleActiveService::IsBundleIdle(const std::string& bundleName)
{
    // get uid
    BUNDLE_ACTIVE_LOGI("Is bundle active called");
    int callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("UID is %{public}d", callingUid);
    // get user id
    int userId = -1;
    int result = -1;
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        result = bundleActiveCore_->IsBundleIdle(bundleName, userId);
    }
    if (result == 0) {
        return false;
    }
    return true;
}

std::vector<BundleActivePackageStats> BundleActiveService::QueryPackageStats(const int intervalType,
    const int64_t beginTime, const int64_t endTime)
{
    BUNDLE_ACTIVE_LOGI("QueryPackageStats stats called, intervaltype is %{public}d",
        intervalType);
    std::vector<BundleActivePackageStats> result;
    // get uid
    int callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryPackageStats UID is %{public}d", callingUid);
    // get userid
    int userId = -1;
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryPackageStats user id is %{public}d", userId);
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, userId);
        if (isSystemAppAndHasPermission == true) {
            int convertedIntervalType = ConvertIntervalType(intervalType);
            result = bundleActiveCore_->QueryPackageStats(userId, convertedIntervalType, beginTime, endTime, "");
        }
    }
    return result;
}

std::vector<BundleActiveEvent> BundleActiveService::QueryEvents(const int64_t beginTime, const int64_t endTime)
{
    BUNDLE_ACTIVE_LOGI("QueryEvents stats called");
    std::vector<BundleActiveEvent> result;
    // get uid
    int callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryEvents UID is %{public}d", callingUid);
    // get userid
    int userId = -1;
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryEvents userid is %{public}d", userId);
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, userId);
        if (isSystemAppAndHasPermission == true) {
            result = bundleActiveCore_->QueryEvents(userId, beginTime, endTime, "");
        }
    }
    return result;
}

void BundleActiveService::SetBundleGroup(const std::string& bundleName, int newGroup, int userId)
{
    bundleActiveCore_->SetBundleGroup(bundleName, newGroup, userId);
}


std::vector<BundleActivePackageStats> BundleActiveService::QueryCurrentPackageStats(const int intervalType,
    const int64_t beginTime, const int64_t endTime)
{
    BUNDLE_ACTIVE_LOGI("QueryCurrentPackageStats stats called");
    std::vector<BundleActivePackageStats> result;
    // get uid
    int callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("UID is %{public}d", callingUid);
    // get userid
    int userId = -1;
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryCurrentPackageStats userid is %{public}d", userId);
        if (!GetBundleMgrProxy()) {
            BUNDLE_ACTIVE_LOGE("BundleActiveGroupController::CheckEachBundleState get bundle manager proxy failed!");
            return result;
        }
        std::string bundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, userId);
        if (!bundleName.empty() && isSystemAppAndHasPermission == true) {
            int convertedIntervalType = ConvertIntervalType(intervalType);
            result = bundleActiveCore_->QueryPackageStats(userId, convertedIntervalType, beginTime, endTime,
                bundleName);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryCurrentPackageStats result size is %{public}d", result.size());
    return result;
}

std::vector<BundleActiveEvent> BundleActiveService::QueryCurrentEvents(const int64_t beginTime,
    const int64_t endTime)
{
    BUNDLE_ACTIVE_LOGI("QueryCurrentEvents stats called");
    std::vector<BundleActiveEvent> result;
    // get uid
    int callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryCurrentEvents UID is %{public}d", callingUid);
    // get userid
    int userId = -1;
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        std::string bundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        if (!bundleName.empty()) {
            BUNDLE_ACTIVE_LOGI("QueryCurrentEvents buindle name is %{public}s",
                bundleName.c_str());
            result = bundleActiveCore_->QueryEvents(userId, beginTime, endTime, bundleName);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryCurrentEvents result size is %{public}d", result.size());
    return result;
}

int BundleActiveService::QueryPackageGroup()
{
    BUNDLE_ACTIVE_LOGI("QueryPackageGroup stats called");
    // get uid
    int callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryPackageGroup UID is %{public}d", callingUid);
    // get userid
    int userId = -1;
    int result = -1;
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(callingUid, userId);
    BUNDLE_ACTIVE_LOGI("QueryPackageGroup user id is %{public}d", userId);
    if (ret == ERR_OK && userId != -1) {
        if (!GetBundleMgrProxy()) {
            BUNDLE_ACTIVE_LOGE("BundleActiveGroupController::CheckEachBundleState get bundle manager proxy failed!");
            return result;
        }
        std::string bundleName = "";
        // get bundle name
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        BUNDLE_ACTIVE_LOGI("QueryPackageGroup bundlename is %{public}s", bundleName.c_str());
        if (!bundleName.empty()) {
            result = bundleActiveCore_->QueryPackageGroup(userId, bundleName);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryPackageGroup result is %{public}d", result);
    return result;
}

bool BundleActiveService::GetBundleMgrProxy()
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

int BundleActiveService::ConvertIntervalType(const int intervalType)
{
    if (intervalType == PERIOD_BEST_JS) {
        return PERIOD_BEST_SERVICE;
    } else if (intervalType > PERIOD_BEST_JS && intervalType <= PERIOD_YEARLY_JS) {
        return intervalType - 1;
    }
    return -1;
}

bool BundleActiveService::CheckBundleIsSystemAppAndHasPermission(const int uid, const int userId)
{
    if (!GetBundleMgrProxy()) {
            BUNDLE_ACTIVE_LOGE("Get bundle manager proxy failed!");
            return false;
        }
        std::string bundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(uid, bundleName);
        bool bundleIsSystemApp = sptrBundleMgr_->CheckIsSystemAppByUid(uid);
        int bundleHasPermission = sptrBundleMgr_->CheckPermissionByUid(bundleName, NEEDED_PERMISSION, userId);
        BUNDLE_ACTIVE_LOGE(" %{public}s is system app %{public}d, "
            "has permission %{public}d", bundleName.c_str(), bundleIsSystemApp, bundleHasPermission);
        if (bundleIsSystemApp == true && bundleHasPermission == 0) {
            return true;
        }
        return false;
}
}
}
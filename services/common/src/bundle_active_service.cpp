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
#include "bundle_active_account_helper.h"
#include "bundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace OHOS::Security;
static const int32_t PERIOD_BEST_JS = 0;
static const int32_t PERIOD_YEARLY_JS = 4;
static const int32_t PERIOD_BEST_SERVICE = 4;
static const int32_t DELAY_TIME = 2000;
static const std::string PERMITTED_PROCESS_NAME = "foundation";
static const int32_t MAXNUM_UP_LIMIT = 1000;
const std::string NEEDED_PERMISSION = "ohos.permission.BUNDLE_ACTIVE_INFO";
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<BundleActiveService>::GetInstance().get());

BundleActiveService::BundleActiveService() : SystemAbility(DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID, true)
{
}

BundleActiveService::~BundleActiveService()
{
}

void BundleActiveService::OnStart()
{
    BUNDLE_ACTIVE_LOGI("OnStart() called");
    if (ready_) {
        BUNDLE_ACTIVE_LOGI("service is ready. nothing to do.");
        return;
    }
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
    auto registerTask = [this]() { this->InitNecessaryState(); };
    handler_->PostSyncTask(registerTask);
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

    if (systemAbilityManager->GetSystemAbility(APP_MGR_SERVICE_ID) == nullptr
        || systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID) == nullptr
        || systemAbilityManager->GetSystemAbility(POWER_MANAGER_SERVICE_ID) == nullptr
        || systemAbilityManager->GetSystemAbility(COMMON_EVENT_SERVICE_ID) == nullptr
        || systemAbilityManager->GetSystemAbility(BACKGROUND_TASK_MANAGER_SERVICE_ID) == nullptr
        || systemAbilityManager->GetSystemAbility(TIME_SERVICE_ID) == nullptr) {
        BUNDLE_ACTIVE_LOGI("request system service object is not ready yet!");
        auto task = [this]() { this->InitNecessaryState(); };
        handler_->PostTask(task, DELAY_TIME);
        return;
    }
    InitService();
    ready_ = true;
    int32_t ret = Publish(DelayedSingleton<BundleActiveService>::GetInstance().get());
    if (!ret) {
        BUNDLE_ACTIVE_LOGE("[Server] OnStart, Register SystemAbility[1907] FAIL.");
        return;
    }
    BUNDLE_ACTIVE_LOGI("[Server] OnStart, Register SystemAbility[1907] SUCCESS.");
}

void BundleActiveService::InitService()
{
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
    shutdownCallback_ = new (std::nothrow) BundleActiveShutdownCallbackService(bundleActiveCore_);
    powerStateCallback_ = new (std::nothrow) BundleActivePowerStateCallbackService(bundleActiveCore_);
    auto& powerManagerClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    if (shutdownCallback_) {
        powerManagerClient.RegisterShutdownCallback(shutdownCallback_);
    }
    if (powerStateCallback_) {
        powerManagerClient.RegisterPowerStateCallback(powerStateCallback_);
    }
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
    if (!object) {
        return nullptr;
    }
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
    if (appStateObserver_ == nullptr || appManager == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeAppState appstateobserver is null, return");
        return false;
    }
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_.get());
    if (err != 0) {
        BUNDLE_ACTIVE_LOGE("RegisterApplicationStateObserver failed. err:%{public}d", err);
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
        powerManagerClient.UnRegisterPowerStateCallback(powerStateCallback_);
        return;
    }
    BUNDLE_ACTIVE_LOGI("[Server] OnStop");
    ready_ = false;
}

int32_t BundleActiveService::ReportEvent(BundleActiveEvent& event, const int32_t userId)
{
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if ((AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) == AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE)) {
        AccessToken::NativeTokenInfo callingTokenInfo;
        AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenId, callingTokenInfo);
        int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
        BUNDLE_ACTIVE_LOGI("calling process name is %{public}s, uid is %{public}d",
            callingTokenInfo.processName.c_str(), callingUid);
        if (callingTokenInfo.processName == PERMITTED_PROCESS_NAME) {
            BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
            tmpHandlerObject.event_ = event;
            sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
            tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
            reportHandler_->SendEvent(event);
            return 0;
        } else {
            BUNDLE_ACTIVE_LOGE("token does not belong to fms service process, return");
            return -1;
        }
    } else {
        BUNDLE_ACTIVE_LOGE("token does not belong to native process, return");
        return -1;
    }
}

bool BundleActiveService::IsBundleIdle(const std::string& bundleName)
{
    // get uid
    BUNDLE_ACTIVE_LOGI("Is bundle active called");
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("UID is %{public}d", callingUid);
    // get user id
    int32_t userId = -1;
    int32_t result = -1;
    OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        result = bundleActiveCore_->IsBundleIdle(bundleName, userId);
    }
    if (result == 0) {
        return false;
    }
    return true;
}

std::vector<BundleActivePackageStats> BundleActiveService::QueryPackageStats(const int32_t intervalType,
    const int64_t beginTime, const int64_t endTime, int32_t& errCode, int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("QueryPackageStats stats called, intervaltype is %{public}d", intervalType);
    std::vector<BundleActivePackageStats> result;
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    BUNDLE_ACTIVE_LOGI("QueryPackageStats UID is %{public}d", callingUid);
    if (userId == -1) {
        // get userid
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return result;
        }
    }
    if (userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryPackageStats user id is %{public}d", userId);
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode);
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        if (isSystemAppAndHasPermission == true ||
            AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) ==
            AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
            errCode = 0;
            int32_t convertedIntervalType = ConvertIntervalType(intervalType);
            result = bundleActiveCore_->QueryPackageStats(userId, convertedIntervalType, beginTime, endTime, "");
        }
    }
    return result;
}

std::vector<BundleActiveEvent> BundleActiveService::QueryEvents(const int64_t beginTime,
    const int64_t endTime, int32_t& errCode, int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("QueryEvents stats called");
    std::vector<BundleActiveEvent> result;
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    BUNDLE_ACTIVE_LOGI("QueryEvents UID is %{public}d", callingUid);
    if (userId == -1) {
        // get userid
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return result;
        }
    }
    if (userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryEvents userid is %{public}d", userId);
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode);
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        if (isSystemAppAndHasPermission == true ||
            AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) ==
            AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
            errCode = 0;
            result = bundleActiveCore_->QueryEvents(userId, beginTime, endTime, "");
        }
    }
    return result;
}

int32_t BundleActiveService::SetBundleGroup(const std::string& bundleName, int32_t newGroup, int32_t errCode,
    int32_t userId)
{
    bool result = -1;
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (userId == -1) {
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return result;
        }
    }
    if (userId != -1) {
        BUNDLE_ACTIVE_LOGI("SetBundleGroup userid is %{public}d", userId);
        if (CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode) ||
            AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) == AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
            result = bundleActiveCore_->SetBundleGroup(bundleName, newGroup, userId);
        }
    }
    return result;
}


std::vector<BundleActivePackageStats> BundleActiveService::QueryCurrentPackageStats(const int32_t intervalType,
    const int64_t beginTime, const int64_t endTime)
{
    BUNDLE_ACTIVE_LOGI("QueryCurrentPackageStats stats called");
    std::vector<BundleActivePackageStats> result;
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    BUNDLE_ACTIVE_LOGI("UID is %{public}d", callingUid);
    // get userid
    int32_t userId = -1;
    OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryCurrentPackageStats userid is %{public}d", userId);
        if (!GetBundleMgrProxy()) {
            BUNDLE_ACTIVE_LOGE("get bundle manager proxy failed!");
            return result;
        }
        std::string bundleName = "";
        int32_t errCode = 0;
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode);
        if (!bundleName.empty() && isSystemAppAndHasPermission == true) {
            int32_t convertedIntervalType = ConvertIntervalType(intervalType);
            result = bundleActiveCore_->QueryPackageStats(userId, convertedIntervalType, beginTime, endTime,
                bundleName);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryCurrentPackageStats result size is %{public}zu", result.size());
    return result;
}

std::vector<BundleActiveEvent> BundleActiveService::QueryCurrentEvents(const int64_t beginTime,
    const int64_t endTime)
{
    BUNDLE_ACTIVE_LOGI("QueryCurrentEvents stats called");
    std::vector<BundleActiveEvent> result;
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryCurrentEvents UID is %{public}d", callingUid);
    // get userid
    int32_t userId = -1;
    OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        if (!GetBundleMgrProxy()) {
            BUNDLE_ACTIVE_LOGE("get bundle manager proxy failed!");
            return result;
        }
        std::string bundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        if (!bundleName.empty()) {
            BUNDLE_ACTIVE_LOGI("QueryCurrentEvents buindle name is %{public}s",
                bundleName.c_str());
            result = bundleActiveCore_->QueryEvents(userId, beginTime, endTime, bundleName);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryCurrentEvents result size is %{public}zu", result.size());
    return result;
}

int32_t BundleActiveService::QueryPackageGroup(std::string& bundleName, int32_t userId)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("QueryPackageGroup UID is %{public}d", callingUid);
    int32_t errCode = 0;
    int32_t result = -1;
    if (userId == -1) {
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return result;
        }
    }
    if (userId != -1) {
        if (bundleName.empty()) {
            if (!GetBundleMgrProxy()) {
                BUNDLE_ACTIVE_LOGE("get bundle manager proxy failed!");
                return result;
            }
            std::string g_bundleName = "";
            sptrBundleMgr_->GetBundleNameForUid(callingUid, g_bundleName);
            bundleName = g_bundleName;
            result = bundleActiveCore_->QueryPackageGroup(bundleName, userId);
        } else {
            AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
            if (CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode) ||
                AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) ==
                AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
                    result = bundleActiveCore_->QueryPackageGroup(bundleName, userId);
            }
        }
    }
    return result;
}

int32_t BundleActiveService::RegisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer)
{
    int result = -1;
    if (!bundleActiveCore_) {
        return result;
    }
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    int32_t errCode = 0;
    if (CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode) ||
    AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) == AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
        result = bundleActiveCore_->RegisterGroupCallBack(tokenId, observer);
    }
    return result;
}

int32_t BundleActiveService::UnregisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer)
{
    int32_t result = -1;
    if (!bundleActiveCore_) {
        return result;
    }

    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    int32_t errCode = 0;
    if (CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode) ||
        AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) == AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
        result = bundleActiveCore_->UnregisterGroupCallBack(tokenId, observer);
    }
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

int32_t BundleActiveService::ConvertIntervalType(const int32_t intervalType)
{
    if (intervalType == PERIOD_BEST_JS) {
        return PERIOD_BEST_SERVICE;
    } else if (intervalType > PERIOD_BEST_JS && intervalType <= PERIOD_YEARLY_JS) {
        return intervalType - 1;
    }
    return -1;
}

bool BundleActiveService::CheckBundleIsSystemAppAndHasPermission(const int32_t uid,
    OHOS::Security::AccessToken::AccessTokenID tokenId, int32_t& errCode)
{
    if (!GetBundleMgrProxy()) {
            BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack Get bundle manager proxy failed!");
            return false;
    }
    std::string bundleName = "";
    sptrBundleMgr_->GetBundleNameForUid(uid, bundleName);

    int32_t bundleHasPermission = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, NEEDED_PERMISSION);
    if (bundleHasPermission != 0) {
        errCode = bundleHasPermission;
        BUNDLE_ACTIVE_LOGE("UnRegisterGroupCallBack %{public}s hasn't permission", bundleName.c_str());
        return false;
    } else {
        BUNDLE_ACTIVE_LOGI("UnRegisterGroupCallBack %{public}s has permission %{public}d",
            bundleName.c_str(), bundleHasPermission);
        return true;
    }
}

int32_t BundleActiveService::QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
    int32_t userId)
{
    int32_t errCode = 0;
    if (maxNum > MAXNUM_UP_LIMIT || maxNum <= 0) {
        BUNDLE_ACTIVE_LOGE("MaxNum is Invalid!");
        errCode = -1;
        return errCode;
    }
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryFormStatistics UID is %{public}d", callingUid);
    // get userid when userId is -1
    if (userId == -1) {
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return errCode;
        }
    }
    if (userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryFormStatistics userid is %{public}d", userId);
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        bool isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode);
        if (isSystemAppAndHasPermission == true ||
            AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) ==
            AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
            errCode = bundleActiveCore_->QueryFormStatistics(maxNum, results, userId);
            for (auto& oneResult : results) {
                QueryModuleRecordInfos(oneResult);
            }
        }
    }
    return errCode;
}

int32_t BundleActiveService::QueryEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryEventStats UID is %{public}d", callingUid);
    // get userid when userId is -1
    int32_t errCode = 0;
    if (userId == -1) {
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return errCode;
        }
    }
    if (userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryEventStats userid is %{public}d", userId);
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        if ((CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode)) ||
            (AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) ==
            AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE)) {
            errCode = bundleActiveCore_->QueryEventStats(beginTime, endTime, eventStats, userId);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryEventStats result size is %{public}zu", eventStats.size());
    return errCode;
}

int32_t BundleActiveService::QueryAppNotificationNumber(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGI("QueryAppNotificationNumber UID is %{public}d", callingUid);
    // get userid when userId is -1
    int32_t errCode = 0;
    if (userId == -1) {
        OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK) {
            errCode = -1;
            return errCode;
        }
    }
    if (userId != -1) {
        BUNDLE_ACTIVE_LOGI("QueryAppNotificationNumber userid is %{public}d", userId);
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        if ((CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId, errCode)) ||
            (AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId) ==
            AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE)) {
            errCode = bundleActiveCore_->QueryAppNotificationNumber(beginTime, endTime, eventStats, userId);
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryAppNotificationNumber result size is %{public}zu", eventStats.size());
    return errCode;
}

void BundleActiveService::QueryModuleRecordInfos(BundleActiveModuleRecord& moduleRecord)
{
    if (!GetBundleMgrProxy()) {
        return;
    }
    ApplicationInfo appInfo;
    if (!sptrBundleMgr_->GetApplicationInfo(moduleRecord.bundleName_, ApplicationFlag::GET_BASIC_APPLICATION_INFO,
        moduleRecord.userId_, appInfo)) {
        BUNDLE_ACTIVE_LOGW("GetApplicationInfo failed!");
        return;
    }
    BundleInfo bundleInfo;
    if (!sptrBundleMgr_->GetBundleInfo(moduleRecord.bundleName_, BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO,
        bundleInfo, moduleRecord.userId_)) {
        BUNDLE_ACTIVE_LOGW("GetBundleInfo failed!");
        return;
    }
    for (const auto & oneModuleInfo : bundleInfo.hapModuleInfos) {
        if (oneModuleInfo.moduleName == moduleRecord.moduleName_) {
            std::string mainAbility = oneModuleInfo.mainAbility;
            for (auto oneAbilityInfo : oneModuleInfo.abilityInfos) {
                if (oneAbilityInfo.type != AbilityType::PAGE) {
                    continue;
                }
                if (mainAbility.empty() || mainAbility.compare(oneAbilityInfo.name) == 0) {
                    SerModuleProperties(oneModuleInfo, appInfo, oneAbilityInfo, moduleRecord);
                    break;
                }
            }
        }
    }
}

void BundleActiveService::SerModuleProperties(const HapModuleInfo& hapModuleInfo,
    const ApplicationInfo& appInfo, const AbilityInfo& abilityInfo, BundleActiveModuleRecord& moduleRecord)
{
    moduleRecord.deviceId_ = appInfo.deviceId;
    moduleRecord.abilityName_ = abilityInfo.name;
    moduleRecord.appLabelId_ = appInfo.labelId;
    moduleRecord.labelId_ = static_cast<uint32_t>(hapModuleInfo.labelId);
    moduleRecord.abilityLableId_ = abilityInfo.labelId;
    moduleRecord.descriptionId_ = abilityInfo.descriptionId;
    moduleRecord.abilityIconId_ = abilityInfo.iconId;
    moduleRecord.installFreeSupported_ = hapModuleInfo.installationFree;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


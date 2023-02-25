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
#include "bundle_mgr_proxy.h"
#include "unistd.h"
#include "accesstoken_kit.h"

#include "bundle_state_inner_errors.h"
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
const int32_t EVENTS_PARAM = 5;
static constexpr int32_t NO_DUMP_PARAM_NUMS = 0;
const int32_t PACKAGE_USAGE_PARAM = 6;
const int32_t MODULE_USAGE_PARAM = 4;
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
    std::set<int32_t> serviceIdSets{
        APP_MGR_SERVICE_ID, BUNDLE_MGR_SERVICE_SYS_ABILITY_ID, POWER_MANAGER_SERVICE_ID, COMMON_EVENT_SERVICE_ID,
        BACKGROUND_TASK_MANAGER_SERVICE_ID, TIME_SERVICE_ID,
    };
    sptr<ISystemAbilityManager> systemAbilityManager
        = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        BUNDLE_ACTIVE_LOGI("GetSystemAbilityManager fail!");
        auto task = [this]() { this->InitNecessaryState(); };
        handler_->PostTask(task, DELAY_TIME);
        return;
    }
    for (const auto& serviceItem : serviceIdSets) {
        auto checkResult = systemAbilityManager->CheckSystemAbility(serviceItem);
        if (!checkResult) {
            BUNDLE_ACTIVE_LOGI("request system service is not ready yet!");
            auto task = [this]() { this->InitNecessaryState(); };
            handler_->PostTask(task, DELAY_TIME);
            return;
        }
    }

    for (const auto& serviceItem : serviceIdSets) {
        auto getAbility = systemAbilityManager->GetSystemAbility(serviceItem);
        if (!getAbility) {
            BUNDLE_ACTIVE_LOGI("request system service object is not ready yet!");
            auto task = [this]() { this->InitNecessaryState(); };
            handler_->PostTask(task, DELAY_TIME);
            return;
        }
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
    if (!appStateObserver_) {
        appStateObserver_ = new (std::nothrow)BundleActiveAppStateObserver();
        if (!appStateObserver_) {
            BUNDLE_ACTIVE_LOGE("malloc app state observer failed");
            return;
        }
        appStateObserver_->Init(reportHandler);
    }
}

void BundleActiveService::InitContinuousSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
#ifdef BGTASKMGR_ENABLE
    if (continuousTaskObserver_ == nullptr) {
        continuousTaskObserver_ = std::make_shared<BundleActiveContinuousTaskObserver>();
        continuousTaskObserver_->Init(reportHandler);
    }
#endif
}

bool BundleActiveService::SubscribeAppState()
{
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
    if (appStateObserver_ == nullptr || appManager == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeAppState appstateobserver is null, return");
        return false;
    }
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_);
    if (err != 0) {
        BUNDLE_ACTIVE_LOGE("RegisterApplicationStateObserver failed. err:%{public}d", err);
        return false;
    }
    BUNDLE_ACTIVE_LOGD("RegisterApplicationStateObserver success.");
    return true;
}

bool BundleActiveService::SubscribeContinuousTask()
{
#ifdef BGTASKMGR_ENABLE
    if (continuousTaskObserver_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeContinuousTask continuousTaskObserver_ is null, return");
        return false;
    }
    ErrCode errCode = BackgroundTaskMgr::BackgroundTaskMgrHelper::SubscribeBackgroundTask(*continuousTaskObserver_);
    if (errCode != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("SubscribeBackgroundTask failed.");
        return false;
    }
#endif
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

ErrCode BundleActiveService::ReportEvent(BundleActiveEvent& event, const int32_t userId)
{
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (CheckNativePermission(tokenId) == ERR_OK) {
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
            return ERR_OK;
        } else {
            BUNDLE_ACTIVE_LOGE("token does not belong to fms service process, return");
            return ERR_PERMISSION_DENIED;
        }
    } else {
        BUNDLE_ACTIVE_LOGE("token does not belong to native process, return");
        return ERR_PERMISSION_DENIED;
    }
}

ErrCode BundleActiveService::IsBundleIdle(bool& isBundleIdle, const std::string& bundleName, int32_t userId)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ErrCode ret = GetBundleMgrProxy();
    if (ret != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("IsBundleIdle Get bundle manager proxy failed!");
        return ret;
    }
    std::string callingBundleName = "";
    sptrBundleMgr_->GetBundleNameForUid(callingUid, callingBundleName);
    BUNDLE_ACTIVE_LOGI("UID is %{public}d, bundle name is %{public}s", callingUid, callingBundleName.c_str());
    // get user id
    int32_t result = -1;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
    }

    if (callingBundleName == bundleName) {
        if (!sptrBundleMgr_->CheckIsSystemAppByUid(callingUid)) {
            BUNDLE_ACTIVE_LOGE("%{public}s is not sys app", bundleName.c_str());
            return ERR_NOT_SYSTEM_APP;
        }
        BUNDLE_ACTIVE_LOGI("%{public}s check its own idle state", bundleName.c_str());
        result = bundleActiveCore_->IsBundleIdle(bundleName, userId);
    } else {
        ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
        if (ret == ERR_OK) {
            result = bundleActiveCore_->IsBundleIdle(bundleName, userId);
        } else {
            return ret;
        }
    }
    if (result == 0 || result == -1) {
        isBundleIdle = false;
    } else {
        isBundleIdle = true;
    }
    return ERR_OK;
}

ErrCode BundleActiveService::QueryBundleStatsInfoByInterval(std::vector<BundleActivePackageStats>& PackageStats,
    const int32_t intervalType, const int64_t beginTime, const int64_t endTime, int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfoByInterval stats called, intervaltype is %{public}d", intervalType);
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ErrCode ret = ERR_OK;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryBundleStatsInfos user id is %{public}d", userId);
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret == ERR_OK) {
        int32_t convertedIntervalType = ConvertIntervalType(intervalType);
        ret = bundleActiveCore_->QueryBundleStatsInfos(
            PackageStats, userId, convertedIntervalType, beginTime, endTime, "");
    }
    return ret;
}

ErrCode BundleActiveService::QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents,
    const int64_t beginTime, const int64_t endTime, int32_t userId)
{
    ErrCode ret = ERR_OK;
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryBundleEvents userid is %{public}d", userId);
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->QueryBundleEvents(bundleActiveEvents, userId, beginTime, endTime, "");
        BUNDLE_ACTIVE_LOGI("QueryBundleEvents result is %{public}zu", bundleActiveEvents.size());
    }
    return ret;
}

ErrCode BundleActiveService::SetAppGroup(const std::string& bundleName, int32_t newGroup, int32_t userId)
{
    ErrCode ret = ERR_OK;
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    bool isFlush = false;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK || userId == -1) {
            return ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        }
        isFlush = true;
    }
    BUNDLE_ACTIVE_LOGI("SetAppGroup userId is %{public}d", userId);

    ret = GetBundleMgrProxy();
    if (ret != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("SetAppGroup Get bundle manager proxy failed!");
        return ret;
    }
    std::string localBundleName = "";
    sptrBundleMgr_->GetBundleNameForUid(callingUid, localBundleName);
    if (localBundleName == bundleName) {
        BUNDLE_ACTIVE_LOGI("SetAppGroup can not set its bundleName");
        return ERR_PERMISSION_DENIED;
    }
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->SetAppGroup(bundleName, newGroup, userId, isFlush);
    }
    return ret;
}

ErrCode BundleActiveService::QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& bundleActivePackageStats,
    const int32_t intervalType, const int64_t beginTime, const int64_t endTime)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    BUNDLE_ACTIVE_LOGD("UID is %{public}d", callingUid);
    // get userid
    int32_t userId = -1;
    ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfos userid is %{public}d", userId);
        ret = GetBundleMgrProxy();
        if (ret != ERR_OK) {
            BUNDLE_ACTIVE_LOGE("QueryBundleStatsInfos Get bundle manager proxy failed!");
            return ret;
        }
        std::string bundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        ErrCode isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId);
        if (!bundleName.empty() && isSystemAppAndHasPermission == ERR_OK) {
            int32_t convertedIntervalType = ConvertIntervalType(intervalType);
            ret = bundleActiveCore_->QueryBundleStatsInfos(bundleActivePackageStats, userId, convertedIntervalType,
                beginTime, endTime, bundleName);
        }
    }
    return ret;
}

ErrCode BundleActiveService::QueryCurrentBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents,
    const int64_t beginTime, const int64_t endTime)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents UID is %{public}d", callingUid);
    // get userid
    int32_t userId = -1;
    ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
    if (ret == ERR_OK && userId != -1) {
        ret = GetBundleMgrProxy();
        if (ret != ERR_OK) {
            BUNDLE_ACTIVE_LOGE("QueryCurrentBundleEvents Get bundle manager proxy failed!");
            return ret;
        }
        std::string bundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(callingUid, bundleName);
        if (!sptrBundleMgr_->CheckIsSystemAppByUid(callingUid)) {
            BUNDLE_ACTIVE_LOGE("%{public}s is not sys app", bundleName.c_str());
            return ERR_NOT_SYSTEM_APP;
        }
        if (!bundleName.empty()) {
            BUNDLE_ACTIVE_LOGI("QueryCurrentBundleEvents buindle name is %{public}s",
                bundleName.c_str());
            ret = bundleActiveCore_->QueryBundleEvents(bundleActiveEvents, userId, beginTime, endTime, bundleName);
        }
    }
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents bundleActiveEvents size is %{public}zu", bundleActiveEvents.size());
    return ret;
}

ErrCode BundleActiveService::QueryAppGroup(int32_t& appGroup, std::string& bundleName, int32_t userId)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("QueryAppGroup UID is %{public}d", callingUid);
    ErrCode ret = ERR_OK;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (ret != ERR_OK || userId == -1) {
            return ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        }
    }
    if (bundleName.empty()) {
        ret = GetBundleMgrProxy();
        if (ret != ERR_OK) {
            BUNDLE_ACTIVE_LOGE("QueryAppGroup Get bundle manager proxy failed!");
            return ret;
        }
        std::string localBundleName = "";
        sptrBundleMgr_->GetBundleNameForUid(callingUid, localBundleName);
        if (!sptrBundleMgr_->CheckIsSystemAppByUid(callingUid)) {
            BUNDLE_ACTIVE_LOGE("%{public}s is not sys app", localBundleName.c_str());
            return ERR_NOT_SYSTEM_APP;
        }
        bundleName = localBundleName;
        ret = bundleActiveCore_->QueryAppGroup(appGroup, bundleName, userId);
    } else {
        AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
        ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
        if (ret == ERR_OK) {
            ret = bundleActiveCore_->QueryAppGroup(appGroup, bundleName, userId);
        }
    }
    return ret;
}

ErrCode BundleActiveService::RegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer)
{
    if (!bundleActiveCore_) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ErrCode ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->RegisterAppGroupCallBack(tokenId, observer);
    }
    return ret;
}

ErrCode BundleActiveService::UnRegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer)
{
    if (!bundleActiveCore_) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ErrCode ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->UnRegisterAppGroupCallBack(tokenId, observer);
    }
    return ret;
}

ErrCode BundleActiveService::GetBundleMgrProxy()
{
    if (!sptrBundleMgr_) {
        sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (!systemAbilityManager) {
            BUNDLE_ACTIVE_LOGE("Failed to get system ability mgr.");
            return ERR_GET_SYSTEM_ABILITY_MANAGER_FAILED;
        }
        sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
        if (!remoteObject) {
            BUNDLE_ACTIVE_LOGE("Failed to get bundle manager service.");
            return ERR_GET_SYSTEM_ABILITY_FAILED;
        }
        sptrBundleMgr_ = iface_cast<IBundleMgr>(remoteObject);
        if (!sptrBundleMgr_) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability, sptrBundleMgr_");
            return ERR_REMOTE_OBJECT_IF_CAST_FAILED;
        }
        auto object = sptrBundleMgr_->AsObject();
        if (!object) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability, sptrBundleMgr_->AsObject()");
            return ERR_REMOTE_OBJECT_IF_CAST_FAILED;
        }
    }
    return ERR_OK;
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

ErrCode BundleActiveService::CheckBundleIsSystemAppAndHasPermission(const int32_t uid,
    OHOS::Security::AccessToken::AccessTokenID tokenId)
{
    ErrCode errCode = GetBundleMgrProxy();
    if (errCode != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("CheckBundleIsSystemAppAndHasPermission Get bundle manager proxy failed!");
        return errCode;
    }
    std::string bundleName = "";
    sptrBundleMgr_->GetBundleNameForUid(uid, bundleName);

    int32_t bundleHasPermission = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, NEEDED_PERMISSION);
    if (bundleHasPermission != 0) {
        BUNDLE_ACTIVE_LOGE("%{public}s hasn't permission", bundleName.c_str());
        return ERR_PERMISSION_DENIED;
    }
    BUNDLE_ACTIVE_LOGI("%{public}s has permission", bundleName.c_str());
    return ERR_OK;
}

ErrCode BundleActiveService::CheckNativePermission(OHOS::Security::AccessToken::AccessTokenID tokenId)
{
    auto tokenFlag = AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenFlag == AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
        return ERR_OK;
    }
    if (tokenFlag == AccessToken::TypeATokenTypeEnum::TOKEN_SHELL) {
        return ERR_OK;
    }
    return ERR_PERMISSION_DENIED;
}

ErrCode BundleActiveService::CheckSystemAppOrNativePermission(const int32_t uid,
    OHOS::Security::AccessToken::AccessTokenID tokenId)
{
    if (CheckBundleIsSystemAppAndHasPermission(uid, tokenId) == ERR_OK) {
        return ERR_OK;
    }
    return CheckNativePermission(tokenId);
}

ErrCode BundleActiveService::QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
    int32_t userId)
{
    ErrCode errCode = ERR_OK;
    if (maxNum > MAXNUM_UP_LIMIT || maxNum <= 0) {
        BUNDLE_ACTIVE_LOGE("MaxNum is Invalid!");
        return ERR_FIND_APP_USAGE_RECORDS_FAILED;
    }
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    if (userId == -1) {
        errCode = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (errCode != ERR_OK || userId == -1) {
            return errCode;
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryModuleUsageRecords userid is %{public}d", userId);
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    errCode = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (errCode == ERR_OK) {
        errCode = bundleActiveCore_->QueryModuleUsageRecords(maxNum, results, userId);
        for (auto& oneResult : results) {
            QueryModuleRecordInfos(oneResult);
        }
    }
    return errCode;
}

ErrCode BundleActiveService::QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    ErrCode errCode = ERR_OK;
    if (userId == -1) {
        errCode = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (errCode != ERR_OK || userId == -1) {
            return errCode;
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryDeviceEventStats userid is %{public}d", userId);
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    errCode = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (errCode == ERR_OK) {
        errCode = bundleActiveCore_->QueryDeviceEventStats(beginTime, endTime, eventStats, userId);
    }
    BUNDLE_ACTIVE_LOGD("QueryDeviceEventStats result size is %{public}zu", eventStats.size());
    return errCode;
}

ErrCode BundleActiveService::QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("QueryNotificationEventStats UID is %{public}d", callingUid);
    // get userid when userId is -1
    ErrCode errCode = ERR_OK;
    if (userId == -1) {
        errCode = BundleActiveAccountHelper::GetUserId(callingUid, userId);
        if (errCode != ERR_OK || userId == -1) {
            return errCode;
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryNotificationEventStats userid is %{public}d", userId);
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    errCode = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (errCode == ERR_OK) {
        errCode = bundleActiveCore_->QueryNotificationEventStats(beginTime, endTime, eventStats, userId);
    }
    return errCode;
}

void BundleActiveService::QueryModuleRecordInfos(BundleActiveModuleRecord& moduleRecord)
{
    ErrCode errCode = GetBundleMgrProxy();
    if (errCode != ERR_OK) {
        return;
    }
    ApplicationInfo appInfo;
    bool getInfoIsSuccess = sptrBundleMgr_->GetApplicationInfo(moduleRecord.bundleName_,
        ApplicationFlag::GET_BASIC_APPLICATION_INFO, moduleRecord.userId_, appInfo);
    if (!getInfoIsSuccess) {
        BUNDLE_ACTIVE_LOGE("GetApplicationInfo failed!");
        return;
    }
    BundleInfo bundleInfo;
    getInfoIsSuccess = sptrBundleMgr_->GetBundleInfo(moduleRecord.bundleName_,
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, moduleRecord.userId_);
    if (!getInfoIsSuccess) {
        BUNDLE_ACTIVE_LOGE("GetBundleInfo failed!");
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

int32_t BundleActiveService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    std::vector<std::string> argsInStr;
    std::transform(args.begin(), args.end(), std::back_inserter(argsInStr),
        [](const std::u16string &arg) {
        return Str16ToStr8(arg);
    });
    std::string result;
    int32_t ret = ERR_OK;
    if (argsInStr.size() == NO_DUMP_PARAM_NUMS) {
        DumpUsage(result);
    } else {
        std::vector<std::string> infos;
        if (argsInStr[0] == "-h") {
            DumpUsage(result);
        } else if (argsInStr[0] == "-A") {
            ret = ShellDump(argsInStr, infos);
        } else {
            infos.emplace_back("BundleActiveService Error params.\n");
            ret = ERR_USAGE_STATS_INVALID_PARAM;
        }
        for (auto info : infos) {
            result.append(info);
        }
    }
    if (!SaveStringToFd(fd, result)) {
        BUNDLE_ACTIVE_LOGE("BundleActiveService dump save string to fd failed!");
        ret = ERR_USAGE_STATS_METHOD_CALLED_FAILED;
    }
    return ret;
}

int32_t BundleActiveService::ShellDump(const std::vector<std::string> &dumpOption, std::vector<std::string> &dumpInfo)
{
    int32_t ret = -1;

    if (dumpOption[1] == "Events") {
        std::vector<BundleActiveEvent> eventResult;
        if (static_cast<int32_t>(dumpOption.size()) != EVENTS_PARAM) {
            return ret;
        }
        int64_t beginTime = std::stoll(dumpOption[2]);
        int64_t endTime = std::stoll(dumpOption[3]);
        int32_t userId = std::stoi(dumpOption[4]);
        this->QueryBundleEvents(eventResult, beginTime, endTime, userId);
        for (auto& oneEvent : eventResult) {
            dumpInfo.emplace_back(oneEvent.ToString());
        }
    } else if (dumpOption[1] == "PackageUsage") {
        std::vector<BundleActivePackageStats> packageUsageResult;
        if (static_cast<int32_t>(dumpOption.size()) != PACKAGE_USAGE_PARAM) {
            return ret;
        }
        int32_t intervalType = std::stoi(dumpOption[2]);
        int64_t beginTime = std::stoll(dumpOption[3]);
        int64_t endTime = std::stoll(dumpOption[4]);
        int32_t userId = std::stoi(dumpOption[5]);
        this->QueryBundleStatsInfoByInterval(packageUsageResult, intervalType, beginTime, endTime, userId);
        for (auto& onePackageRecord : packageUsageResult) {
            dumpInfo.emplace_back(onePackageRecord.ToString());
        }
    } else if (dumpOption[1] == "ModuleUsage") {
        std::vector<BundleActiveModuleRecord> moduleResult;
        if (static_cast<int32_t>(dumpOption.size()) != MODULE_USAGE_PARAM) {
            return ret;
        }
        int32_t maxNum = std::stoi(dumpOption[2]);
        int32_t userId = std::stoi(dumpOption[3]);
        BUNDLE_ACTIVE_LOGI("M is %{public}d, u is %{public}d", maxNum, userId);
        ret = this->QueryModuleUsageRecords(maxNum, moduleResult, userId);
        for (auto& oneModuleRecord : moduleResult) {
            dumpInfo.emplace_back(oneModuleRecord.ToString());
            for (uint32_t i = 0; i < oneModuleRecord.formRecords_.size(); i++) {
                std::string oneFormInfo = "form " + std::to_string(static_cast<int32_t>(i) + 1) + ", ";
                dumpInfo.emplace_back(oneFormInfo + oneModuleRecord.formRecords_[i].ToString());
            }
        }
    }
    return ret;
}

void BundleActiveService::DumpUsage(std::string &result)
{
    std::string dumpHelpMsg =
        "usage: bundleactive dump [<options>]\n"
        "options list:\n"
        "  -h                                                             help menu\n"
        "  -A                                                                                    \n"
        "      Events [beginTime] [endTime] [userId]                      get events for one user\n"
        "      PackageUsage [intervalType] [beginTime] [endTime] [userId] get package usage for one user\n"
        "      ModuleUsage [maxNum] [userId]                              get module usage for one user\n";
    result.append(dumpHelpMsg);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


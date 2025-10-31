/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include <parameters.h>

#include "time_service_client.h"
#ifdef DEVICE_USAGES_STATISTICS_POWERMANGER_ENABLE
#include "power_mgr_client.h"
#include "shutdown/shutdown_client.h"
#endif
#include "unistd.h"
#include "accesstoken_kit.h"

#include "bundle_active_log.h"
#include "bundle_state_inner_errors.h"
#include "bundle_active_event.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_account_helper.h"
#include "bundle_active_bundle_mgr_helper.h"
#include "bundle_active_shutdown_callback_service.h"
#include "tokenid_kit.h"
#include "xcollie/watchdog.h"
#include "bundle_active_util.h"
#include "bundle_active_report_controller.h"
#include "bundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace OHOS::Security;
static const int32_t PERIOD_BEST_JS = 0;
static const int32_t PERIOD_YEARLY_JS = 4;
static const int32_t PERIOD_BEST_SERVICE = 4;
static const int32_t DELAY_TIME = 2000 * 1000;
static const std::string PERMITTED_PROCESS_NAME_FOUNDATION = "foundation";
static const std::string PERMITTED_PROCESS_NAME_RSS = "resource_schedule_service";
static const int32_t MAXNUM_UP_LIMIT = 1000;
const int32_t EVENTS_PARAM = 5;
static constexpr int32_t NO_DUMP_PARAM_NUMS = 0;
const int32_t PACKAGE_USAGE_PARAM = 6;
const int32_t MODULE_USAGE_PARAM = 4;
const int32_t HIGH_FREQUENCY_HOUR_USAGE_PARAM = 3;
const std::string NEEDED_PERMISSION = "ohos.permission.BUNDLE_ACTIVE_INFO";
const int32_t ENG_MODE = OHOS::system::GetIntParameter("const.debuggable", 0);
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<BundleActiveService>::GetInstance().get());

BundleActiveService::BundleActiveService() : SystemAbility(DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID, true)
{
}

BundleActiveService::~BundleActiveService()
{
}
// LCOV_EXCL_START
void BundleActiveService::OnStart()
{
    BUNDLE_ACTIVE_LOGI("OnStart() called");
    if (ready_) {
        BUNDLE_ACTIVE_LOGI("service is ready. nothing to do.");
        return;
    }
    std::shared_ptr<BundleActiveService> service = shared_from_this();
    ffrt::submit([service]() {
        service->InitNecessaryState();
        });
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
        std::shared_ptr<BundleActiveService> service = shared_from_this();
        ffrt::submit([service]() {
            service->InitNecessaryState();
            }, ffrt::task_attr().delay(DELAY_TIME));
        return;
    }
    for (const auto& serviceItem : serviceIdSets) {
        auto checkResult = systemAbilityManager->CheckSystemAbility(serviceItem);
        if (!checkResult) {
            BUNDLE_ACTIVE_LOGI("request system service is not ready yet!");
            std::shared_ptr<BundleActiveService> service = shared_from_this();
            ffrt::submit([service]() {
                service->InitNecessaryState();
                }, ffrt::task_attr().delay(DELAY_TIME));
            return;
        }
    }

    for (const auto& serviceItem : serviceIdSets) {
        auto getAbility = systemAbilityManager->GetSystemAbility(serviceItem);
        if (!getAbility) {
            BUNDLE_ACTIVE_LOGI("request system service object is not ready yet!");
            std::shared_ptr<BundleActiveService> service = shared_from_this();
            ffrt::submit([service]() {
                service->InitNecessaryState();
                }, ffrt::task_attr().delay(DELAY_TIME));
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
// LCOV_EXCL_STOP
void BundleActiveService::InitService()
{
    if (bundleActiveCore_ == nullptr) {
        bundleActiveCore_ = std::make_shared<BundleActiveCore>();
        bundleActiveCore_->Init();
    }
    BundleActiveReportController::GetInstance().Init(bundleActiveCore_);
    auto bundleActiveReportHandler = BundleActiveReportController::GetInstance().GetBundleReportHandler();
// LCOV_EXCL_START
    if (bundleActiveReportHandler == nullptr || bundleActiveCore_ == nullptr) {
        return;
    }
// LCOV_EXCL_STOP
    BUNDLE_ACTIVE_LOGI("core and handler is not null");
#ifdef DEVICE_USAGES_STATISTICS_POWERMANGER_ENABLE
    shutdownCallback_ = new (std::nothrow) BundleActiveShutdownCallbackService(bundleActiveCore_);
    powerStateCallback_ = new (std::nothrow) BundleActivePowerStateCallbackService(bundleActiveCore_);
    auto& powerManagerClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
    auto& shutdownClient = OHOS::PowerMgr::ShutdownClient::GetInstance();
    if (shutdownCallback_) {
        shutdownClient.RegisterShutdownCallback(shutdownCallback_);
    }
    if (powerStateCallback_) {
        powerManagerClient.RegisterPowerStateCallback(powerStateCallback_);
    }
#endif
    InitAppStateSubscriber();
    InitContinuousSubscriber();
    bundleActiveCore_->InitBundleGroupController();
    SubscribeAppState();
    SubscribeContinuousTask();
}

OHOS::sptr<OHOS::AppExecFwk::IAppMgr> BundleActiveService::GetAppManagerInstance()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(OHOS::APP_MGR_SERVICE_ID);
// LCOV_EXCL_START
    if (!object) {
        return nullptr;
    }
// LCOV_EXCL_STOP
    return OHOS::iface_cast<OHOS::AppExecFwk::IAppMgr>(object);
}

void BundleActiveService::InitAppStateSubscriber()
{
    if (!appStateObserver_) {
        appStateObserver_ = new (std::nothrow)BundleActiveAppStateObserver();
// LCOV_EXCL_START
        if (!appStateObserver_) {
            BUNDLE_ACTIVE_LOGE("malloc app state observer failed");
            return;
        }
// LCOV_EXCL_STOP
    }
}

void BundleActiveService::InitContinuousSubscriber()
{
#ifdef BGTASKMGR_ENABLE
    if (continuousTaskObserver_ == nullptr) {
        continuousTaskObserver_ = std::make_shared<BundleActiveContinuousTaskObserver>();
    }
#endif
}

bool BundleActiveService::SubscribeAppState()
{
    sptr<OHOS::AppExecFwk::IAppMgr> appManager = GetAppManagerInstance();
// LCOV_EXCL_START
    if (appStateObserver_ == nullptr || appManager == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeAppState appstateobserver is null, return");
        return false;
    }
// LCOV_EXCL_STOP
    int32_t err = appManager->RegisterApplicationStateObserver(appStateObserver_);
    if (err != 0) {
        BUNDLE_ACTIVE_LOGE("RegisterApplicationStateObserver failed. err:%{public}d", err);
        return false;
    }
// LCOV_EXCL_START
    BUNDLE_ACTIVE_LOGD("RegisterApplicationStateObserver success.");
    return true;
// LCOV_EXCL_STOP
}

bool BundleActiveService::SubscribeContinuousTask()
{
#ifdef BGTASKMGR_ENABLE
    if (continuousTaskObserver_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("SubscribeContinuousTask continuousTaskObserver_ is null, return");
        return false;
    }
    ErrCode errCode = BackgroundTaskMgr::BackgroundTaskMgrHelper::SubscribeBackgroundTask(*continuousTaskObserver_);
// LCOV_EXCL_START
    if (errCode != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("SubscribeBackgroundTask failed.");
        return false;
    }
// LCOV_EXCL_STOP
#endif
    return true;
}

// LCOV_EXCL_START
void BundleActiveService::OnStop()
{
#ifdef DEVICE_USAGES_STATISTICS_POWERMANGER_ENABLE
    if (shutdownCallback_ != nullptr) {
        auto& shutdownClient = OHOS::PowerMgr::ShutdownClient::GetInstance();
        auto& powerManagerClient = OHOS::PowerMgr::PowerMgrClient::GetInstance();
        shutdownClient.UnRegisterShutdownCallback(shutdownCallback_);
        powerManagerClient.UnRegisterPowerStateCallback(powerStateCallback_);
        return;
    }
#endif
    bundleActiveCore_->DeInit();
    BUNDLE_ACTIVE_LOGI("[Server] OnStop");
    ready_ = false;
}
// LCOV_EXCL_STOP

ErrCode BundleActiveService::ReportEvent(const BundleActiveEvent& event, int32_t userId)
{
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (CheckNativePermission(tokenId) != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("token does not belong to native process, return");
        return ERR_PERMISSION_DENIED;
    }

    AccessToken::NativeTokenInfo callingTokenInfo;
    AccessToken::AccessTokenKit::GetNativeTokenInfo(tokenId, callingTokenInfo);
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("calling process name is %{public}s, uid is %{public}d",
        callingTokenInfo.processName.c_str(), callingUid);
    if (callingTokenInfo.processName != PERMITTED_PROCESS_NAME_FOUNDATION &&
        callingTokenInfo.processName != PERMITTED_PROCESS_NAME_RSS) {
        BUNDLE_ACTIVE_LOGE("token does not belong to fms service process, return");
        return ERR_PERMISSION_DENIED;
    }
// LCOV_EXCL_START
    BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
    BundleActiveEvent eventNew(event);
    tmpHandlerObject.event_ = eventNew;
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
    std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
        std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
    auto bundleActiveReportHandler = BundleActiveReportController::GetInstance().GetBundleReportHandler();
    if (bundleActiveReportHandler == nullptr) {
        BUNDLE_ACTIVE_LOGE("bundleActiveReportHandler is null, return");
        return ERR_OK;
    }
    bundleActiveReportHandler->SendEvent(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
    return ERR_OK;
// LCOV_EXCL_STOP
}

ErrCode BundleActiveService::IsBundleIdle(bool& isBundleIdle, const std::string& bundleName, int32_t userId)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ErrCode ret = ERR_OK;
    std::string callingBundleName = "";
    BundleActiveBundleMgrHelper::GetInstance()->GetNameForUid(callingUid, callingBundleName);
    BUNDLE_ACTIVE_LOGI("UID is %{public}d, bundle name is %{public}s", callingUid, callingBundleName.c_str());
    // get user id
    int32_t result = -1;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
// LCOV_EXCL_STOP
    }

    if (callingBundleName == bundleName) {
        if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
            BUNDLE_ACTIVE_LOGE("%{public}s is not system app", bundleName.c_str());
            return ERR_NOT_SYSTEM_APP;
        }
// LCOV_EXCL_START
        BUNDLE_ACTIVE_LOGI("%{public}s check its own idle state", bundleName.c_str());
        result = bundleActiveCore_->IsBundleIdle(bundleName, userId);
// LCOV_EXCL_STOP
    } else {
        ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
        if (ret == ERR_OK) {
            result = bundleActiveCore_->IsBundleIdle(bundleName, userId);
        } else {
            return ret;
        }
    }
// LCOV_EXCL_START
    if (result == 0 || result == -1) {
        isBundleIdle = false;
    } else {
        isBundleIdle = true;
    }
// LCOV_EXCL_STOP
    return ERR_OK;
}

ErrCode BundleActiveService::IsBundleUsePeriod(bool& IsUsePeriod, const std::string& bundleName, int32_t userId)
{
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (AccessToken::AccessTokenKit::GetTokenType(tokenId) != AccessToken::ATokenTypeEnum::TOKEN_NATIVE) {
        return ERR_PERMISSION_DENIED;
    }
    auto ret = CheckNativePermission(tokenId);
// LCOV_EXCL_START
    if (ret != ERR_OK) {
        return ret;
    }
// LCOV_EXCL_STOP
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
// LCOV_EXCL_STOP
    }
    IsUsePeriod = bundleActiveCore_->IsBundleUsePeriod(bundleName, userId);
    BUNDLE_ACTIVE_LOGI("IsBundleUsePeriod %{public}d", IsUsePeriod);
    return ERR_OK;
}

ErrCode BundleActiveService::QueryBundleStatsInfoByInterval(std::vector<BundleActivePackageStats>& packageStats,
    const int32_t intervalType, const int64_t beginTime, const int64_t endTime, int32_t userId)
{
    BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfoByInterval stats called, intervaltype is %{public}d", intervalType);
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ErrCode ret = ERR_OK;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
// LCOV_EXCL_STOP
    }
    BUNDLE_ACTIVE_LOGI("QueryBundleStatsInfos user id is %{public}d", userId);
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret != ERR_OK) {
        return ret;
    }
    std::vector<BundleActivePackageStats> tempPackageStats;
    int32_t convertedIntervalType = ConvertIntervalType(intervalType);
    ret = bundleActiveCore_->QueryBundleStatsInfos(
        tempPackageStats, userId, convertedIntervalType, beginTime, endTime, "");
// LCOV_EXCL_START
    for (auto& packageStat : tempPackageStats) {
        packageStat.appIndex_ = GetNameAndIndexForUid(packageStat.uid_);
    }
// LCOV_EXCL_STOP
    packageStats = MergePackageStats(tempPackageStats);
    return ret;
}

int32_t BundleActiveService::GetNameAndIndexForUid(int32_t uid)
{
    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
// LCOV_EXCL_START
    if (systemAbilityManager == nullptr) {
        BUNDLE_ACTIVE_LOGE("failed to get samgr");
        return -1;
    }
// LCOV_EXCL_STOP

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
// LCOV_EXCL_START
    if (remoteObject == nullptr) {
        BUNDLE_ACTIVE_LOGE("failed to get bundle manager service");
        return -1;
    }
// LCOV_EXCL_STOP
    sptr<AppExecFwk::IBundleMgr> bundleManager = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    int32_t appIndex = -1;
    if (bundleManager != nullptr) {
        std::string bundleName;
        bundleManager->GetNameAndIndexForUid(uid, bundleName, appIndex);
    }
    return appIndex;
}

ErrCode BundleActiveService::QueryBundleEvents(BundleActiveEventVecRawData& bundleActiveEventVecRawData,
    const int64_t beginTime, const int64_t endTime, int32_t userId)
{
    ErrCode ret = ERR_OK;
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
// LCOV_EXCL_STOP
    }
    BUNDLE_ACTIVE_LOGI("QueryBundleEvents userid is %{public}d", userId);
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    std::vector<BundleActiveEvent> bundleActiveEvents;
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->QueryBundleEvents(bundleActiveEvents, userId, beginTime, endTime, "");
        BUNDLE_ACTIVE_LOGI("QueryBundleEvents result is %{public}zu", bundleActiveEvents.size());
    }
    if (ret == ERR_OK) {
        ret = bundleActiveEventVecRawData.Marshalling(bundleActiveEvents);
    }
    return ret;
}

ErrCode BundleActiveService::QueryHighFrequencyPeriodBundle(
    std::vector<BundleActiveHighFrequencyPeriod>& appFreqHours, int32_t userId)
{
    ErrCode ret = ERR_OK;
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
// LCOV_EXCL_STOP
    }
    BUNDLE_ACTIVE_LOGI("QueryHighFrequencyPeriodBundle userid is %{public}d", userId);
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
// LCOV_EXCL_START
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->QueryHighFrequencyPeriodBundle(appFreqHours, userId);
        BUNDLE_ACTIVE_LOGI("QueryHighFrequencyPeriodBundle result is %{public}zu", appFreqHours.size());
    }
// LCOV_EXCL_STOP
    return ret;
}

ErrCode BundleActiveService::QueryBundleTodayLatestUsedTime(
    int64_t& latestUsedTime, const std::string& bundleName, int32_t userId)
{
    ErrCode ret = ERR_OK;
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ret;
        }
// LCOV_EXCL_STOP
    }
    BUNDLE_ACTIVE_LOGI("QueryBundleTodayLatestUsedTime userid is %{public}d", userId);
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
// LCOV_EXCL_START
    if (ret != ERR_OK) {
        return ret;
    }
// LCOV_EXCL_STOP
    int64_t currentSystemTime = BundleActiveUtil::GetSystemTimeMs();
    int64_t startTime = BundleActiveUtil::GetIntervalTypeStartTime(currentSystemTime, BundleActiveUtil::PERIOD_DAILY);
    std::vector<BundleActivePackageStats> packageStats;
    ret = bundleActiveCore_->QueryBundleStatsInfos(
        packageStats, userId, BundleActiveUtil::PERIOD_DAILY, startTime, currentSystemTime, bundleName);
    if (ret != ERR_OK) {
        return ret;
    }
    auto bundleActivePackageStats = MergePackageStats(packageStats);
    if (bundleActivePackageStats.empty() || bundleActivePackageStats[0].bundleName_ != bundleName) {
        return ERR_NO_APP_GROUP_INFO_IN_DATABASE;
    }
    latestUsedTime = bundleActivePackageStats[0].lastTimeUsed_;
    return ret;
}

ErrCode BundleActiveService::SetAppGroup(const std::string& bundleName, int32_t newGroup, int32_t userId)
{
    ErrCode ret = ERR_OK;
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    bool isFlush = false;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        }
// LCOV_EXCL_STOP
        isFlush = true;
    }
    BUNDLE_ACTIVE_LOGI("SetAppGroup userId is %{public}d", userId);

    std::string localBundleName = "";
    BundleActiveBundleMgrHelper::GetInstance()->GetNameForUid(callingUid, localBundleName);
    if (localBundleName == bundleName) {
        BUNDLE_ACTIVE_LOGI("SetAppGroup can not set its bundleName");
        return ERR_PERMISSION_DENIED;
    }
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
// LCOV_EXCL_START
    if (ret == ERR_OK) {
        ret = bundleActiveCore_->SetAppGroup(bundleName, newGroup, userId, isFlush);
    }
// LCOV_EXCL_STOP
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
// LCOV_EXCL_START
    if (ret != ERR_OK || userId == -1) {
        return ret;
    }
// LCOV_EXCL_STOP
    std::vector<BundleActivePackageStats> tempPackageStats;
    BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfos userid is %{public}d", userId);
    std::string bundleName = "";
    BundleActiveBundleMgrHelper::GetInstance()->GetNameForUid(callingUid, bundleName);
    ErrCode isSystemAppAndHasPermission = CheckBundleIsSystemAppAndHasPermission(callingUid, tokenId);
    if (!bundleName.empty() && isSystemAppAndHasPermission == ERR_OK) {
// LCOV_EXCL_START
        int32_t convertedIntervalType = ConvertIntervalType(intervalType);
        ret = bundleActiveCore_->QueryBundleStatsInfos(tempPackageStats, userId, convertedIntervalType,
            beginTime, endTime, bundleName);
    }
// LCOV_EXCL_STOP
    bundleActivePackageStats = MergePackageStats(tempPackageStats);
    return ret;
}

ErrCode BundleActiveService::QueryHighFrequencyUsageBundleInfos(std::vector<BundleActivePackageStats>& packageStats,
    const int32_t userId, const int32_t maxNum)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    int32_t ret = CheckSystemAppOrNativePermission(callingUid, tokenId);
    if (ret != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("no premission.");
        return ERR_PERMISSION_DENIED;
    }
    BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfos userid is %{public}d", userId);
    std::vector<BundleActivePackageStats> tempPackageStats;
    constexpr int64_t SEVEN_DAYS = static_cast<int64_t>(7) * 24 * 60 * 60 * 1000;
    int64_t endTime = BundleActiveUtil::GetSystemTimeMs();
    int64_t beginTime = endTime - SEVEN_DAYS;
    ret = bundleActiveCore_->QueryBundleStatsInfos(tempPackageStats, userId, BundleActiveUtil::PERIOD_DAILY,
        beginTime, endTime, "");
    std::vector<BundleActivePackageStats> results;
    results = MergePackageStats(tempPackageStats);
    std::sort(results.begin(), results.end(),
        [](const BundleActivePackageStats& lhs, const BundleActivePackageStats& rhs) {
            return lhs.startCount_ > rhs.startCount_;
        });
    if (static_cast<int32_t>(results.size()) > maxNum) {
        packageStats.assign(results.begin(), results.begin() + maxNum);
    } else {
        packageStats = std::move(results);
    }
    return ret;
}

ErrCode BundleActiveService::QueryCurrentBundleEvents(BundleActiveEventVecRawData& bundleActiveEventVecRawData,
    const int64_t beginTime, const int64_t endTime)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents UID is %{public}d", callingUid);
    // get userid
    int32_t userId = -1;
    ErrCode ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
    std::vector<BundleActiveEvent> bundleActiveEvents;
    if (ret == ERR_OK && userId != -1) {
        std::string bundleName = "";
        BundleActiveBundleMgrHelper::GetInstance()->GetNameForUid(callingUid, bundleName);
// LCOV_EXCL_START
        if (!bundleName.empty()) {
            if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
                BUNDLE_ACTIVE_LOGE("%{public}s is not system app", bundleName.c_str());
                return ERR_NOT_SYSTEM_APP;
            }
            BUNDLE_ACTIVE_LOGI("QueryCurrentBundleEvents buindle name is %{public}s",
                bundleName.c_str());
            ret = bundleActiveCore_->QueryBundleEvents(bundleActiveEvents, userId, beginTime, endTime, bundleName);
            if (ret == ERR_OK) {
                ret = bundleActiveEventVecRawData.Marshalling(bundleActiveEvents);
            }
        }
// LCOV_EXCL_STOP
    }
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents bundleActiveEvents size is %{public}zu", bundleActiveEvents.size());
    return ret;
}

ErrCode BundleActiveService::QueryAppGroup(int32_t& appGroup, const std::string& bundleName, int32_t userId)
{
    // get uid
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    BUNDLE_ACTIVE_LOGD("QueryAppGroup UID is %{public}d", callingUid);
    ErrCode ret = ERR_OK;
    if (userId == -1) {
        ret = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (ret != ERR_OK || userId == -1) {
            return ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        }
// LCOV_EXCL_STOP
    }
    if (bundleName.empty()) {
        std::string localBundleName = "";
        BundleActiveBundleMgrHelper::GetInstance()->GetNameForUid(callingUid, localBundleName);
        if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
            BUNDLE_ACTIVE_LOGE("%{public}s is not system app", localBundleName.c_str());
            return ERR_NOT_SYSTEM_APP;
        }
// LCOV_EXCL_START
        ret = bundleActiveCore_->QueryAppGroup(appGroup, localBundleName, userId);
// LCOV_EXCL_STOP
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
// LCOV_EXCL_START
    if (!bundleActiveCore_) {
        return ERR_MEMORY_OPERATION_FAILED;
    }
// LCOV_EXCL_STOP
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
    std::string bundleName = "";
    BundleActiveBundleMgrHelper::GetInstance()->GetNameForUid(uid, bundleName);

    if (!Security::AccessToken::TokenIdKit::IsSystemAppByFullTokenID(IPCSkeleton::GetCallingFullTokenID())) {
        BUNDLE_ACTIVE_LOGE("%{public}s is not system app", bundleName.c_str());
        return ERR_NOT_SYSTEM_APP;
    }
// LCOV_EXCL_START
    int32_t bundleHasPermission = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, NEEDED_PERMISSION);
    if (bundleHasPermission != 0) {
        BUNDLE_ACTIVE_LOGE("%{public}s hasn't permission", bundleName.c_str());
        return ERR_PERMISSION_DENIED;
    }
    BUNDLE_ACTIVE_LOGI("%{public}s has permission", bundleName.c_str());
    return ERR_OK;
// LCOV_EXCL_STOP
}

ErrCode BundleActiveService::CheckNativePermission(OHOS::Security::AccessToken::AccessTokenID tokenId)
{
    int32_t bundleHasPermission = AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, NEEDED_PERMISSION);
    if (bundleHasPermission != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        BUNDLE_ACTIVE_LOGE("check native permission not have permission");
        return ERR_PERMISSION_DENIED;
    }
    auto tokenFlag = AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (tokenFlag == AccessToken::TypeATokenTypeEnum::TOKEN_NATIVE) {
        return ERR_OK;
    }
// LCOV_EXCL_START
    if (tokenFlag == AccessToken::TypeATokenTypeEnum::TOKEN_SHELL) {
        return ERR_OK;
    }
    return ERR_PERMISSION_DENIED;
// LCOV_EXCL_STOP
}

ErrCode BundleActiveService::CheckSystemAppOrNativePermission(const int32_t uid,
    OHOS::Security::AccessToken::AccessTokenID tokenId)
{
// LCOV_EXCL_START
    if (AccessToken::AccessTokenKit::GetTokenType(tokenId) == AccessToken::ATokenTypeEnum::TOKEN_HAP) {
        return CheckBundleIsSystemAppAndHasPermission(uid, tokenId);
    }
// LCOV_EXCL_STOP
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
// LCOV_EXCL_START
        if (errCode != ERR_OK || userId == -1) {
            return errCode;
        }
// LCOV_EXCL_STOP
    }
    BUNDLE_ACTIVE_LOGI("QueryModuleUsageRecords userid is %{public}d", userId);
    AccessToken::AccessTokenID tokenId = OHOS::IPCSkeleton::GetCallingTokenID();
    errCode = CheckSystemAppOrNativePermission(callingUid, tokenId);
// LCOV_EXCL_START
    if (errCode == ERR_OK) {
        errCode = bundleActiveCore_->QueryModuleUsageRecords(maxNum, results, userId);
        for (auto& oneResult : results) {
            QueryModuleRecordInfos(oneResult);
        }
    }
// LCOV_EXCL_STOP
    return errCode;
}

ErrCode BundleActiveService::QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    int32_t callingUid = OHOS::IPCSkeleton::GetCallingUid();
    ErrCode errCode = ERR_OK;
    if (userId == -1) {
        errCode = BundleActiveAccountHelper::GetUserId(callingUid, userId);
// LCOV_EXCL_START
        if (errCode != ERR_OK || userId == -1) {
            return errCode;
        }
// LCOV_EXCL_STOP
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
// LCOV_EXCL_START
        if (errCode != ERR_OK || userId == -1) {
            return errCode;
        }
// LCOV_EXCL_STOP
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
    ApplicationInfo appInfo;
    bool getInfoIsSuccess = BundleActiveBundleMgrHelper::GetInstance()->GetApplicationInfo(moduleRecord.bundleName_,
        ApplicationFlag::GET_BASIC_APPLICATION_INFO, moduleRecord.userId_, appInfo);
    if (!getInfoIsSuccess) {
        BUNDLE_ACTIVE_LOGE("GetApplicationInfo failed!");
        return;
    }
// LCOV_EXCL_START
    BundleInfo bundleInfo;
    getInfoIsSuccess = BundleActiveBundleMgrHelper::GetInstance()->GetBundleInfo(moduleRecord.bundleName_,
        BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO, bundleInfo, moduleRecord.userId_);
    if (!getInfoIsSuccess) {
        BUNDLE_ACTIVE_LOGE("GetBundleInfo failed!");
        return;
    }
    for (const auto& oneModuleInfo : bundleInfo.hapModuleInfos) {
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
// LCOV_EXCL_STOP
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

bool BundleActiveService::AllowDump()
{
// LCOV_EXCL_START
    if (ENG_MODE == 0) {
        BUNDLE_ACTIVE_LOGE("Not eng mode");
        return false;
    }
// LCOV_EXCL_STOP
    Security::AccessToken::AccessTokenID tokenId = IPCSkeleton::GetFirstTokenID();
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenId, "ohos.permission.DUMP");
    if (ret != Security::AccessToken::PermissionState::PERMISSION_GRANTED) {
        BUNDLE_ACTIVE_LOGE("CheckPermission failed");
        return false;
    }
    return true;
}

int32_t BundleActiveService::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    if (!AllowDump()) {
        return ERR_PERMISSION_DENIED;
    }
// LCOV_EXCL_START
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
// LCOV_EXCL_STOP
}

int32_t BundleActiveService::ShellDump(const std::vector<std::string> &dumpOption, std::vector<std::string> &dumpInfo)
{
    int32_t ret = -1;
    if (!bundleActiveCore_) {
        return ret;
    }
    if (dumpOption[1] == "Events") {
        ret = DumpEvents(dumpOption, dumpInfo);
    } else if (dumpOption[1] == "PackageUsage") {
        ret = DumpPackageUsage(dumpOption, dumpInfo);
    } else if (dumpOption[1] == "ModuleUsage") {
        ret = DumpModuleUsage(dumpOption, dumpInfo);
    } else if (dumpOption[1] == "HighFreqHourUsage") {
        ret = DumpHighFreqHourUsage(dumpOption, dumpInfo);
    }
    return ret;
}

int32_t BundleActiveService::DumpEvents(const std::vector<std::string> &dumpOption, std::vector<std::string> &dumpInfo)
{
    int32_t ret = -1;
    std::vector<BundleActiveEvent> eventResult;
    if (static_cast<int32_t>(dumpOption.size()) != EVENTS_PARAM) {
        return ret;
    }
// LCOV_EXCL_START
    int64_t beginTime = BundleActiveUtil::StringToInt64(dumpOption[2]);
    int64_t endTime = BundleActiveUtil::StringToInt64(dumpOption[3]);
    int32_t userId = BundleActiveUtil::StringToInt32(dumpOption[4]);
    bundleActiveCore_->QueryBundleEvents(eventResult, userId, beginTime, endTime, "");
    for (auto& oneEvent : eventResult) {
        dumpInfo.emplace_back(oneEvent.ToString());
    }
    return ret;
// LCOV_EXCL_STOP
}

int32_t BundleActiveService::DumpPackageUsage(const std::vector<std::string> &dumpOption,
    std::vector<std::string> &dumpInfo)
{
    int32_t ret = -1;
    std::vector<BundleActivePackageStats> tempPackageUsage;
// LCOV_EXCL_START
    if (static_cast<int32_t>(dumpOption.size()) != PACKAGE_USAGE_PARAM) {
        return ret;
    }
// LCOV_EXCL_STOP
    int32_t intervalType = ConvertIntervalType(BundleActiveUtil::StringToInt32(dumpOption[2]));
    int64_t beginTime = BundleActiveUtil::StringToInt64(dumpOption[3]);
    int64_t endTime = BundleActiveUtil::StringToInt64(dumpOption[4]);
    int32_t userId = BundleActiveUtil::StringToInt32(dumpOption[5]);
    bundleActiveCore_->QueryBundleStatsInfos(
        tempPackageUsage, userId, intervalType, beginTime, endTime, "");
    auto packageUsageResult = MergePackageStats(tempPackageUsage);
// LCOV_EXCL_START
    for (auto& onePackageRecord : packageUsageResult) {
        dumpInfo.emplace_back(onePackageRecord.ToString());
    }
// LCOV_EXCL_STOP
    return ret;
}

int32_t BundleActiveService::DumpModuleUsage(const std::vector<std::string> &dumpOption,
    std::vector<std::string> &dumpInfo)
{
    int32_t ret = -1;
    std::vector<BundleActiveModuleRecord> moduleResult;
    if (static_cast<int32_t>(dumpOption.size()) != MODULE_USAGE_PARAM) {
        return ret;
    }
// LCOV_EXCL_START
    int32_t maxNum = BundleActiveUtil::StringToInt32(dumpOption[2]);
    int32_t userId = BundleActiveUtil::StringToInt32(dumpOption[3]);
    BUNDLE_ACTIVE_LOGI("M is %{public}d, u is %{public}d", maxNum, userId);
    ret = bundleActiveCore_->QueryModuleUsageRecords(maxNum, moduleResult, userId);
    for (auto& oneResult : moduleResult) {
        QueryModuleRecordInfos(oneResult);
    }
    for (auto& oneModuleRecord : moduleResult) {
        dumpInfo.emplace_back(oneModuleRecord.ToString());
        for (uint32_t i = 0; i < oneModuleRecord.formRecords_.size(); i++) {
            std::string oneFormInfo = "form " + std::to_string(static_cast<int32_t>(i) + 1) + ", ";
            dumpInfo.emplace_back(oneFormInfo + oneModuleRecord.formRecords_[i].ToString());
        }
    }
    return ret;
// LCOV_EXCL_STOP
}

int32_t BundleActiveService::DumpHighFreqHourUsage(const std::vector<std::string>& dumpOption,
    std::vector<std::string>& dumpInfo)
{
    int32_t ret = -1;
    if (static_cast<int32_t>(dumpOption.size()) != HIGH_FREQUENCY_HOUR_USAGE_PARAM) {
        return ret;
    }
// LCOV_EXCL_START
    int32_t userId = BundleActiveUtil::StringToInt64(dumpOption[2]);
    std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
    ret = bundleActiveCore_->QueryHighFrequencyPeriodBundle(appFreqHours, userId);
    dumpInfo.emplace_back("appFreqHour size " + std::to_string(appFreqHours.size()) + "\n");
    for (auto& appFreqHour : appFreqHours) {
        dumpInfo.emplace_back(appFreqHour.ToString());
    }
    return ret;
// LCOV_EXCL_STOP
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
// LCOV_EXCL_START
std::vector<BundleActivePackageStats> BundleActiveService::MergePackageStats(
    const std::vector<BundleActivePackageStats>& packageStats)
{
    if (packageStats.empty()) {
        return packageStats;
    }
    std::vector<BundleActivePackageStats> tempPackageStats;
    std::shared_ptr<std::map<std::string, BundleActivePackageStats>> mergedPackageStats =
        std::make_shared<std::map<std::string, BundleActivePackageStats>>();
    for (auto packageStat : packageStats) {
        std::string mergedPackageStatsKey = packageStat.bundleName_ + std::to_string(packageStat.uid_);
        auto iter = mergedPackageStats->find(mergedPackageStatsKey);
        if (iter != mergedPackageStats->end()) {
            MergeSamePackageStats(iter->second, packageStat);
        } else {
            mergedPackageStats->
                insert(std::pair<std::string, BundleActivePackageStats>(mergedPackageStatsKey, packageStat));
        }
    }
    for (auto pair : *mergedPackageStats) {
        tempPackageStats.push_back(pair.second);
    }
    return tempPackageStats;
}
// LCOV_EXCL_STOP
void BundleActiveService::MergeSamePackageStats(BundleActivePackageStats& left, const BundleActivePackageStats& right)
{
    if (left.bundleName_ != right.bundleName_) {
        BUNDLE_ACTIVE_LOGE("Merge package stats failed, existing packageName : %{public}s,"
            " new packageName : %{public}s,", left.bundleName_.c_str(), right.bundleName_.c_str());
        return;
    }
    left.lastTimeUsed_ = std::max(left.lastTimeUsed_, right.lastTimeUsed_);
    left.lastContiniousTaskUsed_ = std::max(left.lastContiniousTaskUsed_, right.lastContiniousTaskUsed_);
    left.totalInFrontTime_ += right.totalInFrontTime_;
    left.totalContiniousTaskUsedTime_ += right.totalContiniousTaskUsedTime_;
    left.bundleStartedCount_ += right.bundleStartedCount_;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


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

#include "bundle_active_app_state_observer.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_event.h"
#include "bundle_active_continuous_task_observer.h"

namespace OHOS {
namespace DeviceUsageStats {
#ifndef OS_ACCOUNT_PART_ENABLED
namespace {
constexpr int32_t UID_TRANSFORM_DIVISOR = 200000;
void GetOsAccountIdFromUid(int uid, int &osAccountId)
{
    osAccountId = uid / UID_TRANSFORM_DIVISOR;
}
} // namespace
#endif // OS_ACCOUNT_PART_ENABLED

void BundleActiveContinuousTaskObserver::Init(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
    if (reportHandler != nullptr) {
        BUNDLE_ACTIVE_LOGI("BundleActiveAppStateObserver::Init report handler is not null, init success");
        reportHandler_ = reportHandler;
    }
}

void BundleActiveContinuousTaskObserver::OnContinuousTaskStart(
    const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>& continuousTaskCallbackInfo)
{
    ReportContinuousTaskEvent(continuousTaskCallbackInfo, true);
}

void BundleActiveContinuousTaskObserver::OnContinuousTaskStop(
    const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo> &continuousTaskCallbackInfo)
{
    ReportContinuousTaskEvent(continuousTaskCallbackInfo, false);
}

void BundleActiveContinuousTaskObserver::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    isRemoteDied_.store(true);
}

bool BundleActiveContinuousTaskObserver::GetBundleMgr()
{
    if (!bundleMgr_) {
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
        bundleMgr_ = iface_cast<IBundleMgr>(remoteObject);
        if ((!bundleMgr_) || (!bundleMgr_->AsObject())) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability");
            return false;
        }
    }
    return true;
}

void BundleActiveContinuousTaskObserver::ReportContinuousTaskEvent(
    const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>& continuousTaskCallbackInfo,
    const bool isStart)
{
    int uid = continuousTaskCallbackInfo->GetCreatorUid();
    int pid = continuousTaskCallbackInfo->GetCreatorPid();
    std::string continuousTaskAbilityName_ = continuousTaskCallbackInfo->GetAbilityName();
    int userId = -1;
    std::string bundleName = "";
    if (GetBundleMgr()) {
        bundleMgr_->GetBundleNameForUid(uid, bundleName);
    } else {
        BUNDLE_ACTIVE_LOGE("Get bundle mgr failed!");
        return;
    }
#ifdef OS_ACCOUNT_PART_ENABLED
    OHOS::ErrCode ret = OHOS::AccountSA::OsAccountManager::GetOsAccountLocalIdFromUid(uid, userId);
#else // OS_ACCOUNT_PART_ENABLED
    OHOS::ErrCode ret = ERR_OK;
    GetOsAccountIdFromUid(uid, userId);
#endif // OS_ACCOUNT_PART_ENABLED
    if (ret == ERR_OK && userId != -1 && !bundleName.empty()) {
        BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
        BundleActiveEvent event(bundleName, continuousTaskAbilityName_);
        sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
        tmpHandlerObject.event_ = event;
        tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
        if (isStart) {
            tmpHandlerObject.event_.eventId_ = BundleActiveEvent::LONG_TIME_TASK_STARTTED;
        } else {
            tmpHandlerObject.event_.eventId_ = BundleActiveEvent::LONG_TIME_TASK_ENDED;
        }
        BUNDLE_ACTIVE_LOGI("OnContinuousTaskStart id is %{public}d, bundle name is %{public}s, "
            "ability name is %{public}s, event id is %{public}d,"
            "uid is %{public}d, pid is %{public}d",
            tmpHandlerObject.userId_, tmpHandlerObject.event_.bundleName_.c_str(),
            tmpHandlerObject.event_.continuousTaskAbilityName_.c_str(), tmpHandlerObject.event_.eventId_,
            uid, pid);
        if (reportHandler_ != nullptr) {
            BUNDLE_ACTIVE_LOGI("BundleActiveAppStateObserver::OnAbilityStateChanged handler not null, SEND");
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            auto event = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
            reportHandler_->SendEvent(event);
        }
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


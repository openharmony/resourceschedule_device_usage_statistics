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

#ifdef BGTASKMGR_ENABLE
#include "time_service_client.h"

#include "bundle_active_app_state_observer.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_event.h"
#include "bundle_active_account_helper.h"
#include "bundle_active_continuous_task_observer.h"

namespace OHOS {
namespace DeviceUsageStats {
void BundleActiveContinuousTaskObserver::Init(const std::shared_ptr<BundleActiveReportHandler>& reportHandler)
{
    if (reportHandler != nullptr) {
        BUNDLE_ACTIVE_LOGI("report handler is not null, init success");
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
        if (!bundleMgr_) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability, bundleMgr_");
            return false;
        }
        auto object = bundleMgr_->AsObject();
        if (!object) {
            BUNDLE_ACTIVE_LOGE("Failed to get system bundle manager services ability, bundleMgr_->AsObject()");
            return false;
        }
    }
    return true;
}

void BundleActiveContinuousTaskObserver::ReportContinuousTaskEvent(
    const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>& continuousTaskCallbackInfo,
    const bool isStart)
{
    int32_t uid = continuousTaskCallbackInfo->GetCreatorUid();
    pid_t pid = continuousTaskCallbackInfo->GetCreatorPid();
    std::string continuousTaskAbilityName_ = continuousTaskCallbackInfo->GetAbilityName();
    int32_t userId = -1;
    std::string bundleName = "";
    if (GetBundleMgr()) {
        bundleMgr_->GetBundleNameForUid(uid, bundleName);
    } else {
        BUNDLE_ACTIVE_LOGE("Get bundle mgr failed!");
        return;
    }
    OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(uid, userId);
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
            auto getEvent = AppExecFwk::InnerEvent::Get(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
            reportHandler_->SendEvent(getEvent);
        }
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS

#endif
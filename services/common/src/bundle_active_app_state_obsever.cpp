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
#include "bundle_active_app_state_observer.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_event.h"
#include "bundle_active_account_helper.h"
#include "bundle_active_report_controller.h"

namespace OHOS {
namespace DeviceUsageStats {

void BundleActiveAppStateObserver::OnAbilityStateChanged(const AbilityStateData &abilityStateData)
{
    if (!ValidateAbilityStateData(abilityStateData)) {
        BUNDLE_ACTIVE_LOGE("validate ability state data failed!");
        return;
    }
    if (abilityStateData.abilityType != 1) {
        return;
    }
    int32_t userId = -1;
    OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(abilityStateData.uid, userId);
    if (ret == ERR_OK && userId != -1) {
        std::stringstream stream;
        stream << abilityStateData.token.GetRefPtr();
        BundleActiveReportHandlerObject tmpHandlerObject(userId, "");
        BundleActiveEvent event(abilityStateData.bundleName, abilityStateData.abilityName,
            abilityStateData.abilityName, abilityStateData.moduleName, abilityStateData.uid);
        tmpHandlerObject.event_ = event;
        sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
        tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
        switch (abilityStateData.abilityState) {
            case static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND):
                tmpHandlerObject.event_.eventId_ = BundleActiveEvent::ABILITY_FOREGROUND;
                break;
            case static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND):
                tmpHandlerObject.event_.eventId_ = BundleActiveEvent::ABILITY_BACKGROUND;
                break;
            case static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED):
                tmpHandlerObject.event_.eventId_ = BundleActiveEvent::ABILITY_STOP;
                break;
            default:
                return;
        }
        BUNDLE_ACTIVE_LOGI("OnAblityStateChanged %{public}s", tmpHandlerObject.ToString().c_str());
        auto reportHandler = BundleActiveReportController::GetInstance().GetBundleReportHandler();
        if (reportHandler != nullptr) {
            std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
                std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
            reportHandler->SendEvent(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
        }
    }
    return;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


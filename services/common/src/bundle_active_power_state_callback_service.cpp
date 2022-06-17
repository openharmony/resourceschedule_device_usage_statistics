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

#include "bundle_active_power_state_callback_service.h"
#include "power_state_machine_info.h"

namespace OHOS {
namespace DeviceUsageStats {
using OHOS::PowerMgr::PowerState;
BundleActivePowerStateCallbackService::BundleActivePowerStateCallbackService(
    std::shared_ptr<BundleActiveCore> bundleActiveCore)
{
    if (bundleActiveCore != nullptr) {
        bundleActiveCore_ = bundleActiveCore;
    }
}

void BundleActivePowerStateCallbackService::OnPowerStateChanged(PowerState state)
{
    int32_t eventId;
    BUNDLE_ACTIVE_LOGD("BundleActDvePowerStateCallbackService::OnPowerStateChanged power state is %{public}u", state);
    if (state == PowerState::AWAKE) {
        eventId = BundleActiveEvent::SYSTEM_WAKEUP;
    } else if (state == PowerState::SLEEP) {
        eventId = BundleActiveEvent::SYSTEM_SLEEP;
    } else {
        return;
    }
    bundleActiveCore_->PreservePowerStateInfo(eventId);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


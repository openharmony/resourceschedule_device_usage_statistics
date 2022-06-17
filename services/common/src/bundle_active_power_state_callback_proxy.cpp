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

#include "bundle_active_power_state_callback_proxy.h"
#include "power_state_machine_info.h"

namespace OHOS {
namespace DeviceUsageStats {
void BundleActivePowerStateCallbackProxy::OnPowerStateChanged(PowerMgr::PowerState state)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return;
    }
    MessageParcel data;
    data.WriteInt32((int32_t)state);
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(BundleActivePowerStateCallbackProxy::GetDescriptor())) {
        return;
    }
    int32_t ret = remote->SendRequest(IPowerStateCallback::POWER_STATE_CHANGED, data, reply, option);
    if (ret != ERR_OK) {
        BUNDLE_ACTIVE_LOGE("BundleActivePowerStateCallbackProxy::PowerStateCallback failed!");
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


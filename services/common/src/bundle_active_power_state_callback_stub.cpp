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

#include "bundle_active_power_state_callback_stub.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
int32_t BundleActivePowerStateCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string descriptor = BundleActivePowerStateCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    PowerMgr::PowerState state = static_cast<PowerMgr::PowerState>(data.ReadInt32());
    if (descriptor != remoteDescriptor) {
        BUNDLE_ACTIVE_LOGE("BundleActivePowerStateCallbackStub::OnRemoteRequest cannot get power mgr service");
        return -1;
    }
    switch (code) {
        case IPowerStateCallback::POWER_STATE_CHANGED: {
            PowerStateStub(state);
            return ERR_OK;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
        }
}

void BundleActivePowerStateCallbackStub::PowerStateStub(PowerMgr::PowerState state)
{
    OnPowerStateChanged(state);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


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

#include "bundle_active_group_callback_stub.h"

namespace OHOS {
namespace DeviceUsageStats {
static std::mutex callbackMutex_;
int32_t BundleActiveGroupCallbackStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string descriptor = BundleActiveGroupCallbackStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack OnRemoteRequest cannot get power mgr service");
        return -1;
    }
    BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack BundleActiveGroupCallbackStub will switch");
    switch (code) {
        case static_cast<uint32_t>(IBundleActiveGroupCallback::message::ON_BUNDLE_GROUP_CHANGED): {
            BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack OnRemoteRequest is nowing ON_BUNDLE_GROUP_CHANGED");
            BundleActiveGroupCallbackInfo* groupInfo = nullptr;
            std::unique_lock<std::mutex> lock(callbackMutex_);
            groupInfo = data.ReadParcelable<BundleActiveGroupCallbackInfo>();
            if (!groupInfo) {
                BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack ReadParcelable<AbilityStateData> failed");
                return -1;
            }
            OnBundleGroupChanged(*groupInfo);
            delete groupInfo;
            groupInfo = nullptr;
            break;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}

void BundleActiveGroupCallbackStub::OnBundleGroupChanged(
    const BundleActiveGroupCallbackInfo &bundleActiveGroupCallbackInfo)
{
}
}  // namespace DeviceUsageStats
}  // namespace OHOS

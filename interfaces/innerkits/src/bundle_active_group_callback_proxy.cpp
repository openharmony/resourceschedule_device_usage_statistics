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
#include "bundle_active_group_callback_proxy.h"
#include <message_parcel.h>
#include <stdint.h>
#include "bundle_active_log.h"
#include "errors.h"
#include "message_option.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveGroupCallbackProxy::BundleActiveGroupCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IBundleActiveGroupCallback>(impl) {}
BundleActiveGroupCallbackProxy::~BundleActiveGroupCallbackProxy() {}

void BundleActiveGroupCallbackProxy::OnBundleGroupChanged(
    const BundleActiveGroupCallbackInfo &bundleActiveGroupCallbackInfo)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack remote is dead.");
        return;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(BundleActiveGroupCallbackProxy::GetDescriptor())) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack write interface token failed.");
        return;
    }
    if (!data.WriteParcelable(&bundleActiveGroupCallbackInfo)) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack write parcel failed.");
        return;
    }
    MessageParcel reply;
    MessageOption option = {MessageOption::TF_ASYNC};
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IBundleActiveGroupCallback::message::ON_BUNDLE_GROUP_CHANGED), data, reply, option);
    if (ret!= ERR_OK) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack SendRequest failed, error code: %{public}d", ret);
    }
}
}  // namespace BackgroundTaskMgr
}  // namespace OHOS
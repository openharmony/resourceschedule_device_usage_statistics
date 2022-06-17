/*
 * Copyright (c) Huawei Device Co., Ltd. 2022-2022. All rights reserved.
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

#ifndef BUNDLE_ACTIVE_GROUP_CALLBACK_STUB_H
#define BUNDLE_ACTIVE_GROUP_CALLBACK_STUB_H

#include "iremote_stub.h"
#include "ibundle_active_group_callback.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveGroupCallbackStub : public IRemoteStub<IBundleActiveGroupCallback> {
public:
    BundleActiveGroupCallbackStub()=default;
    virtual ~BundleActiveGroupCallbackStub()=default;
    /*
    * function: OnRemoteRequest, handle message from proxy.
    * parameters: code, data, reply, option
    * return: errorcode.
    */
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    /*
    * function: OnBundleGroupChanged, bundleGroupChanged callback, handle message from proxy.
    * parameters: bundleActiveGroupCallbackInfo
    * return: void.
    */
    virtual void OnBundleGroupChanged(
        const BundleActiveGroupCallbackInfo &bundleActiveGroupCallbackInfo) override;
private:
    DISALLOW_COPY_AND_MOVE(BundleActiveGroupCallbackStub);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_GROUP_CALLBACK_STUB_H

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

#ifndef BUNDLE_ACTIVE_GROUP_CALLBACK_PROXY_H
#define BUNDLE_ACTIVE_GROUP_CALLBACK_PROXY_H

#include "iremote_proxy.h"
#include "ibundle_active_group_callback.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveGroupCallbackProxy : public IRemoteProxy<IBundleActiveGroupCallback> {
public:
    explicit BundleActiveGroupCallbackProxy(const sptr<IRemoteObject>& impl);
    virtual ~BundleActiveGroupCallbackProxy() override;
    DISALLOW_COPY_AND_MOVE(BundleActiveGroupCallbackProxy);
    /*
    * function: OnBundleGroupChanged, bundleGroupChanged callback, IPC proxy, send message to stub.
    * parameters: bundleActiveGroupCallbackInfo
    * return: void.
    */
    virtual void OnBundleGroupChanged(
        const BundleActiveGroupCallbackInfo& continuousTaskCallbackInfo) override;

private:
    static inline BrokerDelegator<BundleActiveGroupCallbackProxy> delegator_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_GROUP_CALLBACK_PROXY_H
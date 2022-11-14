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

#ifndef APP_GROUP_CALLBACK_PROXY_H
#define APP_GROUP_CALLBACK_PROXY_H

#include <functional>
#include "nocopyable.h"

#include "iremote_proxy.h"

#include "iapp_group_callback.h"
#include "app_group_callback_info.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveGroupCallbackProxy : public IRemoteProxy<IAppGroupCallback> {
public:
    explicit BundleActiveGroupCallbackProxy(const sptr<IRemoteObject>& impl);
    ~BundleActiveGroupCallbackProxy() override;
    DISALLOW_COPY_AND_MOVE(BundleActiveGroupCallbackProxy);
    /*
    * function: OnAppGroupChanged, bundleGroupChanged callback, IPC proxy, send message to stub.
    * parameters: AppGroupCallbackInfo
    * return: void.
    */
    void OnAppGroupChanged(
        const AppGroupCallbackInfo& appGroupCallbackInfo) override;

private:
    static inline BrokerDelegator<BundleActiveGroupCallbackProxy> delegator_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // APP_GROUP_CALLBACK_PROXY_H
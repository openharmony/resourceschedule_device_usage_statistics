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

#ifndef BUNDLE_ACTIVE_SHUTDOWN_CALLBACK_PROXY_H
#define BUNDLE_ACTIVE_SHUTDOWN_CALLBACK_PROXY_H

#include "ibundle_active_service.h"
#include "ishutdown_callback.h"
#include "nocopyable.h"

namespace OHOS {
namespace DeviceUsageStats {
using IShutdownCallback = OHOS::PowerMgr::IShutdownCallback;
class BundleActiveShutdownCallbackProxy : public IRemoteProxy<IShutdownCallback> {
public:
    explicit BundleActiveShutdownCallbackProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IShutdownCallback>(impl) {}
    ~BundleActiveShutdownCallbackProxy() = default;
    DISALLOW_COPY_AND_MOVE(BundleActiveShutdownCallbackProxy);
    void ShutdownCallback() override;
private:
    static inline BrokerDelegator<BundleActiveShutdownCallbackProxy> delegator_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_SHUTDOWN_CALLBACK_PROXY_H


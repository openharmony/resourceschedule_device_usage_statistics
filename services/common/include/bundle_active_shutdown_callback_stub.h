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

#ifndef BUNDLE_ACTIVE_SHUTDOWN_CALLBACK_STUB_H
#define BUNDLE_ACTIVE_SHUTDOWN_CALLBACK_STUB_H

#include "ishutdown_callback.h"
#include "nocopyable.h"

#include "ibundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
using IShutdownCallback = OHOS::PowerMgr::IShutdownCallback;
class BundleActiveShutdownCallbackStub : public IRemoteStub<IShutdownCallback> {
public:
    DISALLOW_COPY_AND_MOVE(BundleActiveShutdownCallbackStub);
    BundleActiveShutdownCallbackStub() = default;
    virtual ~BundleActiveShutdownCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply, MessageOption &option) override;
private:
    void ShutdownStub();
};
}
}

#endif
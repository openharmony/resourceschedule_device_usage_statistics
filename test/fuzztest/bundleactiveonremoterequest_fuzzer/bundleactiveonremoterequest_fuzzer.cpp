/*
 * Copyright (c) Huawei Device Co., Ltd. 2021-2022. All rights reserved.
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

#include "bundleactiveonremoterequest_fuzzer.h"

#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "bundle_active_stub.h"
#define private public
#include "bundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
    constexpr int32_t MIN_LEN = 4;
    constexpr int32_t MAX_CODE_TEST = 15; // current max code is 9
    static bool isInited = false;

    bool DoInit()
    {
        auto instance = DelayedSingleton<BundleActiveService>::GetInstance();
        if (!instance->runner_) {
            instance->runner_ = AppExecFwk::EventRunner::Create("device_usage_stats_init_handler");
        }
        if (!instance->runner_) {
            return false;
        }

        if (!instance->handler_) {
            instance->handler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(instance->runner_);
        }
        if (!instance->handler_) {
            return false;
        }
        instance->InitNecessaryState();
        return true;
    }

    int32_t OnRemoteRequest(uint32_t code, MessageParcel& data)
    {
        MessageParcel reply;
        MessageOption option;
        int32_t ret = DelayedSingleton<BundleActiveService>::GetInstance()->OnRemoteRequest(code, data, reply, option);
        return ret;
    }

    void IpcBundleActiveStubFuzzTest(const uint8_t* data, size_t size)
    {
        if (size <= MIN_LEN) {
            return;
        }

        MessageParcel dataMessageParcel;
        if (!dataMessageParcel.WriteInterfaceToken(BundleActiveStub::GetDescriptor())) {
            return;
        }

        uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
        if (code > MAX_CODE_TEST) {
            return;
        }

        size -= sizeof(uint32_t);

        dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), size);
        dataMessageParcel.RewindRead(0);

        if (!isInited) {
            isInited = DoInit();
        }
        if (isInited) {
            OnRemoteRequest(code, dataMessageParcel);
        }
    }
} // namespace DeviceUsageStats
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DeviceUsageStats::IpcBundleActiveStubFuzzTest(data, size);
    return 0;
}


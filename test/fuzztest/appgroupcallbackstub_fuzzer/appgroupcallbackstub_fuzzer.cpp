/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "appgroupcallbackstub_fuzzer.h"

#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "app_group_callback_stub.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace DeviceUsageStats {
    constexpr uint32_t U32_AT_SIZE = 4;
    constexpr uint32_t MAX_CODE = 2; // current max code is 2
    constexpr uint8_t TWENTYFOUR = 24;
    constexpr uint8_t SIXTEEN = 16;
    constexpr uint8_t EIGHT = 8;
    constexpr uint8_t TWO = 2;
    constexpr uint8_t THREE = 3;
    const std::u16string APP_GOUNP_ACTIVE_TOKEN = u"OHOS.DeviceUsageStats.IAppGroupCallback";

    class TestAppGroupChangeCallback : public AppGroupCallbackStub {
    public:
        ErrCode OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo) override;
    };

    ErrCode TestAppGroupChangeCallback::OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
    {
        return ERR_OK;
    }

    uint32_t GetU32Data(const char* ptr)
    {
        return (ptr[0] << TWENTYFOUR) | (ptr[1] << SIXTEEN) | (ptr[TWO] << EIGHT) | (ptr[THREE]);
    }

    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        DelayedSingleton<FuzztestHelper>::GetInstance()->NativeTokenGet();
        uint32_t code = GetU32Data(data);
        MessageParcel datas;
        datas.WriteInterfaceToken(APP_GOUNP_ACTIVE_TOKEN);
        datas.WriteBuffer(data, size);
        datas.RewindRead(0);
        MessageParcel reply;
        MessageOption option;
        DelayedSingleton<TestAppGroupChangeCallback>::GetInstance()->OnRemoteRequest(code % MAX_CODE,
            datas, reply, option);
        return true;
    }
} // namespace DeviceUsageStats
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::DeviceUsageStats::U32_AT_SIZE) {
        return 0;
    }
    char* ch = (char *)malloc(size + 1);
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }

    OHOS::DeviceUsageStats::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}


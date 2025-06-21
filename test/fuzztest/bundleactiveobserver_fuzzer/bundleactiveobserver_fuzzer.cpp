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

#include "bundleactiveobserver_fuzzer.h"

#include "accesstoken_kit.h"
#include "app_mgr_interface.h"

#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "bundle_active_service.h"
#include "iapp_group_callback.h"
#include "bundle_active_config_reader.h"

namespace OHOS {
namespace DeviceUsageStats {
    constexpr uint32_t U32_AT_SIZE = 4;
    constexpr uint8_t TWENTYFOUR = 24;
    constexpr uint8_t SIXTEEN = 16;
    constexpr uint8_t EIGHT = 8;
    constexpr uint8_t TWO = 2;
    constexpr uint8_t THREE = 3;
    bool g_isInited = false;

    uint32_t GetU32Data(const char* ptr)
    {
        return (ptr[0] << TWENTYFOUR) | (ptr[1] << SIXTEEN) | (ptr[TWO] << EIGHT) | (ptr[THREE]);
    }

    bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
    {
        DelayedSingleton<FuzztestHelper>::GetInstance()->NativeTokenGet();
        if (!g_isInited) {
            DelayedSingleton<BundleActiveService>::GetInstance()->InitService();
            g_isInited = true;
        }
        sptr<IAppGroupCallback> appGroupCallback = nullptr;
        DelayedSingleton<BundleActiveService>::GetInstance()->RegisterAppGroupCallBack(appGroupCallback);
        DelayedSingleton<BundleActiveService>::GetInstance()->UnRegisterAppGroupCallBack(appGroupCallback);
        bool result = false;
        int32_t userId = static_cast<int32_t>(GetU32Data(data));
        std::string inputBundlName(data);
        DelayedSingleton<BundleActiveService>::GetInstance()->IsBundleIdle(result, inputBundlName, userId);
        DelayedSingleton<BundleActiveService>::GetInstance()->IsBundleUsePeriod(result, inputBundlName, userId);
        DelayedSingleton<BundleActiveConfigReader>::GetInstance()->GetApplicationUsePeriodicallyConfig();
        DelayedSingleton<BundleActiveConfigReader>::GetInstance()->GetAppHighFrequencyPeriodThresholdConfig();
        DelayedSingleton<BundleActiveConfigReader>::GetInstance()->LoadConfigFile(inputBundlName.c_str());
        return true;
    }

    bool BundleActiveServiceDumpFuzzTest(const char* data, size_t size)
    {
        int32_t fd = static_cast<int32_t>(GetU32Data(data));
        std::vector<std::u16string> args;
        args = {to_utf16(std::to_string(GetU32Data(data)))};
        DelayedSingleton<BundleActiveService>::GetInstance()->Dump(fd, args);
        DelayedSingleton<BundleActiveService>::GetInstance()->AllowDump();

        std::vector<std::string> dumpOption = {
            std::to_string(GetU32Data(data)), std::to_string(GetU32Data(data)),
            std::to_string(GetU32Data(data)), std::to_string(GetU32Data(data)),
            std::to_string(GetU32Data(data)), std::to_string(GetU32Data(data))};
        std::vector<std::string> dumpInfo;
        DelayedSingleton<BundleActiveService>::GetInstance()->ShellDump(dumpOption, dumpInfo);
        dumpInfo.clear();
        DelayedSingleton<BundleActiveService>::GetInstance()->DumpEvents(dumpOption, dumpInfo);
        dumpInfo.clear();
        DelayedSingleton<BundleActiveService>::GetInstance()->DumpPackageUsage(dumpOption, dumpInfo);
        dumpInfo.clear();
        DelayedSingleton<BundleActiveService>::GetInstance()->DumpModuleUsage(dumpOption, dumpInfo);
        dumpInfo.clear();
        DelayedSingleton<BundleActiveService>::GetInstance()->DumpHighFreqHourUsage(dumpOption, dumpInfo);
        std::string result;
        DelayedSingleton<BundleActiveService>::GetInstance()->DumpUsage(result);
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
    OHOS::DeviceUsageStats::BundleActiveServiceDumpFuzzTest(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}


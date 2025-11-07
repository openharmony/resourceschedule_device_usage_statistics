/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "bundleactivemodulerecord_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <fuzzer/FuzzedDataProvider.h>

#include "bundle_active_form_record.h"
#include "bundle_active_module_record.h"
#include "message_parcel.h"

namespace OHOS {
namespace DeviceUsageStats {
namespace {

// ======= 常量定义区 =======
constexpr size_t MAX_OPS = 32;
constexpr size_t MAX_NAME_LEN = 32;
constexpr size_t MAX_DEVICE_ID_LEN = 16;
constexpr size_t MAX_FORM_NAME_LEN = 16;

// ======= 动作类型枚举 =======
enum class ActionType : uint8_t {
    ADD_FORM = 0,
    REMOVE_FORM,
    UPDATE_MODULE,
    COPY_AND_COMPARE,
    FUZZ_MARSHALLING,
    UPDATE_INFO,
    ACTION_MAX
};

// ======= 构建记录对象 =======
BundleActiveModuleRecord BuildRecord(FuzzedDataProvider &fdp)
{
    BundleActiveModuleRecord record;
    record.bundleName_ = fdp.ConsumeRandomLengthString(MAX_NAME_LEN);
    record.moduleName_ = fdp.ConsumeRandomLengthString(MAX_NAME_LEN);
    record.deviceId_ = fdp.ConsumeRandomLengthString(MAX_DEVICE_ID_LEN);
    record.abilityName_ = fdp.ConsumeRandomLengthString(MAX_NAME_LEN);
    record.uid_ = fdp.ConsumeIntegral<int32_t>();
    record.userId_ = fdp.ConsumeIntegral<int32_t>();
    record.lastModuleUsedTime_ = fdp.ConsumeIntegral<int64_t>();
    record.launchedCount_ = fdp.ConsumeIntegral<int32_t>();
    return record;
}

// ======= 序列化与反序列化测试 =======
void FuzzMarshalling(BundleActiveModuleRecord &record)
{
    MessageParcel parcel;
    if (record.Marshalling(parcel)) {
        parcel.RewindRead(0);
        std::unique_ptr<BundleActiveModuleRecord> restored(BundleActiveModuleRecord::Unmarshalling(parcel));
        if (restored != nullptr) {
            (void)restored->ToString();
        }
    }
}

// ======= 模块记录驱动函数 =======
void DriveModuleRecord(BundleActiveModuleRecord &record, FuzzedDataProvider &fdp)
{
    size_t ops = fdp.ConsumeIntegralInRange<size_t>(1, MAX_OPS);
    for (size_t i = 0; i < ops && fdp.remaining_bytes() > 0; i++) {
        uint8_t actionValue = fdp.ConsumeIntegral<uint8_t>() % static_cast<uint8_t>(ActionType::ACTION_MAX);
        ActionType action = static_cast<ActionType>(actionValue);

        switch (action) {
            case ActionType::ADD_FORM: {
                std::string formName = fdp.ConsumeRandomLengthString(MAX_FORM_NAME_LEN);
                int32_t dimension = fdp.ConsumeIntegral<int32_t>();
                int64_t formId = fdp.ConsumeIntegral<int64_t>();
                int64_t timeStamp = fdp.ConsumeIntegral<int64_t>();
                int32_t uid = fdp.ConsumeIntegral<int32_t>();
                record.AddOrUpdateOneFormRecord(formName, dimension, formId, timeStamp, uid);
                break;
            }
            case ActionType::REMOVE_FORM: {
                std::string formName = fdp.ConsumeRandomLengthString(MAX_FORM_NAME_LEN);
                int32_t dimension = fdp.ConsumeIntegral<int32_t>();
                int64_t formId = fdp.ConsumeIntegral<int64_t>();
                record.RemoveOneFormRecord(formName, dimension, formId);
                break;
            }
            case ActionType::UPDATE_MODULE: {
                int64_t timeStamp = fdp.ConsumeIntegral<int64_t>();
                record.UpdateModuleRecord(timeStamp);
                break;
            }
            case ActionType::COPY_AND_COMPARE: {
                BundleActiveModuleRecord copy(record);
                copy = record;
                (void)BundleActiveModuleRecord::cmp(copy, record);
                (void)copy.ToString();
                break;
            }
            case ActionType::FUZZ_MARSHALLING:
                FuzzMarshalling(record);
                break;
            case ActionType::UPDATE_INFO:
                record.bundleName_ = fdp.ConsumeRandomLengthString(MAX_DEVICE_ID_LEN);
                record.moduleName_ = fdp.ConsumeRandomLengthString(MAX_DEVICE_ID_LEN);
                record.userId_ = fdp.ConsumeIntegral<int32_t>();
                record.uid_ = fdp.ConsumeIntegral<int32_t>();
                break;
            default:
                break;
        }
    }

    if (fdp.ConsumeBool()) {
        FuzzMarshalling(record);
    }
}

} // namespace

// ======= 模糊测试主入口 =======
bool BundleActiveModuleRecordFuzzTest(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return false;
    }
    FuzzedDataProvider fdp(data, size);
    BundleActiveModuleRecord record = BuildRecord(fdp);
    DriveModuleRecord(record, fdp);
    return true;
}

} // namespace DeviceUsageStats
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::DeviceUsageStats::BundleActiveModuleRecordFuzzTest(data, size);
    return 0;
}

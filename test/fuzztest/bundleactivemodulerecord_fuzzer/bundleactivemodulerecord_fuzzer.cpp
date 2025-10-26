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
constexpr size_t MAX_OPS = 32;

BundleActiveModuleRecord BuildRecord(FuzzedDataProvider &fdp)
{
    BundleActiveModuleRecord record;
    record.bundleName_ = fdp.ConsumeRandomLengthString(32);
    record.moduleName_ = fdp.ConsumeRandomLengthString(32);
    record.deviceId_ = fdp.ConsumeRandomLengthString(16);
    record.abilityName_ = fdp.ConsumeRandomLengthString(32);
    record.uid_ = fdp.ConsumeIntegral<int32_t>();
    record.userId_ = fdp.ConsumeIntegral<int32_t>();
    record.lastModuleUsedTime_ = fdp.ConsumeIntegral<int64_t>();
    record.launchedCount_ = fdp.ConsumeIntegral<int32_t>();
    return record;
}

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

void DriveModuleRecord(BundleActiveModuleRecord &record, FuzzedDataProvider &fdp)
{
    size_t ops = fdp.ConsumeIntegralInRange<size_t>(1, MAX_OPS);
    for (size_t i = 0; i < ops && fdp.remaining_bytes() > 0; i++) {
        uint8_t action = fdp.ConsumeIntegral<uint8_t>() % 6; // six actions
        switch (action) {
            case 0: {
                std::string formName = fdp.ConsumeRandomLengthString(16);
                int32_t dimension = fdp.ConsumeIntegral<int32_t>();
                int64_t formId = fdp.ConsumeIntegral<int64_t>();
                int64_t timeStamp = fdp.ConsumeIntegral<int64_t>();
                int32_t uid = fdp.ConsumeIntegral<int32_t>();
                record.AddOrUpdateOneFormRecord(formName, dimension, formId, timeStamp, uid);
                break;
            }
            case 1: {
                std::string formName = fdp.ConsumeRandomLengthString(16);
                int32_t dimension = fdp.ConsumeIntegral<int32_t>();
                int64_t formId = fdp.ConsumeIntegral<int64_t>();
                record.RemoveOneFormRecord(formName, dimension, formId);
                break;
            }
            case 2: {
                int64_t timeStamp = fdp.ConsumeIntegral<int64_t>();
                record.UpdateModuleRecord(timeStamp);
                break;
            }
            case 3: {
                BundleActiveModuleRecord copy(record);
                copy = record;
                (void)BundleActiveModuleRecord::cmp(copy, record);
                (void)copy.ToString();
                break;
            }
            case 4:
                FuzzMarshalling(record);
                break;
            case 5:
                record.bundleName_ = fdp.ConsumeRandomLengthString(16);
                record.moduleName_ = fdp.ConsumeRandomLengthString(16);
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
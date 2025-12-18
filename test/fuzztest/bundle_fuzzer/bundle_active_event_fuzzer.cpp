/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

#ifndef ErrCode
using ErrCode = int;
#endif

#include "message_parcel.h"
#include "bundle_active_event.h"
#include "bundle_active_event_vec_raw_data.h"

using namespace OHOS::DeviceUsageStats;
using namespace OHOS;

namespace OHOS {
bool FuzzBundleActiveEventUnmarshalling(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }

    MessageParcel parcel;
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);

    BundleActiveEventVecRawData eventVecRawData;
    eventVecRawData.data = reinterpret_cast<const void*>(data);
    eventVecRawData.size = static_cast<uint32_t>(size);

    std::vector<BundleActiveEvent> out;
    eventVecRawData.Unmarshalling(out);

    eventVecRawData.data = nullptr;
    return true;
}
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::FuzzBundleActiveEventUnmarshalling(data, size);
    return 0;
}

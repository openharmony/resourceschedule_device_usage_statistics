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

#ifndef BUNDLE_ACTIVE_EVENT_VEC_RAW_DATA_H
#define BUNDLE_ACTIVE_EVENT_VEC_RAW_DATA_H

#include "parcel.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "bundle_active_event.h"
#include "bundle_state_inner_errors.h"

namespace OHOS {
namespace DeviceUsageStats {
struct BundleActiveEventVecRawData {
    uint32_t size;
    const void *data;
    std::string serializedData;
    ErrCode Marshalling(const std::vector<BundleActiveEvent> &inData);
    ErrCode Unmarshalling(std::vector<BundleActiveEvent> &out) const;
    ErrCode RawDataCpy(const void *inData);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_EVENT_VEC_RAW_DATA_H


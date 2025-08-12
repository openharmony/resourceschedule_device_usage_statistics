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

#include "bundle_active_event_vec_raw_data.h"

#include "bundle_active_log.h"
#include "string_ex.h"

namespace OHOS {
namespace DeviceUsageStats {
    constexpr size_t PARCEL_MAX_CAPACITY_IN_BYTE = 1024 * 1024 * 10;
    ErrCode BundleActiveEventVecRawData::Marshalling(const std::vector<BundleActiveEvent> &inData)
    {
        std::stringstream ss;
        size_t eventCount = inData.size();
        ss.write(reinterpret_cast<const char *>(&eventCount), sizeof(eventCount));
        OHOS::Parcel parcel;
        parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY_IN_BYTE);
        for (const auto &event : inData) {
            parcel.RewindWrite(0);
            if (!event.Marshalling(parcel)) {
                BUNDLE_ACTIVE_LOGE("Marshalling failed,budle:%{public}s", event.bundleName_.c_str());
                return ERR_INVALID_VALUE;
            }
            size_t eventDataSize = parcel.GetWritePosition();
            ss.write(reinterpret_cast<const char *>(&eventDataSize), sizeof(eventDataSize));
            ss.write(reinterpret_cast<const char *>(parcel.GetData()), eventDataSize);
        }
        serializedData = ss.str();
        size = serializedData.length();
        data = reinterpret_cast<const void *>(serializedData.data());
        return ERR_OK;
    }

    ErrCode BundleActiveEventVecRawData::Unmarshalling(std::vector<BundleActiveEvent> &out) const
    {
        std::stringstream ss;
        if (data == nullptr) {
            BUNDLE_ACTIVE_LOGE("Unmarshalling data is null");
            return ERR_INVALID_VALUE;
        }
        ss.write(reinterpret_cast<const char *>(data), size);
        size_t eventCount = 0;
        ss.read(reinterpret_cast<char *>(&eventCount), sizeof(eventCount));
        for (uint32_t i = 0; i < eventCount; i++) {
            OHOS::Parcel parcel;
            if (!parcel.SetMaxCapacity(PARCEL_MAX_CAPACITY_IN_BYTE)) {
                BUNDLE_ACTIVE_LOGE("parcel SetMaxCapacity failed");
                return ERR_INVALID_VALUE;
            }
            size_t eventDataSize = 0;
            ss.read(reinterpret_cast<char *>(&eventDataSize), sizeof(eventDataSize));
            std::string eventBuf;
            eventBuf.resize(eventDataSize);
            ss.read(eventBuf.data(), eventDataSize);
            parcel.WriteBuffer(eventBuf.data(), eventDataSize);

            BundleActiveEvent bundleActiveEvent;
            BundleActiveEvent* result = bundleActiveEvent.Unmarshalling(parcel);
            if (!result) {
                BUNDLE_ACTIVE_LOGE("bundleActiveEvent Unmarshalling failed, eventDataSize:%{public}zu", eventDataSize);
                return ERR_INVALID_VALUE;
            }
            out.push_back(*result);
        }
        return ERR_OK;
    }

    ErrCode BundleActiveEventVecRawData::RawDataCpy(const void *inData)
    {
        if (inData == nullptr) {
            BUNDLE_ACTIVE_LOGE("RawDataCpy inData is null");
            return ERR_INVALID_VALUE;
        }
        std::stringstream ss;
        ss.write(reinterpret_cast<const char *>(inData), size);
        serializedData = ss.str();
        data = reinterpret_cast<const void *>(serializedData.data());
        return ERR_OK;
    }
}  // namespace DeviceUsageStats
}  // namespace OHOS


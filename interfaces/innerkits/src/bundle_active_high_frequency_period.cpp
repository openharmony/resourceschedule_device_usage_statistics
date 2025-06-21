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

#include <sstream>

#include "bundle_active_high_frequency_period.h"

namespace OHOS {
namespace DeviceUsageStats {
namespace {
    static const int32_t MAX_HOUR_NUM = 24;
}
BundleActiveHighFrequencyPeriod::BundleActiveHighFrequencyPeriod()
{
    bundleName_ = "";
}

BundleActiveHighFrequencyPeriod::BundleActiveHighFrequencyPeriod(
    const std::string& bundleName, const std::vector<int32_t>& highFreqHours)
    : bundleName_(bundleName), highFreqHours_(highFreqHours)
{
}

BundleActiveHighFrequencyPeriod::BundleActiveHighFrequencyPeriod(const BundleActiveHighFrequencyPeriod& orig)
{
    bundleName_ = orig.bundleName_;
    highFreqHours_ = orig.highFreqHours_;
}

bool BundleActiveHighFrequencyPeriod::Marshalling(Parcel &parcel) const
{
    if (highFreqHours_.size() > MAX_HOUR_NUM) {
        return false;
    }
    if (parcel.WriteString(bundleName_) && parcel.WriteUint32(highFreqHours_.size())) {
        for (auto highFreqHour : highFreqHours_) {
            if (!parcel.WriteInt32(highFreqHour)) {
                return false;
            }
        }
        return true;
    }
    return false;
}

BundleActiveHighFrequencyPeriod *BundleActiveHighFrequencyPeriod::Unmarshalling(Parcel& parcel)
{
    BundleActiveHighFrequencyPeriod *result = new (std::nothrow) BundleActiveHighFrequencyPeriod();
    if (result == nullptr) {
        return nullptr;
    }
    result->bundleName_ = parcel.ReadString();
    uint32_t size;
    if (!parcel.ReadUint32(size) || size > MAX_HOUR_NUM) {
        delete result;
        return nullptr;
    }
    result->highFreqHours_.clear();
    for (uint32_t i = 0; i < size; i++) {
        int32_t tmp;
        if (!parcel.ReadInt32(tmp)) {
            delete result;
            return nullptr;
        }
        result->highFreqHours_.emplace_back(tmp);
    }
    return result;
}

std::string BundleActiveHighFrequencyPeriod::ToString() const
{
    std::stringstream highFreqHoursToString;
    highFreqHoursToString << "bundle name is ";
    highFreqHoursToString << this->bundleName_;
    highFreqHoursToString << ", highFreqHours_ is ";
    for (size_t i = 0; i < highFreqHours_.size(); ++i) {
        if (i != 0) {
            highFreqHoursToString << ", ";
        }
        highFreqHoursToString << highFreqHours_[i];
    }
    highFreqHoursToString << "\n";
    return highFreqHoursToString.str();
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


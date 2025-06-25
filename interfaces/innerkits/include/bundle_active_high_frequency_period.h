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

#ifndef BUNDLE_ACTIVE_HIGH_FREQUENCY_PERIOD_H
#define BUNDLE_ACTIVE_HIGH_FREQUENCY_PERIOD_H

#include <cstdint>
#include <iosfwd>
#include <memory>
#include "parcel.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveHighFrequencyPeriod : public Parcelable {
public:
    std::string bundleName_;
    std::vector<int32_t> highFreqHours_;

    /*
    * function: BundleActiveHighFrequencyPeriod, default constructor.
    */
    BundleActiveHighFrequencyPeriod();

    /*
    * function: BundleActiveHighFrequencyPeriod, copy constructor.
    * parameters: orig
    */
    BundleActiveHighFrequencyPeriod(const BundleActiveHighFrequencyPeriod& orig);

    /*
    * function: BundleActiveHighFrequencyPeriod, use bundleName, highFreqHours to construct
    * object. parameters: bundleName, highFreqHours
    */
    BundleActiveHighFrequencyPeriod(
        const std::string& bundleName, const std::vector<int32_t>& highFreqHours);

    /*
    * function: ~BundleActiveHighFrequencyPeriod, default destructor.
    */
    ~BundleActiveHighFrequencyPeriod() {}

    /*
    * function: Marshalling, mashalling BundleActiveHighFrequencyPeriod object to parcel.
    * parameters: parcel
    * return: result of mashalling, true means successful, flase means failed.
    */
    virtual bool Marshalling(Parcel &parcel) const override;

    /*
    * function: Unmarshalling, Unmashalling BundleActiveHighFrequencyPeriod object from parcel.
    * parameters: parcel
    * return: point to a BundleActiveHighFrequencyPeriod.
    */
    static BundleActiveHighFrequencyPeriod *Unmarshalling(Parcel &parcel);

    /*
    * function: ToString, change app high-frequency info to string.
    * return: string of from bundleName and highFreqHours
    */
    std::string ToString() const;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_HIGH_FREQUENCY_PERIOD_H


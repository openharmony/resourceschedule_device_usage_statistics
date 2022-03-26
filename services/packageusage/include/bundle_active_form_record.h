/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_ACTIVE_FORM_RECORD_H
#define BUNDLE_ACTIVE_FORM_RECORD_H

#include <string>

#include "ibundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveFormRecord : public Parcelable {
public:
    std::string formName_;
    int32_t formDimension_;
    int64_t formId_;
    int64_t formLastUsedTime_;
    int32_t count_;
    int32_t userId_;
    void UpdateFormRecord(const int64_t timeStamp);
    BundleActiveFormRecord(const BundleActiveFormRecord& orig);
    virtual bool Marshalling(Parcel &parcel) const override;
    std::shared_ptr<BundleActiveFormRecord> UnMarshalling(Parcel &parcel);
    BundleActiveFormRecord();
    BundleActiveFormRecord(const std::string formName, const int32_t formDimension, const int64_t formId,
        const int64_t timeStamp, const int32_t userId);
    ~BundleActiveFormRecord() {}
    bool operator==(const BundleActiveFormRecord& formRecord) const
    {
        return (this->formId_ == formRecord.formId_);
    }
    bool operator==(const int64_t formId) const
    {
        return (this->formId_ == formId);
    }
    static bool cmp(const BundleActiveFormRecord& formRecordA, const BundleActiveFormRecord& formRecordB);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_FORM_RECORD_H


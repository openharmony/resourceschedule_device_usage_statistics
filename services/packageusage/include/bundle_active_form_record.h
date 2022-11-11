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

#include <cstdint>
#include <iosfwd>
#include <memory>
#include "parcel.h"

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

public:
    /*
    * function: BundleActiveFormRecord, copy constructor.
    * parameters: orig
    */
    BundleActiveFormRecord(const BundleActiveFormRecord& orig);
    /*
    * function: BundleActiveFormRecord, default constructor.
    */
    BundleActiveFormRecord();
    /*
    * function: BundleActiveFormRecord, use formName, formDimension, formId, timeStamp, userId to construct object.
    * parameters: formName, formDimension, formId, timeStamp, userId
    */
    BundleActiveFormRecord(const std::string formName, const int32_t formDimension, const int64_t formId,
        const int64_t timeStamp, const int32_t userId);
    /*
    * function: ~BundleActiveFormRecord, default destructor.
    */
    ~BundleActiveFormRecord() {}
    /*
    * function: UpdateFormRecord, update form record by timestamp.
    * parameters: timeStamp
    */
    void UpdateFormRecord(const int64_t timeStamp);
    /*
    * function: operator==, override operator ==.
    * parameters: formRecord
    * return: true if the formId_ is same.
    */
    bool operator==(const BundleActiveFormRecord& formRecord) const
    {
        return (this->formId_ == formRecord.formId_);
    }
    /*
    * function: operator==, override operator ==.
    * parameters: formId
    * return: true if the formId_ is same.
    */
    bool operator==(const int64_t formId) const
    {
        return (this->formId_ == formId);
    }
    /*
    * function: cmp, compare two BundleActiveFormRecord.
    * parameters: formRecordA, formRecordB
    * return: true if formRecordA.count_ > formRecordB.
    */
    static bool cmp(const BundleActiveFormRecord& formRecordA, const BundleActiveFormRecord& formRecordB);
    /*
    * function: Marshalling, mashalling form record object to parcel.
    * parameters: parcel
    * return: result of mashalling, true means successful, flase means failed.
    */
    bool Marshalling(Parcel &parcel) const override;
    /*
    * function: UnMarshalling, Unmashalling record object from parcel.
    * parameters: parcel
    * return: point to a BundleActiveFormRecord.
    */
    std::shared_ptr<BundleActiveFormRecord> UnMarshalling(Parcel &parcel);
    /*
    * function: ToString, change form record object to string.
    * return: string of form name, form id, form dimension, last used time and touch count.
    */
    std::string ToString();
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_FORM_RECORD_H


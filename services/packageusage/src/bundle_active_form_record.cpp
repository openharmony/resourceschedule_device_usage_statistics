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

#include "bundle_active_form_record.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveFormRecord::BundleActiveFormRecord()
{
    formName_ = "";
    formDimension_ = 0;
    formId_ = 0;
    formLastUsedTime_ = -1;
    count_ = 0;
    userId_ = -1;
}

BundleActiveFormRecord::BundleActiveFormRecord(const std::string formName, const int32_t formDimension,
    const int64_t formId, const int64_t timeStamp, const int32_t userId)
{
    formName_ = formName;
    formDimension_ = formDimension;
    formId_ = formId;
    formLastUsedTime_ = timeStamp;
    count_ = 1;
    userId_ = userId;
}

BundleActiveFormRecord::BundleActiveFormRecord(const BundleActiveFormRecord& orig)
{
    formName_ = orig.formName_;
    formDimension_ = orig.formDimension_;
    formId_ = orig.formId_;
    formLastUsedTime_ = orig.formLastUsedTime_;
    count_ = orig.count_;
    userId_ = orig.userId_;
}

void BundleActiveFormRecord::UpdateFormRecord(const int64_t timeStamp)
{
    if (formLastUsedTime_ < timeStamp) {
        formLastUsedTime_ = timeStamp;
        count_++;
    }
}

bool BundleActiveFormRecord::Marshalling(Parcel &parcel) const
{
    if (parcel.WriteString(formName_) &&
        parcel.WriteInt32(formDimension_) &&
        parcel.WriteInt64(formId_) &&
        parcel.WriteInt64(formLastUsedTime_) &&
        parcel.WriteInt32(count_)) {
        return true;
    }
    return false;
}

std::shared_ptr<BundleActiveFormRecord> BundleActiveFormRecord::UnMarshalling(Parcel &parcel)
{
    std::shared_ptr<BundleActiveFormRecord> result = std::make_shared<BundleActiveFormRecord>();
    result->formName_ = parcel.ReadString();
    result->formDimension_ = parcel.ReadInt32();
    result->formId_ = parcel.ReadInt64();
    result->formLastUsedTime_ = parcel.ReadInt64();
    result->count_ = parcel.ReadInt32();
    return result;
}

bool BundleActiveFormRecord::cmp(const BundleActiveFormRecord& formRecordA,
    const BundleActiveFormRecord& formRecordB)
{
    return formRecordA.count_ > formRecordB.count_;
}

std::string BundleActiveFormRecord::ToString()
{
    return "form name is " + this->formName_ +
        ", form dimension is " + std::to_string(this->formDimension_) +
        ", form id is " + std::to_string(this->formId_) +
        ", last used time stamp is" + std::to_string(this->formLastUsedTime_) +
        ", touch count is " + std::to_string(this->count_) + "\n";
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


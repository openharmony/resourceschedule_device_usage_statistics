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

#include "bundle_active_module_record.h"

namespace OHOS {
namespace DeviceUsageStats {
void BundleActiveModuleRecord::AddOrUpdateOneFormRecord(const std::string formName, const int32_t formDimension,
    const int64_t formId, const int64_t timeStamp)
{
    BundleActiveFormRecord newFormRecord(formName, formDimension, formId, timeStamp, userId_);
    for (auto& formRecord : formRecords_) {
        if (formRecord == newFormRecord) {
            formRecord.UpdateFormRecord(timeStamp);
            return;
        }
    }
    formRecords_.emplace_back(newFormRecord);
}

void BundleActiveModuleRecord::RemoveOneFormRecord(const std::string formName, const int32_t formDimension,
    const int64_t formId)
{
    for (auto it = formRecords_.begin(); it != formRecords_.end(); it++) {
        if (*it == formId) {
            formRecords_.erase(it);
            return;
        }
    }
}

void BundleActiveModuleRecord::UpdateModuleRecord(int64_t timeStamp)
{
    if (lastModuleUsedTime_ < timeStamp) {
        lastModuleUsedTime_ = timeStamp;
        launchedCount_++;
    }
}

BundleActiveModuleRecord::BundleActiveModuleRecord()
{
    deviceId_ = "";
    bundleName_ = ""; // in database
    moduleName_ = ""; // in database
    abilityName_ = "";
    appLabelId_ = 0;
    labelId_ = 0;
    descriptionId_ = 0;
    abilityLableId_ = 0;
    abilityDescriptionId_ = 0;
    abilityIconId_ = 0;
    launchedCount_ = 0;
    lastModuleUsedTime_ = -1;
    removed_ = false;
    installFreeSupported_ = false;
    isNewAdded_ = false;
    userId_ = -1;
}

bool BundleActiveModuleRecord::Marshalling(Parcel &parcel) const
{
    if (parcel.WriteString(deviceId_) &&
        parcel.WriteString(bundleName_) &&
        parcel.WriteString(moduleName_) &&
        parcel.WriteString(abilityName_) &&
        parcel.WriteUint32(appLabelId_) &&
        parcel.WriteUint32(descriptionId_) &&
        parcel.WriteUint32(abilityLableId_) &&
        parcel.WriteUint32(abilityDescriptionId_) &&
        parcel.WriteUint32(abilityIconId_) &&
        parcel.WriteInt32(launchedCount_) &&
        parcel.WriteInt64(lastModuleUsedTime_) &&
        parcel.WriteUint32(formRecords_.size())
        ) {
            for (auto formRecord : formRecords_) {
                formRecord.Marshalling(parcel);
            }
            return true;
        }
    return false;
}

std::shared_ptr<BundleActiveModuleRecord> BundleActiveModuleRecord::UnMarshalling(Parcel &parcel)
{
    std::shared_ptr<BundleActiveModuleRecord> result = std::make_shared<BundleActiveModuleRecord>();
    result->deviceId_ = parcel.ReadString();
    result->bundleName_ = parcel.ReadString();
    result->moduleName_ = parcel.ReadString();
    result->abilityName_ = parcel.ReadString();
    result->appLabelId_ = parcel.ReadUint32();
    result->descriptionId_ = parcel.ReadUint32();
    result->abilityLableId_ = parcel.ReadUint32();
    result->abilityDescriptionId_ = parcel.ReadUint32();
    result->abilityIconId_ = parcel.ReadUint32();
    result->launchedCount_ = parcel.ReadInt32();
    result->lastModuleUsedTime_ = parcel.ReadInt64();
    uint32_t size = parcel.ReadUint32();
    std::shared_ptr<BundleActiveFormRecord> tmp = std::make_shared<BundleActiveFormRecord>();
    for (uint32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(parcel);
        if (!tmp) {
            continue;
        }
        result->formRecords_.emplace_back(*tmp);
    }
    return result;
}

bool BundleActiveModuleRecord::cmp(const BundleActiveModuleRecord& moduleRecordA,
    const BundleActiveModuleRecord& moduleRecordB)
{
    return moduleRecordA.lastModuleUsedTime_ > moduleRecordB.lastModuleUsedTime_;
}

std::string BundleActiveModuleRecord::ToString()
{
    return "bundle name is " + this->bundleName_ +
        ", module name is " + this->moduleName_ +
        ", last used time stamp is " + std::to_string(this->lastModuleUsedTime_) +
        ", module is used for " + std::to_string(this->launchedCount_) + " times\n";
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


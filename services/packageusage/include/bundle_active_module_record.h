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

#ifndef BUNDLE_ACTIVE_MODULE_RECORD_H
#define BUNDLE_ACTIVE_MODULE_RECORD_H

#include "bundle_active_form_record.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveModuleRecord : public Parcelable {
public:
    void AddOrUpdateOneFormRecord(const std::string formName, const int32_t formDimension,
        const int64_t formId, const int64_t timeStamp);
    void RemoveOneFormRecord(const std::string formName, const int32_t formDimension,
        const int64_t formId);
    void UpdateModuleRecord(int64_t timeStamp);
    BundleActiveModuleRecord();
    ~BundleActiveModuleRecord() {}
    static bool cmp(const BundleActiveModuleRecord& moduleRecordA, const BundleActiveModuleRecord& moduleRecordB);
    bool Marshalling(Parcel &parcel) const override;
    std::shared_ptr<BundleActiveModuleRecord> UnMarshalling(Parcel &parcel);
    std::string ToString();

public:
    std::string deviceId_;
    std::string bundleName_;
    std::string moduleName_;
    std::string abilityName_;
    uint32_t appLabelId_;
    uint32_t labelId_;
    uint32_t descriptionId_;
    uint32_t abilityLableId_;
    uint32_t abilityDescriptionId_;
    uint32_t abilityIconId_;
    int32_t launchedCount_;
    int64_t lastModuleUsedTime_;
    int32_t userId_;
    bool removed_;
    bool installFreeSupported_;
    bool isNewAdded_;
    std::vector<BundleActiveFormRecord> formRecords_; // key:formid, value:formhistory
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_MODULE_RECORD_H
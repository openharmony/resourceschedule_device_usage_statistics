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

#include "bundle_active_event.h"
#include "bundle_active_log.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveEvent::BundleActiveEvent()
{
    bundleName_ = "";
    continuousTaskAbilityName_ = "";
    abilityName_ = "";
    abilityId_ = "";
    moduleName_ = "";
    formName_ = "";
    formId_ = 0;
    formDimension_ = 0;
    timeStamp_ = 0;
    eventId_ = DEFAULT_EVENT_ID;
}

BundleActiveEvent::BundleActiveEvent (const BundleActiveEvent& orig)
{
    bundleName_ = orig.bundleName_;
    continuousTaskAbilityName_ = orig.continuousTaskAbilityName_;
    abilityName_ = orig.abilityName_;
    abilityId_ = orig.abilityId_;
    moduleName_ = orig.moduleName_;
    formName_ = orig.formName_;
    formId_ = orig.formId_;
    formDimension_ = orig.formDimension_;
    timeStamp_ = orig.timeStamp_;
    eventId_ = orig.eventId_;
}

BundleActiveEvent::BundleActiveEvent(int32_t eventId, int64_t timeStamp)
{
    bundleName_.clear();
    continuousTaskAbilityName_.clear();
    abilityName_.clear();
    abilityId_.clear();
    moduleName_.clear();
    formName_.clear();
    formDimension_ = 0;
    formId_ = 0;
    timeStamp_ = timeStamp;
    eventId_ = eventId;
}

BundleActiveEvent::BundleActiveEvent(const int32_t eventId, const std::string bundleName)
{
    bundleName_ = bundleName;
    continuousTaskAbilityName_.clear();
    abilityName_.clear();
    abilityId_.clear();
    moduleName_.clear();
    formName_.clear();
    formDimension_ = 0;
    formId_ = 0;
    timeStamp_ = 0;
    eventId_ = eventId;
}

BundleActiveEvent::BundleActiveEvent(const std::string bundleName, const std::string continuousTaskAbilityName)
{
    bundleName_ = bundleName;
    continuousTaskAbilityName_ = continuousTaskAbilityName;
    abilityName_.clear();
    abilityId_.clear();
    moduleName_.clear();
    formName_.clear();
    formDimension_ = 0;
    formId_ = 0;
    timeStamp_ = 0;
    eventId_ = DEFAULT_EVENT_ID;
}

BundleActiveEvent::BundleActiveEvent(const std::string bundleName, const std::string abilityName,
    const std::string abilityId, const std::string moduleName)
{
    bundleName_ = bundleName;
    continuousTaskAbilityName_.clear();
    abilityName_ = abilityName;
    abilityId_ = abilityId;
    moduleName_ = moduleName;
    formName_.clear();
    formDimension_ = 0;
    formId_ = 0;
    timeStamp_ = 0;
    eventId_ = DEFAULT_EVENT_ID;
}

BundleActiveEvent::BundleActiveEvent(const std::string bundleName, const std::string moduleName,
    const std::string formName, const int32_t formDimension,
    const int64_t formId, const int32_t eventId)
{
    bundleName_ = bundleName;
    continuousTaskAbilityName_.clear();
    abilityName_.clear();
    abilityId_.clear();
    moduleName_ = moduleName;
    formName_ = formName;
    formDimension_ = formDimension;
    formId_ = formId;
    timeStamp_ = 0;
    eventId_ = eventId;
}

BundleActiveEvent& BundleActiveEvent::operator=(const BundleActiveEvent& orig)
{
    bundleName_ = orig.bundleName_;
    continuousTaskAbilityName_ = orig.continuousTaskAbilityName_;
    abilityName_ = orig.abilityName_;
    abilityId_ = orig.abilityId_;
    moduleName_ = orig.moduleName_;
    formName_ = orig.formName_;
    formDimension_ = orig.formDimension_;
    formId_ = orig.formId_;
    timeStamp_ = orig.timeStamp_;
    eventId_ = orig.eventId_;
    return *this;
}

void BundleActiveEvent::PrintEvent(const bool debug) const
{
    if (!debug) {
        return;
    }
    BUNDLE_ACTIVE_LOGI("bundle name is %{public}s, ability name is %{public}s, continue task ability is %{public}s, "
        "module name is %{public}s, "
        "form name is %{public}s, form dimension is %{public}d, form id is %{public}lld, event id is %{public}d",
        bundleName_.c_str(), abilityName_.c_str(), continuousTaskAbilityName_.c_str(), moduleName_.c_str(),
        formName_.c_str(), formDimension_, (long long)formId_, eventId_);
}

bool BundleActiveEvent::Marshalling(Parcel &parcel) const
{
    if (parcel.WriteString(bundleName_) &&
        parcel.WriteString(continuousTaskAbilityName_) &&
        parcel.WriteString(abilityName_) &&
        parcel.WriteString(abilityId_) &&
        parcel.WriteString(moduleName_) &&
        parcel.WriteString(formName_) &&
        parcel.WriteInt32(formDimension_) &&
        parcel.WriteInt64(formId_) &&
        parcel.WriteInt64(timeStamp_) &&
        parcel.WriteInt32(eventId_)) {
        return true;
    }
    return false;
}

std::shared_ptr<BundleActiveEvent> BundleActiveEvent::UnMarshalling(Parcel &parcel)
{
    std::shared_ptr<BundleActiveEvent> result = std::make_shared<BundleActiveEvent>();
    result->bundleName_ = parcel.ReadString();
    result->continuousTaskAbilityName_ = parcel.ReadString();
    result->abilityName_ = parcel.ReadString();
    result->abilityId_ = parcel.ReadString();
    result->moduleName_ = parcel.ReadString();
    result->formName_ = parcel.ReadString();
    result->formDimension_ = parcel.ReadInt32();
    result->formId_ = parcel.ReadInt64();
    result->timeStamp_ = parcel.ReadInt64();
    result->eventId_ = parcel.ReadInt32();
    return result;
}

std::string BundleActiveEvent::ToString()
{
    return "bundle name is " + this->bundleName_ + ", event is " +
        std::to_string(this->eventId_) + ", timestamp is " + std::to_string(this->timeStamp_) + "\n";
}

bool BundleActiveEvent::IsBundleEvent(const int32_t eventId)
{
    if (eventId == ABILITY_BACKGROUND ||
        eventId == ABILITY_FOREGROUND ||
        eventId == ABILITY_STOP ||
        eventId == LONG_TIME_TASK_STARTTED ||
        eventId == LONG_TIME_TASK_ENDED ||
        eventId == END_OF_THE_DAY) {
            return true;
    }
    return false;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


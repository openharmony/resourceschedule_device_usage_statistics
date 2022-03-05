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

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveEvent::BundleActiveEvent()
{
    bundleName_ = "";
    continuousTaskAbilityName_ = "";
    abilityName_ = "";
    abilityId_ = "";
    timeStamp_ = 0;
    eventId_ = 0;
}

BundleActiveEvent::BundleActiveEvent (const BundleActiveEvent& orig)
{
    bundleName_ = orig.bundleName_;
    abilityName_ = orig.abilityName_;
    abilityId_ = orig.abilityId_;
    timeStamp_ = orig.timeStamp_;
    eventId_ = orig.eventId_;
}

BundleActiveEvent::BundleActiveEvent(int eventId, int64_t timeStamp)
{
    bundleName_.clear();
    abilityName_.clear();
    abilityId_ = "";
    eventId_ = eventId;
    timeStamp_ = timeStamp;
}

std::string BundleActiveEvent::GetBundleName()
{
    return bundleName_;
}

std::string BundleActiveEvent::GetAbilityName()
{
    return abilityName_;
}

std::string BundleActiveEvent::GetAbilityId()
{
    return abilityId_;
}

int64_t BundleActiveEvent::GetTimeStamp()
{
    return timeStamp_;
}

int BundleActiveEvent::GetEventId()
{
    return eventId_;
}

bool BundleActiveEvent::Marshalling(Parcel &parcel) const
{
    if (parcel.WriteString(bundleName_) &&
        parcel.WriteInt32(eventId_) &&
        parcel.WriteInt64(timeStamp_)) {
        return true;
    }
    return false;
}

std::shared_ptr<BundleActiveEvent> BundleActiveEvent::Unmarshalling(Parcel &parcel)
{
    std::shared_ptr<BundleActiveEvent> result = std::make_shared<BundleActiveEvent>();
    result->bundleName_ = parcel.ReadString();
    result->eventId_ = parcel.ReadInt32();
    result->timeStamp_ = parcel.ReadInt64();
    return result;
}
}
}

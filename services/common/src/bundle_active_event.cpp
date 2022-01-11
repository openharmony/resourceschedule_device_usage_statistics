/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
namespace BundleActive {
BundleActiveEvent::BundleActiveEvent (const BundleActiveEvent& orig) {
    bundleName_ = orig.bundleName_;
    abilityName_ = orig.abilityName_;
    abilityId_ = orig.abilityId_;
    timeStamp_ = orig.timeStamp_;
    eventId_ = orig.eventId_;
    isIdle_ = orig.isIdle_;
}

BundleActiveEvent::BundleActiveEvent(const int eventId, const int64_t timeStamp) {
    eventId_ = eventId;
    timeStamp_ = timeStamp;
}

std::string BundleActiveEvent::GetBundleName() {
    return bundleName_;
}

std::string BundleActiveEvent::GetAbilityName() {
    return abilityName_;
}

int BundleActiveEvent::GetAbilityId() {
    return abilityId_;
}

int64_t BundleActiveEvent::GetTimeStamp() {
    return timeStamp_;
}

int BundleActiveEvent::GetEventId() {
    return eventId_;
}

bool BundleActiveEvent::GetIsIdle() {
    return isIdle_;
}
}
}

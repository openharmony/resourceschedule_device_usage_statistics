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
    m_bundleName = orig.m_bundleName;
    m_abilityName = orig.m_abilityName;
    m_abilityId = orig.m_abilityId;
    m_timeStamp = orig.m_timeStamp;
    m_eventId = orig.m_eventId;
    m_isIdle = orig.m_isIdle;
}

BundleActiveEvent::BundleActiveEvent(int eventId, long timeStamp) {
    m_eventId = eventId;
    m_timeStamp = timeStamp;
}

std::string BundleActiveEvent::GetBundleName() {
    return m_bundleName;
}

std::string BundleActiveEvent::GetAbilityName() {
    return m_abilityName;
}

int BundleActiveEvent::GetAbilityId() {
    return m_abilityId;
}

long BundleActiveEvent::GetTimeStamp() {
    return m_timeStamp;
}

int BundleActiveEvent::GetEventId() {
    return m_eventId;
}

bool BundleActiveEvent::GetIsIdle() {
    return m_isIdle;
}

}
}

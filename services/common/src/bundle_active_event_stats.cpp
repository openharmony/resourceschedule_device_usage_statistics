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

#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {
BundleActiveEventStats::BundleActiveEventStats(const BundleActiveEventStats& orig) {
    m_eventId = orig.m_eventId;
    m_beginTimeStamp = orig.m_beginTimeStamp;
    m_endTimeStamp = orig.m_endTimeStamp;
    m_lastEventTime = orig.m_lastEventTime;
    m_totalTime = orig.m_totalTime;
    m_count = orig.m_count;
}

int BundleActiveEventStats::GetEventId() {
    return m_eventId;
}

int BundleActiveEventStats::GetFirstTimeStamp() {
    return m_beginTimeStamp;
}

int BundleActiveEventStats::GetLastTimeStamp() {
    return m_endTimeStamp;
}

int BundleActiveEventStats::GetLastEventTime() {
    return m_lastEventTime;
}

int BundleActiveEventStats::GetTotalTime() {
    return m_totalTime;
}

int BundleActiveEventStats::GetCount() {
    return m_count;
}

void BundleActiveEventStats::add(const BundleActiveEventStats& right) {
    if (m_eventId != right.m_eventId) {
        return;
    }

    if (right.m_beginTimeStamp > m_beginTimeStamp) {
        m_lastEventTime = std::max(m_lastEventTime, right.m_lastEventTime);
    }
    m_beginTimeStamp = std::min(m_beginTimeStamp, right.m_beginTimeStamp);
    m_endTimeStamp = std::max(m_endTimeStamp, right.m_endTimeStamp);
    m_totalTime += right.m_totalTime;
    m_count += right.m_count;
}
}
}
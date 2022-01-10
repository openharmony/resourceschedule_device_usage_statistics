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

#include "bundle_active_event_tracker.h"

namespace OHOS {
namespace BundleActive {
void BundleActiveEventTracker::CommitTime(long timeStamp) {
    if (m_curStartTime != 0) {
        m_duration += timeStamp - m_curStartTime;
        m_curStartTime = 0;
    }
}

void BundleActiveEventTracker::Update(long timeStamp) {
    if (m_curStartTime == 0) {
        m_count++;
    }
    CommitTime(timeStamp);
    m_curStartTime = timeStamp;
    m_lastEventTime = timeStamp;
}

void BundleActiveEventTracker::AddToEventStats(std::vector<BundleActiveEventStats>& eventStatsList, int eventId, long beginTime, long endTime) {
    if (m_count != 0 || m_duration != 0) {
        BundleActiveEventStats newEvent;
        newEvent.m_eventId = eventId;
        newEvent.m_count = m_count;
        newEvent.m_totalTime = m_duration;
        newEvent.m_lastEventTime = m_lastEventTime;
        newEvent.m_beginTimeStamp = beginTime;
        newEvent.m_endTimeStamp = endTime;
        eventStatsList.emplace_back(newEvent);
    }
}
}
}
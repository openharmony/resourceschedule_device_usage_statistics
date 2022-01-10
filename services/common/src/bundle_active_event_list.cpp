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

#include "bundle_active_event_list.h"

namespace OHOS {
namespace BundleActive {
BundleActiveEventList::BundleActiveEventList() {

}

int BundleActiveEventList::Size() {
    return m_events.size();
}

void BundleActiveEventList::Clear() {
    m_events.clear();
}

void BundleActiveEventList::Insert(BundleActiveEvent event) {
    int size = m_events.size();
    if (size == 0 || event.m_timeStamp >= m_events.back().m_timeStamp) {
        m_events.push_back(event);
        return;
    }
    int insertIdx = FirstIndexOnOrAfter(event.m_timeStamp);
    m_events.insert(m_events.begin() + insertIdx, event);
}

int BundleActiveEventList::FirstIndexOnOrAfter(long timeStamp) {
    int size = m_events.size();
    int result = size;
    int lo = 0;
    int hi = size - 1;
    while (lo <= hi) {
        int mid = (hi - lo) / 2 + lo;
        long midTimeStamp = m_events[mid].m_timeStamp;
        if (midTimeStamp >= timeStamp) {
            hi = mid - 1;
            result = mid;
        } else {
            lo = mid + 1;
        }
    }
    return result;
}

void BundleActiveEventList::Merge(const BundleActiveEventList& right) {
    int size = right.m_events.size();
    for (int i = 0; i < size; i++) {
        Insert(right.m_events[i]);
    }
}
}
}
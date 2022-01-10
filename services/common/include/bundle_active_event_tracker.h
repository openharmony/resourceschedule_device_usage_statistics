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

#ifndef BUNDLE_ACTIVE_EVENT_TRACKER_H
#define BUNDLE_ACTIVE_EVENT_TRACKER_H

#include "bundle_active_iservice.h"
#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveEventTracker {
public:
    long m_curStartTime;
    long m_lastEventTime;
    long m_duration;
    long m_count;
    void CommitTime(long timeStamp);
    void Update(long timeStamp);
    void AddToEventStats(std::vector<BundleActiveEventStats>& eventStatsList, int eventId, long beginTime, long endTime);
    BundleActiveEventTracker() {};
};

}
}
#endif
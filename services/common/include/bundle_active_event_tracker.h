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

#include "ibundle_active_service.h"
#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {
class BundleActiveEventTracker {
public:
    int64_t curStartTime_;
    int64_t lastEventTime_;
    int64_t duration_;
    int64_t count_;
    void CommitTime(const int64_t timeStamp);
    void Update(int64_t timeStamp);
    void AddToEventStats(std::vector<BundleActiveEventStats>& eventStatsList, const int eventId,
        const int64_t beginTime, const int64_t endTime);
    BundleActiveEventTracker() {};
};
}
}
#endif
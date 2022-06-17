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

#include "bundle_active_event_tracker.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveEventTracker::BundleActiveEventTracker()
{
    curStartTime_ = 0;
    lastEventTime_ = 0;
    duration_ = 0;
    count_ =0;
}

void BundleActiveEventTracker::CommitTime(const int64_t timeStamp)
{
    if (curStartTime_) {
        duration_ += timeStamp - curStartTime_;
        curStartTime_ = 0;
    }
}

void BundleActiveEventTracker::Update(int64_t timeStamp)
{
    if (!curStartTime_) {
        count_++;
    }
    CommitTime(timeStamp);
    curStartTime_ = timeStamp;
    lastEventTime_ = timeStamp;
}

void BundleActiveEventTracker::AddToEventStats(std::vector<BundleActiveEventStats>& eventStatsList, int32_t eventId,
    int64_t beginTime, int64_t endTime)
{
    if (count_ || duration_) {
        BundleActiveEventStats newEvent;
        newEvent.eventId_ = eventId;
        newEvent.count_ = count_;
        newEvent.totalTime_ = duration_;
        newEvent.lastEventTime_ = lastEventTime_;
        newEvent.beginTimeStamp_ = beginTime;
        newEvent.endTimeStamp_ = endTime;
        eventStatsList.emplace_back(newEvent);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


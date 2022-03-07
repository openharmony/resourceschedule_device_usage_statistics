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

#include "bundle_active_event_stats.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveEventStats::BundleActiveEventStats()
{
    beginTimeStamp_ = 0;
    endTimeStamp_ = 0;
    lastEventTime_ = 0;
    totalTime_ = 0;
    count_ = 0;
}

BundleActiveEventStats::BundleActiveEventStats(const BundleActiveEventStats& orig)
{
    eventId_ = orig.eventId_;
    beginTimeStamp_ = orig.beginTimeStamp_;
    endTimeStamp_ = orig.endTimeStamp_;
    lastEventTime_ = orig.lastEventTime_;
    totalTime_ = orig.totalTime_;
    count_ = orig.count_;
}

int BundleActiveEventStats::GetEventId()
{
    return eventId_;
}

int BundleActiveEventStats::GetFirstTimeStamp()
{
    return beginTimeStamp_;
}

int BundleActiveEventStats::GetLastTimeStamp()
{
    return endTimeStamp_;
}

int BundleActiveEventStats::GetLastEventTime()
{
    return lastEventTime_;
}

int BundleActiveEventStats::GetTotalTime()
{
    return totalTime_;
}

int BundleActiveEventStats::GetCount()
{
    return count_;
}

void BundleActiveEventStats::add(const BundleActiveEventStats& right)
{
    if (eventId_ != right.eventId_) {
        return;
    }

    if (right.beginTimeStamp_ > beginTimeStamp_) {
        lastEventTime_ = std::max(lastEventTime_, right.lastEventTime_);
    }
    beginTimeStamp_ = std::min(beginTimeStamp_, right.beginTimeStamp_);
    endTimeStamp_ = std::max(endTimeStamp_, right.endTimeStamp_);
    totalTime_ += right.totalTime_;
    count_ += right.count_;
}
}
}
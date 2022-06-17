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
    eventId_ = 0;
    beginTimeStamp_ = 0;
    endTimeStamp_ = 0;
    lastEventTime_ = 0;
    totalTime_ = 0;
    count_ = 0;
    name_.clear();
}

BundleActiveEventStats::BundleActiveEventStats(const BundleActiveEventStats& orig)
{
    eventId_ = orig.eventId_;
    beginTimeStamp_ = orig.beginTimeStamp_;
    endTimeStamp_ = orig.endTimeStamp_;
    lastEventTime_ = orig.lastEventTime_;
    totalTime_ = orig.totalTime_;
    count_ = orig.count_;
    name_ = orig.name_;
}

int32_t BundleActiveEventStats::GetEventId()
{
    return eventId_;
}

int64_t BundleActiveEventStats::GetFirstTimeStamp()
{
    return beginTimeStamp_;
}

int64_t BundleActiveEventStats::GetLastTimeStamp()
{
    return endTimeStamp_;
}

int64_t BundleActiveEventStats::GetLastEventTime()
{
    return lastEventTime_;
}

int64_t BundleActiveEventStats::GetTotalTime()
{
    return totalTime_;
}

int32_t BundleActiveEventStats::GetCount()
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

bool BundleActiveEventStats::Marshalling(Parcel &parcel) const
{
    if (parcel.WriteInt32(eventId_) &&
        parcel.WriteInt64(beginTimeStamp_) &&
        parcel.WriteInt64(endTimeStamp_) &&
        parcel.WriteInt64(lastEventTime_) &&
        parcel.WriteInt64(totalTime_) &&
        parcel.WriteInt32(count_) &&
        parcel.WriteString(name_)) {
        return true;
    }
    return false;
}

std::shared_ptr<BundleActiveEventStats> BundleActiveEventStats::UnMarshalling(Parcel &parcel)
{
    std::shared_ptr<BundleActiveEventStats> result = std::make_shared<BundleActiveEventStats>();
    result->eventId_ = parcel.ReadInt32();
    result->beginTimeStamp_ = parcel.ReadInt64();
    result->endTimeStamp_ = parcel.ReadInt64();
    result->lastEventTime_ = parcel.ReadInt64();
    result->totalTime_ = parcel.ReadInt64();
    result->count_ = parcel.ReadInt32();
    result->name_ = parcel.ReadString();
    return result;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


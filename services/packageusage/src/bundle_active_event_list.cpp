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

#include "bundle_active_event_list.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveEventList::BundleActiveEventList()
{
}

int32_t BundleActiveEventList::Size()
{
    return events_.size();
}

void BundleActiveEventList::Clear()
{
    events_.clear();
}

void BundleActiveEventList::Insert(BundleActiveEvent event)
{
    uint32_t size = events_.size();
    if (size == 0 || event.timeStamp_ >= events_.back().timeStamp_) {
        events_.push_back(event);
        return;
    }
    int32_t insertIdx = FindBestIndex(event.timeStamp_);
    events_.insert(events_.begin() + insertIdx, event);
}

int32_t BundleActiveEventList::FindBestIndex(const int64_t timeStamp)
{
    int32_t size = static_cast<int32_t>(events_.size());
    int32_t result = size;
    int32_t lo = 0;
    int32_t hi = size - 1;
    while (lo <= hi) {
        int32_t mid = (hi - lo) / 2 + lo;
        int64_t midTimeStamp = events_[mid].timeStamp_;
        if (midTimeStamp >= timeStamp) {
            hi = mid - 1;
            result = mid;
        } else {
            lo = mid + 1;
        }
    }
    return result;
}

void BundleActiveEventList::Merge(const BundleActiveEventList& right)
{
    int32_t size = static_cast<int32_t>(right.events_.size());
    for (int32_t i = 0; i < size; i++) {
        Insert(right.events_[i]);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


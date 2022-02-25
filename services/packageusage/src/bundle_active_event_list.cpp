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

int BundleActiveEventList::Size()
{
    return events_.size();
}

void BundleActiveEventList::Clear()
{
    events_.clear();
}

void BundleActiveEventList::Insert(BundleActiveEvent event)
{
    int size = events_.size();
    if (size == 0 || event.timeStamp_ >= events_.back().timeStamp_) {
        events_.push_back(event);
        return;
    }
    int insertIdx = FindBestIndex(event.timeStamp_);
    events_.insert(events_.begin() + insertIdx, event);
}

int BundleActiveEventList::FindBestIndex(const int64_t& timeStamp)
{
    int size = events_.size();
    int result = size;
    int lo = 0;
    int hi = size - 1;
    while (lo <= hi) {
        int mid = (hi - lo) / 2 + lo;
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
    int size = right.events_.size();
    for (int i = 0; i < size; i++) {
        Insert(right.events_[i]);
    }
}
}
}
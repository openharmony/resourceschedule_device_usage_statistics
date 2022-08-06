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

#ifndef BUNDLE_ACTIVE_EVENT_LIST_H
#define BUNDLE_ACTIVE_EVENT_LIST_H

#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveEventList {
public:
    /*
    * function: BundleActiveEventList, default constructor.
    */
    BundleActiveEventList();
    /*
    * function: ~BundleActiveEventList, default destructor.
    */
    ~BundleActiveEventList() {}
    /*
    * function: Size, get size of member events_.
    * return: size of member events_
    */
    int32_t Size();
    /*
    * function: Clear, clear all event in member events_.
    */
    void Clear();
    /*
    * function: Insert, insert a event to member events_.
    * parameters: event
    */
    void Insert(BundleActiveEvent event);
    /*
    * function: FindBestIndex, find best index to insert event.
    * parameters: timeStamp
    * return: the index of inserting.
    */
    int32_t FindBestIndex(const int64_t timeStamp);
    /*
    * function: Merge, merge two BundleActiveEventList.
    * parameters: right
    */
    void Merge(const BundleActiveEventList& right);
    std::vector<BundleActiveEvent> events_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_EVENT_LIST_H


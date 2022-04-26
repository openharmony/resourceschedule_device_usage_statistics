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

#ifndef BUNDLE_ACTIVE_EVENT_STATS_H
#define BUNDLE_ACTIVE_EVENT_STATS_H

#include "ibundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveEventStats {
public:
    int32_t eventId_;
    int64_t beginTimeStamp_;
    int64_t endTimeStamp_;
    int64_t lastEventTime_;
    int64_t totalTime_;
    int32_t count_;
    /*
    * function: BundleActiveEventStats, default constructor.
    */
    BundleActiveEventStats();
    /*
    * function: BundleActiveEventStats, copy constructor.
    * parameters: orig
    */
    BundleActiveEventStats(const BundleActiveEventStats& orig);
    /*
    * function: GetEventId, get eventId.
    * return: member eventId_.
    */
    int32_t GetEventId();
    /*
    * function: GetFirstTimeStamp, get first time stamp.
    * return: member beginTimeStamp_.
    */
    int64_t GetFirstTimeStamp();
    /*
    * function: GetLastTimeStamp, get last time stamp.
    * return: member endTimeStamp_.
    */
    int64_t GetLastTimeStamp();
    /*
    * function: GetLastEventTime, get last event time.
    * return: member lastEventTime_.
    */
    int64_t GetLastEventTime();
    /*
    * function: GetTotalTime, get total time.
    * return: member totalTime_.
    */
    int64_t GetTotalTime();
    /*
    * function: GetCount, get count.
    * return: member count_.
    */
    int32_t GetCount();
    /*
    * function: add, add statistics from another BundleActvieEventStats object.
    * parameters: right
    */
    void add(const BundleActiveEventStats& right);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_EVENT_STATS_H


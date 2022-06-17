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

#ifndef BUNDLE_ACTIVE_PERIOD_STATS_H
#define BUNDLE_ACTIVE_PERIOD_STATS_H

#include <memory>

#include "ibundle_active_service.h"
#include "bundle_active_event.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event_list.h"
#include "bundle_active_event_tracker.h"


namespace OHOS {
namespace DeviceUsageStats {
class BundleActivePeriodStats {
public:
    static const int32_t PERIOD_DAILY = 0;
    static const int32_t PERIOD_WEEKLY = 1;
    static const int32_t PERIOD_MONTHLY = 2;
    static const int32_t PERIOD_YEARLY = 3;
    static const int32_t PERIOD_BEST = 4;
    static const int32_t PERIOD_COUNT = 4;
    int64_t beginTime_;
    int64_t endTime_;
    int64_t lastTimeSaved_;
    int32_t userId_;
    std::map<std::string, std::shared_ptr<BundleActivePackageStats>> bundleStats_;
    BundleActiveEventList events_;
    std::set<std::string> packetNamesCache_;
    BundleActiveEventTracker interactiveTracker_;
    BundleActiveEventTracker noninteractiveTracker_;
    BundleActiveEventTracker keyguardShownTracker_;
    BundleActiveEventTracker keyguardHiddenTracker_;
    /*
    * function: BundleActivePeriodStats,default constructor.
    */
    BundleActivePeriodStats();
    /*
    * function: GetOrCreateUsageStats, get or create bundle usage statistics object of a bundle.
    * parameters: bundleName
    * return: point to bundle usage statistics object.
    */
    std::shared_ptr<BundleActivePackageStats> GetOrCreateUsageStats(const std::string& bundleName);
    /*
    * function: Update, update usage statistics of specific bundle.
    * parameters: bundleName, longTimeTaskName, timeStamp, eventId, abilityId
    */
    void Update(const std::string bundleName, const std::string longTimeTaskName, const int64_t timeStamp,
        const int32_t eventId, const std::string abilityId);
    /*
    * function: AddEvent, add a event to member events_.
    * parameters: event
    */
    void AddEvent(BundleActiveEvent event);
    /*
    * function: UpdateScreenInteractive, update screen interactive time.
    * parameters: timeStamp
    */
    void UpdateScreenInteractive(const int64_t timeStamp);
    /*
    * function: UpdateScreenNonInteractive, update screen non interactive time.
    * parameters: timeStamp
    */
    void UpdateScreenNonInteractive(const int64_t timeStamp);
    /*
    * function: UpdateKeyguardShown, key guard shown time.
    * parameters: timeStamp
    */
    void UpdateKeyguardShown(const int64_t timeStamp);
    /*
    * function: UpdateKeyguardHidden, key guard hidden time.
    * parameters: timeStamp
    */
    void UpdateKeyguardHidden(const int64_t timeStamp);
    /*
    * function: CommitTime, key guard hidden time.
    * parameters: timeStamp
    */
    void CommitTime(const int64_t timeStamp);
    /*
    * function: AddEventStatsTo, add all time to eventStatsList.
    * parameters: eventStatsList
    */
    void AddEventStatsTo(std::vector<BundleActiveEventStats>& eventStatsList);
    /*
    * function: GetCachedString, store string to cache.
    * parameters: str
    * return: string
    */
    std::string GetCachedString(std::string str);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_PERIOD_STATS_H


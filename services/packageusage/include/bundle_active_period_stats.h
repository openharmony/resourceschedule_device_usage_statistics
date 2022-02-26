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
    static const int PERIOD_DAILY = 0;
    static const int PERIOD_WEEKLY = 1;
    static const int PERIOD_MONTHLY = 2;
    static const int PERIOD_YEARLY = 3;
    static const int PERIOD_BEST = 4;
    static const int PERIOD_COUNT = 4;
    int64_t beginTime_;
    int64_t endTime_;
    int64_t lastTimeSaved_;
    int userId_;
    std::map<std::string, std::shared_ptr<BundleActivePackageStats>> bundleStats_;
    BundleActiveEventList events_;
    std::set<std::string> packetNamesCache_;
    BundleActiveEventTracker interactiveTracker_;
    BundleActiveEventTracker noninteractiveTracker_;
    BundleActiveEventTracker keyguardShownTracker_;
    BundleActiveEventTracker keyguardHiddenTracker_;
    BundleActivePeriodStats();
    std::shared_ptr<BundleActivePackageStats> GetOrCreateUsageStats(const std::string& bundleName);
    BundleActiveEvent BuildEvent(std::string bundleName, std::string timeTaskName);
    void Update(const std::string bundleName, const std::string longTimeTaskName, const int64_t timeStamp,
        const int eventId, const std::string abilityId);
    void AddEvent(BundleActiveEvent event);
    void UpdateScreenInteractive(const int64_t timeStamp);
    void UpdateScreenNonInteractive(const int64_t timeStamp);
    void UpdateKeyguardShown(const int64_t timeStamp);
    void UpdateKeyguardHidden(const int64_t timeStamp);
    void CommitTime(const int64_t timeStamp);
    void AddEventStatsTo(std::vector<BundleActiveEventStats>& eventStatsList);
    std::string GetCachedString(std::string str);
};
}
}
#endif
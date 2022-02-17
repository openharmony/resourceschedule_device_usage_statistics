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

#ifndef bundle_active_period_stats_H
#define bundle_active_period_stats_H

#include "ibundle_active_service.h"
#include "bundle_active_event.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event_list.h"
#include "bundle_active_event_tracker.h"

namespace OHOS {
namespace BundleActive {
class BundleActivePeriodStats {
public:
    static const int PERIOD_DAILY = 0;
    static const int PERIOD_WEEKLY = 1;
    static const int PERIOD_MONTHLY = 2;
    static const int PERIOD_YEARLY = 3;
    static const int PERIOD_BEST = 4;
    static const int PERIOD_COUNT = 4;
    static const int64_t DAY_IN_MILLIS = (int64_t)24 * 60 * 60 * 1000;
    static const int64_t WEEK_IN_MILLIS = (int64_t)7 * 24 * 60 * 60 * 1000;
    static const int64_t MONTH_IN_MILLIS = (int64_t)30 * 24 * 60 * 60 * 1000;
    static const int64_t YEAR_IN_MILLIS = (int64_t)365 * 24 * 60 * 60 * 1000;
    int64_t beginTime_;
    int64_t endTime_;
    int64_t lastTimeSaved_;
    std::map<std::string, BundleActivePackageStats> bundleStats_;
    BundleActiveEventList events_;
    std::set<std::string> packetNamesCache_;
    BundleActiveEventTracker interactiveTracker_;
    BundleActiveEventTracker noninteractiveTracker_;
    BundleActiveEventTracker keyguardShownTracker_;
    BundleActiveEventTracker keyguardHiddenTracker_;
    BundleActivePackageStats& GetOrCreateUsageStats(std::string bundleName);
    BundleActiveEvent BuildEvent(std::string bundleName, std::string longTimeTaskName);
    void Update(std::string bundleName, std::string longTimeTaskName, const int64_t timeStamp, const int eventId,
        const int abilityId);
    void AddEvent(BundleActiveEvent event);
    void UpdateScreenInteractive(const int64_t timeStamp);
    void UpdateScreenNonInteractive(const int64_t timeStamp);
    void UpdateKeyguardShown(const int64_t timeStamp);
    void UpdateKeyguardHidden(const int64_t timeStamp);

private:
    void CommitTime(const int64_t timeStamp);
    void AddEventStatsTo(std::vector<BundleActiveEventStats>& eventStatsList);
    std::string GetCachedString(std::string str);
};
}
}
#endif
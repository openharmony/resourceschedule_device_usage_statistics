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

#ifndef BUNDLE_ACTIVE_INTERVAL_STATS_H
#define BUNDLE_ACTIVE_INTERVAL_STATS_H

#include "bundle_active_iservice.h"
#include "bundle_active_event.h"
#include "bundle_active_usage_stats.h"
#include "bundle_active_event_list.h"
#include "bundle_active_event_tracker.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveIntervalStats {
public:
    static const int CURRENT_MAJOR_VERSION = 1;
    static const int CURRENT_MINOR_VERSION = 1;
    static const int INTERVAL_DAILY = 0;
    static const int INTERVAL_WEEKLY = 1;
    static const int INTERVAL_MONTHLY = 2;
    static const int INTERVAL_YEARLY = 3;
    static const int INTERVAL_BEST = 4;
    static const int INTERVAL_COUNT = 4;
    static const long long DAY_IN_MILLIS = (long long)24 * 60 * 60 * 1000;
    static const long long WEEK_IN_MILLIS = (long long)7 * 24 * 60 * 60 * 1000;
    static const long long MONTH_IN_MILLIS = (long long)30 * 24 * 60 * 60 * 1000;
    static const long long YEAR_IN_MILLIS = (long long)365 * 24 * 60 * 60 * 1000;
    int majorVersion = CURRENT_MAJOR_VERSION;
    int minorVersion = CURRENT_MINOR_VERSION;
    long m_beginTime;
    long m_endTime;
    long m_lastTimeSaved;
    std::map<std::string, BundleActiveUsageStats> m_bundleStats;
    BundleActiveEventList m_events;
    std::set<std::string> m_packetNamesCache;
    BundleActiveEventTracker m_interactiveTracker;
    BundleActiveEventTracker m_noninteractiveTracker;
    BundleActiveEventTracker m_keyguardShownTracker;
    BundleActiveEventTracker m_keyguardHiddenTracker;

    BundleActiveUsageStats& GetOrCreateUsageStats(std::string bundleName);
    BundleActiveEvent BuildEvent(std::string bundleName, std::string serviceName);
    void Update(std::string bundleName, std::string serviceName, long timeStamp, int eventId, int abilityId);
    void AddEvent(BundleActiveEvent event);
    void UpdateScreenInteractive(long timeStamp);
    void UpdateScreenNonInteractive(long timeStamp);
    void UpdateKeyguardShown(long timeStamp);
    void UpdateKeyguardHidden(long timeStamp);

private:
    void CommitTime(long timeStamp);
    void AddEventStatsTo(std::vector<BundleActiveEventStats>& eventStatsList);
    std::string GetCachedString(std::string str);

};

}
}
#endif
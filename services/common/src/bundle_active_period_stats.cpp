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

#include "bundle_active_period_stats.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {
BundleActivePackageStats& BundleActivePeriodStats::GetOrCreateUsageStats(std::string bundleName)
{
    std::map<std::string, BundleActivePackageStats>::iterator it;
    it = bundleStats_.find(bundleName);
    if (it == bundleStats_.end()) {
        BundleActivePackageStats newStats;
        newStats.beginTimeStamp_ = beginTime_;
        newStats.endTimeStamp_ = endTime_;
        newStats.bundleName_ = GetCachedString(bundleName);
        bundleStats_[newStats.bundleName_] = newStats;
    }
    return bundleStats_[bundleName];
}

void BundleActivePeriodStats::Update(std::string bundleName, std::string longTimeTaskName, const int64_t timeStamp, const int eventId,  const int abilityId)
{
    if (eventId == BundleActiveEvent::DEVICE_SHUTDOWN || eventId == BundleActiveEvent::FLUSH_TO_DISK) {
        for (auto usageStatsPair : bundleStats_) {
            usageStatsPair.second.Update("", timeStamp, eventId, abilityId);
        }
    } else {
        BundleActivePackageStats& usageStats = GetOrCreateUsageStats(bundleName);
        usageStats.Update("", timeStamp, eventId, abilityId);
    }
    if (timeStamp > endTime_) {
        endTime_ = timeStamp;
    }
}

void BundleActivePeriodStats::AddEvent(BundleActiveEvent event)
{
    event.bundleName_ = GetCachedString(event.bundleName_);
    if (!event.longTimeTaskName_.empty()) {
        event.longTimeTaskName_ = GetCachedString(event.longTimeTaskName_);
    }
    events_.Insert(event);
    if (event.timeStamp_ > endTime_) {
        endTime_ = event.timeStamp_;
    }
}

void BundleActivePeriodStats::CommitTime(const int64_t timeStamp)
{
    interactiveTracker_.CommitTime(timeStamp);
    noninteractiveTracker_.CommitTime(timeStamp);
    keyguardShownTracker_.CommitTime(timeStamp);
    keyguardHiddenTracker_.CommitTime(timeStamp);
}

void BundleActivePeriodStats::UpdateScreenInteractive(const int64_t timeStamp)
{
    interactiveTracker_.Update(timeStamp);
    noninteractiveTracker_.CommitTime(timeStamp);
}

void BundleActivePeriodStats::UpdateScreenNonInteractive(const int64_t timeStamp)
{
    noninteractiveTracker_.Update(timeStamp);
    interactiveTracker_.CommitTime(timeStamp);
}

void BundleActivePeriodStats::UpdateKeyguardShown(const int64_t timeStamp)
{
    keyguardShownTracker_.Update(timeStamp);
    keyguardHiddenTracker_.CommitTime(timeStamp);
}

void BundleActivePeriodStats::UpdateKeyguardHidden(const int64_t timeStamp)
{
    keyguardHiddenTracker_.Update(timeStamp);
    keyguardShownTracker_.CommitTime(timeStamp);
}

void BundleActivePeriodStats::AddEventStatsTo(std::vector<BundleActiveEventStats>& eventStatsList) {
    interactiveTracker_.AddToEventStats(eventStatsList, BundleActiveEvent::SCREEN_INTERACTIVE, beginTime_, endTime_);
    noninteractiveTracker_.AddToEventStats(eventStatsList, BundleActiveEvent::SCREEN_NON_INTERACTIVE, beginTime_, endTime_);
    keyguardShownTracker_.AddToEventStats(eventStatsList, BundleActiveEvent::KEYGUARD_SHOWN, beginTime_, endTime_);
    keyguardHiddenTracker_.AddToEventStats(eventStatsList, BundleActiveEvent::KEYGUARD_HIDDEN, beginTime_, endTime_);
}

std::string BundleActivePeriodStats::GetCachedString(std::string str)
{
    std::set<std::string>::iterator it;
    it = packetNamesCache_.find(str);
    if (it == packetNamesCache_.end()) {
        packetNamesCache_.insert(str);
        return str;
    }
    return *it;
}
}
}

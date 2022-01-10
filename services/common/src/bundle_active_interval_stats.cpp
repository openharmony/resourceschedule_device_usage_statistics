#include "bundle_active_interval_stats.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {

BundleActiveUsageStats& BundleActiveIntervalStats::GetOrCreateUsageStats(std::string bundleName) {
    std::map<std::string, BundleActiveUsageStats>::iterator it;
    it = m_bundleStats.find(bundleName);
    if (it == m_bundleStats.end()) {
        BundleActiveUsageStats newStats;
        newStats.m_beginTimeStamp = m_beginTime;
        newStats.m_endTimeStamp = m_endTime;
        newStats.m_bundleName = GetCachedString(bundleName);
        m_bundleStats[newStats.m_bundleName] = newStats;
    }
    return m_bundleStats[bundleName];
}
BundleActiveEvent BundleActiveIntervalStats::BuildEvent(std::string bundleName, std::string serviceName) {
    BundleActiveEvent newEvent;
    newEvent.m_bundleName = bundleName;
    if (!serviceName.empty()) {
        newEvent.m_serviceName = serviceName;
    }
    return newEvent;
}
void BundleActiveIntervalStats::Update(std::string bundleName, std::string serviceName, long timeStamp, int eventId, int abilityId) {
    if (eventId == BundleActiveEvent::DEVICE_SHUTDOWN || eventId == BundleActiveEvent::FLUSH_TO_DISK) {
        for (auto usageStatsPair : m_bundleStats) {
            usageStatsPair.second.Update("", timeStamp, eventId, abilityId);
        }
    } else {
        BundleActiveUsageStats& usageStats = GetOrCreateUsageStats(bundleName);
        usageStats.Update("", timeStamp, eventId, abilityId);
    }
    if (timeStamp > m_endTime) {
        m_endTime = timeStamp;
    }
}
void BundleActiveIntervalStats::AddEvent(BundleActiveEvent event) {
    event.m_bundleName = GetCachedString(event.m_bundleName);
    if (!event.m_serviceName.empty()) {
        event.m_serviceName = GetCachedString(event.m_serviceName);
    }
    m_events.Insert(event);
    if (event.m_timeStamp > m_endTime) {
        m_endTime = event.m_timeStamp;
    }
}
void BundleActiveIntervalStats::CommitTime(long timeStamp) {
    m_interactiveTracker.CommitTime(timeStamp);
    m_noninteractiveTracker.CommitTime(timeStamp);
    m_keyguardShownTracker.CommitTime(timeStamp);
    m_keyguardHiddenTracker.CommitTime(timeStamp);
}
void BundleActiveIntervalStats::UpdateScreenInteractive(long timeStamp) {
    m_interactiveTracker.Update(timeStamp);
    m_noninteractiveTracker.CommitTime(timeStamp);
}
void BundleActiveIntervalStats::UpdateScreenNonInteractive(long timeStamp) {
    m_noninteractiveTracker.Update(timeStamp);
    m_interactiveTracker.CommitTime(timeStamp);
}
void BundleActiveIntervalStats::UpdateKeyguardShown(long timeStamp) {
    m_keyguardShownTracker.Update(timeStamp);
    m_keyguardHiddenTracker.CommitTime(timeStamp);
}
void BundleActiveIntervalStats::UpdateKeyguardHidden(long timeStamp) {
    m_keyguardHiddenTracker.Update(timeStamp);
    m_keyguardShownTracker.CommitTime(timeStamp);
}
void BundleActiveIntervalStats::AddEventStatsTo(std::vector<BundleActiveEventStats>& eventStatsList) {
    m_interactiveTracker.AddToEventStats(eventStatsList, BundleActiveEvent::SCREEN_INTERACTIVE, m_beginTime, m_endTime);
    m_noninteractiveTracker.AddToEventStats(eventStatsList, BundleActiveEvent::SCREEN_NON_INTERACTIVE, m_beginTime, m_endTime);
    m_keyguardShownTracker.AddToEventStats(eventStatsList, BundleActiveEvent::KEYGUARD_SHOWN, m_beginTime, m_endTime);
    m_keyguardHiddenTracker.AddToEventStats(eventStatsList, BundleActiveEvent::KEYGUARD_HIDDEN, m_beginTime, m_endTime);
}
std::string BundleActiveIntervalStats::GetCachedString(std::string str) {
    std::set<std::string>::iterator it;
    it = m_packetNamesCache.find(str);
    if (it == m_packetNamesCache.end()) {
        m_packetNamesCache.insert(str);
        return str;
    }
    return *it;
}

}
}
#include "bundle_active_usage_stats.h"

namespace OHOS {
namespace BundleActive {
BundleActiveUsageStats::BundleActiveUsageStats (const BundleActiveUsageStats& orig) {
    m_bundleName = orig.m_bundleName;
    m_beginTimeStamp = orig.m_beginTimeStamp;
    m_endTimeStamp = orig.m_endTimeStamp;
    m_lastTimeUsed = orig.m_lastTimeUsed;
    m_lastTimeFrontServiceUsed = orig.m_lastTimeFrontServiceUsed;
    m_totalTimeFrontServiceUsed = orig.m_totalTimeFrontServiceUsed;
    m_totalTimeInFront = orig.m_totalTimeInFront;
    m_launchedCount = orig.m_launchedCount;
    m_bundleLaunchedCount = orig.m_bundleLaunchedCount;
    m_abilities = orig.m_abilities;
    m_frontServices = orig.m_frontServices;
    m_lastEvent = orig.m_lastEvent;
}
std::string BundleActiveUsageStats::GetBundleName() {
    return m_bundleName;
}
long BundleActiveUsageStats::GetBeginTimeStamp() {
    return m_beginTimeStamp;
}
long BundleActiveUsageStats::GetEntTimeStamp() {
    return m_endTimeStamp;
}
long BundleActiveUsageStats::GetLastTimeUsed() {
    return m_lastTimeUsed;
}
long BundleActiveUsageStats::GetTotalTimeInFront() {
    return m_totalTimeInFront;
}
long BundleActiveUsageStats::GetLastTimeFrontServiceUsed() {
    return m_lastTimeFrontServiceUsed;
}
long BundleActiveUsageStats::GetTotalTimeFrontServiceUsed() {
    return m_totalTimeFrontServiceUsed;
}
int BundleActiveUsageStats::GetLaunchedCount() {
    return m_launchedCount;
}
int BundleActiveUsageStats::GetBundleLaunchedCount() {
    return m_bundleLaunchedCount;
}
bool BundleActiveUsageStats::HasFrontAbility() {
    for (auto ability : m_abilities) {
        if (ability.second == BundleActiveEvent::ABILITY_FOREGROUND) {
            return true;
        }
    }
    return false;
}
bool BundleActiveUsageStats::AnyFrontServiceStarted() {
    return !m_frontServices.empty();
}
void BundleActiveUsageStats::IncrementTimeUsed(long timeStamp) {
    if (timeStamp > m_lastTimeUsed) {
        m_totalTimeInFront += timeStamp - m_lastTimeUsed;
        m_lastTimeUsed = timeStamp;
    }
}
void BundleActiveUsageStats::IncrementServiceTimeUsed(long timeStamp) {
    if (timeStamp > m_lastTimeFrontServiceUsed) {
        m_totalTimeFrontServiceUsed += timeStamp - m_lastTimeFrontServiceUsed;
        m_lastTimeFrontServiceUsed = timeStamp;
    }
}
void BundleActiveUsageStats::IncrementBundleLaunchedCount() {
    m_bundleLaunchedCount += 1;
}
void BundleActiveUsageStats::UpdateActivity(long timeStamp, int eventId, int abilityId) {
    if (eventId != BundleActiveEvent::ABILITY_FOREGROUND && eventId != BundleActiveEvent::ABILITY_BACKGROUND && 
        eventId != BundleActiveEvent::ABILITY_STOP) {
            return;
        }
    std::map<int, int>::iterator it;
    it = m_abilities.find(abilityId);
    if (it != m_abilities.end()) {
        int lastEventId = it->second;
        //When we recieve a new event, first update the time stats according to the last event in map.
        switch (lastEventId) {
            case BundleActiveEvent::ABILITY_FOREGROUND:
                IncrementTimeUsed(timeStamp);
                break;
            default:
                break;
        }
    }

    switch (eventId) {
        case BundleActiveEvent::ABILITY_FOREGROUND:
            if (!HasFrontAbility()) {
                m_lastTimeUsed = timeStamp;
            }
            m_abilities[abilityId] = eventId;
            break;
        case BundleActiveEvent::ABILITY_BACKGROUND:
            m_abilities[abilityId] = eventId;
            break;
        case BundleActiveEvent::ABILITY_STOP:
            m_abilities.erase(abilityId);
        default:
            break;
    }
}
void BundleActiveUsageStats::UpdateFrontService(std::string frontServiceName, long timeStamp, int eventId) {
    if (eventId != BundleActiveEvent::FRONT_SERVICE_STARTTED && eventId != BundleActiveEvent::FRONT_SERVICE_STOPPED) {
        return;
    }
    
    //When we recieve a new event, first update the time stats according to the last service event in map.
    std::map<std::string, int>::iterator it;
    it = m_frontServices.find(frontServiceName);
    if (it != m_frontServices.end()) {
        int lastEventId = it->second;
        switch (lastEventId) {
            case BundleActiveEvent::FRONT_SERVICE_STARTTED:
                IncrementServiceTimeUsed(timeStamp);
            default:
                break;
        }
    }

    switch (eventId) {
        case BundleActiveEvent::FRONT_SERVICE_STARTTED:
            if(!AnyFrontServiceStarted()) {
                m_lastTimeFrontServiceUsed = timeStamp;
            }
            m_frontServices[frontServiceName] = eventId;
            break;
        case BundleActiveEvent::FRONT_SERVICE_STOPPED:
            m_frontServices.erase(frontServiceName);
        default:
            break;
    }
}
void BundleActiveUsageStats::Update(std::string frontServiceName, long timeStamp, int eventId, int abilityId) {
    switch (eventId) {
        case BundleActiveEvent::ABILITY_FOREGROUND:
        case BundleActiveEvent::ABILITY_BACKGROUND:
        case BundleActiveEvent::ABILITY_STOP:
            UpdateActivity(timeStamp, eventId, abilityId);
            break;
        case BundleActiveEvent::END_OF_THE_DAY:
            if (HasFrontAbility()) {
                IncrementTimeUsed(timeStamp);
            }
        case BundleActiveEvent::FRONT_SERVICE_STARTTED:
        case BundleActiveEvent::FRONT_SERVICE_STOPPED:
            UpdateFrontService(frontServiceName, timeStamp, eventId);
            break;
        case BundleActiveEvent::DEVICE_SHUTDOWN:
        case BundleActiveEvent::FLUSH_TO_DISK:
            if (HasFrontAbility()) {
                IncrementTimeUsed(timeStamp);
            }
            if (AnyFrontServiceStarted()) {
                IncrementServiceTimeUsed(timeStamp);
            }
            break;
        default:
            break;
    }
    m_endTimeStamp = timeStamp;
    if (eventId == BundleActiveEvent::ABILITY_FOREGROUND) {
        m_launchedCount += 1;
    }
}

}
}
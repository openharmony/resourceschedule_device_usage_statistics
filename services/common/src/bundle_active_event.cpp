#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {

BundleActiveEvent::BundleActiveEvent (const BundleActiveEvent& orig) {
    m_bundleName = orig.m_bundleName;
    m_abilityName = orig.m_abilityName;
    m_abilityId = orig.m_abilityId;
    m_timeStamp = orig.m_timeStamp;
    m_eventId = orig.m_eventId;
    m_isIdle = orig.m_isIdle;
}

BundleActiveEvent::BundleActiveEvent(int eventId, long timeStamp) {
    m_eventId = eventId;
    m_timeStamp = timeStamp;
}

std::string BundleActiveEvent::GetBundleName() {
    return m_bundleName;
}

std::string BundleActiveEvent::GetAbilityName() {
    return m_abilityName;
}

int BundleActiveEvent::GetAbilityId() {
    return m_abilityId;
}

long BundleActiveEvent::GetTimeStamp() {
    return m_timeStamp;
}

int BundleActiveEvent::GetEventId() {
    return m_eventId;
}

bool BundleActiveEvent::GetIsIdle() {
    return m_isIdle;
}

}
}

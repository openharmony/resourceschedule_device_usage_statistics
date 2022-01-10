#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {

BundleActiveEventStats::BundleActiveEventStats(const BundleActiveEventStats& orig) {
    m_eventId = orig.m_eventId;
    m_beginTimeStamp = orig.m_beginTimeStamp;
    m_endTimeStamp = orig.m_endTimeStamp;
    m_lastEventTime = orig.m_lastEventTime;
    m_totalTime = orig.m_totalTime;
    m_count = orig.m_count;
}

int BundleActiveEventStats::GetEventId() {
    return m_eventId;
}
int BundleActiveEventStats::GetFirstTimeStamp() {
    return m_beginTimeStamp;
}
int BundleActiveEventStats::GetLastTimeStamp() {
    return m_endTimeStamp;
}
int BundleActiveEventStats::GetLastEventTime() {
    return m_lastEventTime;
}
int BundleActiveEventStats::GetTotalTime() {
    return m_totalTime;
}
int BundleActiveEventStats::GetCount() {
    return m_count;
}
void BundleActiveEventStats::add(const BundleActiveEventStats& right) {
    if (m_eventId != right.m_eventId) {
        return;
    }

    if (right.m_beginTimeStamp > m_beginTimeStamp) {
        m_lastEventTime = std::max(m_lastEventTime, right.m_lastEventTime);
    }
    m_beginTimeStamp = std::min(m_beginTimeStamp, right.m_beginTimeStamp);
    m_endTimeStamp = std::max(m_endTimeStamp, right.m_endTimeStamp);
    m_totalTime += right.m_totalTime;
    m_count += right.m_count;
}

}
}
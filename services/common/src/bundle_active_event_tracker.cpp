#include "bundle_active_event_tracker.h"

namespace OHOS {
namespace BundleActive {
void BundleActiveEventTracker::CommitTime(long timeStamp) {
    if (m_curStartTime != 0) {
        m_duration += timeStamp - m_curStartTime;
        m_curStartTime = 0;
    }
}
void BundleActiveEventTracker::Update(long timeStamp) {
    if (m_curStartTime == 0) {
        m_count++;
    }
    CommitTime(timeStamp);
    m_curStartTime = timeStamp;
    m_lastEventTime = timeStamp;
}
void BundleActiveEventTracker::AddToEventStats(std::vector<BundleActiveEventStats>& eventStatsList, int eventId, long beginTime, long endTime) {
    if (m_count != 0 || m_duration != 0) {
        BundleActiveEventStats newEvent;
        newEvent.m_eventId = eventId;
        newEvent.m_count = m_count;
        newEvent.m_totalTime = m_duration;
        newEvent.m_lastEventTime = m_lastEventTime;
        newEvent.m_beginTimeStamp = beginTime;
        newEvent.m_endTimeStamp = endTime;
        eventStatsList.emplace_back(newEvent);
    }
}

}
}
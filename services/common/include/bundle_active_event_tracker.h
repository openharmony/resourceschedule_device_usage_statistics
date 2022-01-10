#ifndef BUNDLE_ACTIVE_EVENT_TRACKER_H
#define BUNDLE_ACTIVE_EVENT_TRACKER_H

#include "bundle_active_iservice.h"
#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveEventTracker {
public:
    long m_curStartTime;
    long m_lastEventTime;
    long m_duration;
    long m_count;
    void CommitTime(long timeStamp);
    void Update(long timeStamp);
    void AddToEventStats(std::vector<BundleActiveEventStats>& eventStatsList, int eventId, long beginTime, long endTime);
    BundleActiveEventTracker() {};
};

}
}
#endif
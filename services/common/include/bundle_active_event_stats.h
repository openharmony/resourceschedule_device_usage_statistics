#ifndef BUNDLE_ACTIVE_EVENT_STATS_H
#define BUNDLE_ACTIVE_EVENT_STATS_H

#include "bundle_active_iservice.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveEventStats {
public:
    int m_eventId;
    long m_beginTimeStamp;
    long m_endTimeStamp;
    long m_lastEventTime;
    long m_totalTime;
    int m_count;
    BundleActiveEventStats() {};
    BundleActiveEventStats(const BundleActiveEventStats& orig);
    int GetEventId();
    int GetFirstTimeStamp();
    int GetLastTimeStamp();
    int GetLastEventTime();
    int GetTotalTime();
    int GetCount();
    void add(const BundleActiveEventStats& right);
};

}
}
#endif
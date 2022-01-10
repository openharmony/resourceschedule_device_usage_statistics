#include "bundle_active_event_list.h"

namespace OHOS {
namespace BundleActive {
BundleActiveEventList::BundleActiveEventList() {

}
int BundleActiveEventList::Size() {
    return m_events.size();
}
void BundleActiveEventList::Clear() {
    m_events.clear();
}
void BundleActiveEventList::Insert(BundleActiveEvent event) {
    int size = m_events.size();
    if (size == 0 || event.m_timeStamp >= m_events.back().m_timeStamp) {
        m_events.push_back(event);
        return;
    }
    int insertIdx = FirstIndexOnOrAfter(event.m_timeStamp);
    m_events.insert(m_events.begin() + insertIdx, event);
}
int BundleActiveEventList::FirstIndexOnOrAfter(long timeStamp) {
    int size = m_events.size();
    int result = size;
    int lo = 0;
    int hi = size - 1;
    while (lo <= hi) {
        int mid = (hi - lo) / 2 + lo;
        long midTimeStamp = m_events[mid].m_timeStamp;
        if (midTimeStamp >= timeStamp) {
            hi = mid - 1;
            result = mid;
        } else {
            lo = mid + 1;
        }
    }
    return result;
}
void BundleActiveEventList::Merge(const BundleActiveEventList& right) {
    int size = right.m_events.size();
    for (int i = 0; i < size; i++) {
        Insert(right.m_events[i]);
    }
}

}
}
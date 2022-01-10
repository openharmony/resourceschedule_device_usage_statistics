#ifndef BUNDLE_ACTIVE_EVENT_LIST_H
#define BUNDLE_ACTIVE_EVENT_LIST_H

#include "bundle_active_iservice.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {
class BundleActiveEventList {
public:
    BundleActiveEventList();
    int Size();
    void Clear();
    void Insert(BundleActiveEvent event);
    int FirstIndexOnOrAfter(long timeStamp);
    void Merge(const BundleActiveEventList& right);
private:
    std::vector<BundleActiveEvent> m_events;
};

}
}
#endif
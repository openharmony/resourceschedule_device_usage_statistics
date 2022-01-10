#ifndef BUNDLE_ACTIVE_ISERVICE_H
#define BUNDLE_ACTIVE_ISERVICE_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include "bundle_active_log.h"
#include "iremote_broker.h"
#include "iremote_stub.h"
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace BundleActive {
    
class IBundleActiveService : public IRemoteBroker {
public:
    virtual int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) = 0;
    virtual int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) = 0;
    virtual int Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) = 0;

public:
    enum {
        REPORT_EVENT = 1,
        IS_BUNDLE_IDLE = 2,
        QUERY = 3,
    };
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"Resourceschedule.IBundleActiveService");
};

}
}

#endif
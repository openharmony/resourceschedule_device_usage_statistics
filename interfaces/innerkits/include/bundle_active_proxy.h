#ifndef BUNDLE_ACTIVE_PROXY_H
#define BUNDLE_ACTIVE_PROXY_H

#include "bundle_active_iservice.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveProxy : public IRemoteProxy<IBundleActiveService> {
public:
    int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) override;
    int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) override;
    int Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) override;

public:
    explicit BundleActiveProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IBundleActiveService>(impl) {}
    virtual ~BundleActiveProxy() {}
    
private:
    static inline BrokerDelegator<BundleActiveProxy> delegator_;
};

}
}

#endif

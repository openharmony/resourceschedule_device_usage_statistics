#ifndef BUNDLE_ACTIVE_CLIENT_H
#define BUNDLE_ACTIVE_CLIENT_H

#include "bundle_active_iservice.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveClient {
public:
    int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId);
    int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId);
    int Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId);
    static BundleActiveClient& GetInstance();
public:
    BundleActiveClient() {}
    ~BundleActiveClient() {};

private:
    bool GetBundleActiveProxy();
    sptr<IBundleActiveService> bundleActiveProxy;
};

} 
}
#endif
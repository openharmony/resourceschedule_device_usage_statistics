#ifndef BUNDLE_ACTIVE_SERVICE_H
#define BUNDLE_ACTIVE_SERVICE_H

#include "bundle_active_iservice.h"
#include "bundle_active_stub.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveService : public SystemAbility, public BundleActiveStub {
    DECLARE_SYSTEM_ABILITY(BundleActiveService);

public:
    int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) override;
    int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) override;
    int Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) override;
    //void onStatsUpdated() override;
    //void onstatsReloaded() override;
    //oid onNewUpdate(int userId) override;
public:
    BundleActiveService(int32_t systemAbilityId, int runOnCreate)
        : SystemAbility(systemAbilityId, runOnCreate) {}
    ~BundleActiveService() {}

protected:
    void OnStart() override;
    void OnStop() override;

};

} 
}
#endif
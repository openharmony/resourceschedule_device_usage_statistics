#include "bundle_active_service.h"

namespace OHOS {
namespace BundleActive {
REGISTER_SYSTEM_ABILITY_BY_ID(BundleActiveService, BUNDLE_ACTIVE_SYS_ABILITY_ID, true);

void BundleActiveService::OnStart()
{
    int ret = Publish(this);
    if (!ret) {
        BUNDLE_ACTIVE_LOGE("[Server] OnStart, Register SystemAbility[1906] FAIL.");
        return;
    }
    BUNDLE_ACTIVE_LOGI("[Server] OnStart, Register SystemAbility[1906] SUCCESS.");
}
//void BundleActiveService::onStatsUpdated() {}
//void BundleActiveService::onstatsReloaded() {}
//void BundleActiveService::onNewUpdate(int userId) {}
void BundleActiveService::OnStop()
{
    BUNDLE_ACTIVE_LOGI("[Server] OnStop");
}
int BundleActiveService::ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) {
    BUNDLE_ACTIVE_LOGI("Report event called");
    return 111;
}

int BundleActiveService::IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) {
    BUNDLE_ACTIVE_LOGI("Is bundle active called");
    return true;
}

int BundleActiveService::Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) {
    BUNDLE_ACTIVE_LOGI("Query called");
    return 222;
}

}
}
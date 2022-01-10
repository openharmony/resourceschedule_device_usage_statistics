#include "bundle_active_proxy.h"

namespace OHOS{
namespace BundleActive {

int BundleActiveProxy::ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) {
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteString(bundleName);
    data.WriteString(abilityName);
    data.WriteInt32(abilityId);
    data.WriteInt32(userId);
    data.WriteInt32(eventId);
    Remote() -> SendRequest(REPORT_EVENT, data, reply, option);

    int32_t result = reply.ReadInt32();
    return result;
}

int BundleActiveProxy::IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) {
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteString(bundleName);
    data.WriteString(abilityName);
    data.WriteInt32(abilityId);
    data.WriteInt32(userId);
    Remote() -> SendRequest(IS_BUNDLE_IDLE, data, reply, option);

    int32_t result = reply.ReadInt32();
    return result;
}

int BundleActiveProxy::Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) {
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteString(bundleName);
    data.WriteString(abilityName);
    data.WriteInt32(abilityId);
    data.WriteInt32(userId);
    Remote() -> SendRequest(QUERY, data, reply, option);

    int32_t result = reply.ReadInt32();
    return result;
}

}
}
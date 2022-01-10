#include <string>
#include "bundle_active_stub.h"

namespace OHOS {
namespace BundleActive {

int32_t BundleActiveStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply, MessageOption &option) {
    switch(code) {
        case REPORT_EVENT: {
            std::string bundleName = data.ReadString();
            std::string ablityName = data.ReadString();
            int abilityId = data.ReadInt32();
            int userId = data.ReadInt32();
            int eventId = data.ReadInt32();
            int result = ReportEvent(bundleName, ablityName, abilityId, userId, eventId);
            return reply.WriteInt32(result);
        }
        case IS_BUNDLE_IDLE: {
            std::string bundleName = data.ReadString();
            std::string ablityName = data.ReadString();
            int abilityId = data.ReadInt32();
            int userId = data.ReadInt32();
            int result = IsBundleIdle(bundleName, ablityName, abilityId, userId);
            return reply.WriteInt32(result);
        }
        case QUERY: {
            std::string bundleName = data.ReadString();
            std::string ablityName = data.ReadString();
            int abilityId = data.ReadInt32();
            int userId = data.ReadInt32();
            int result = Query(bundleName, ablityName, abilityId, userId);
            return reply.WriteInt32(result);           
        }

        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

}
}
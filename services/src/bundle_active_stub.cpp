/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
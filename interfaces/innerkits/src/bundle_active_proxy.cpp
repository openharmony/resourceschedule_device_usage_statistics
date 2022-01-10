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
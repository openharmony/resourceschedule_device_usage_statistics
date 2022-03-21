/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "bundle_active_stub.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
int32_t BundleActiveStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return -1;
    }
    switch (code) {
        case REPORT_EVENT: {
            std::string bundleName = data.ReadString();
            std::string ablityName = data.ReadString();
            std::string abilityId = data.ReadString();
            std::string continuousTaskName = data.ReadString();
            int userId = data.ReadInt32();
            int eventId = data.ReadInt32();
            int result = ReportEvent(bundleName, ablityName, abilityId, continuousTaskName, userId, eventId);
            return reply.WriteInt32(result);
        }
        case IS_BUNDLE_IDLE: {
            std::string bundleName = data.ReadString();
            int result = IsBundleIdle(bundleName);
            return reply.WriteInt32(result);
        }
        case QUERY_USAGE_STATS: {
            std::vector<BundleActivePackageStats> result;
            int size = 0;
            int intervalType = data.ReadInt32();
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_USAGE_STATS intervaltype is %{public}d",
                intervalType);
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            int32_t errCode = data.ReadInt32();
            result = QueryPackageStats(intervalType, beginTime, endTime, errCode);
            reply.WriteInt32(errCode);
            size = static_cast<int>(result.size());
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_USAGE_STATS result size is %{public}d", size);
            reply.WriteInt32(size);
            for (int i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_EVENTS: {
            std::vector<BundleActiveEvent> result;
            int size = 0;
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            int32_t errCode = data.ReadInt32();
            result = QueryEvents(beginTime, endTime, errCode);
            size = static_cast<int>(result.size());
            reply.WriteInt32(errCode);
            reply.WriteInt32(size);
            for (int i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case SET_BUNDLE_GROUP: {
            std::string bundleName = data.ReadString();
            int newGroup = data.ReadInt32();
            int userId = data.ReadInt32();
            SetBundleGroup(bundleName, newGroup, userId);
            return 0;
        }
        case QUERY_CURRENT_USAGE_STATS: {
            std::vector<BundleActivePackageStats> result;
            int size = 0;
            int intervalType = data.ReadInt32();
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_CURRENT_USAGE_STATS intervaltype "
                "is %{public}d", intervalType);
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            result = QueryCurrentPackageStats(intervalType, beginTime, endTime);
            size = static_cast<int>(result.size());
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_CURRENT_USAGE_STATS result size "
                "is %{public}d", size);
            reply.WriteInt32(size);
            for (int i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_CURRENT_EVENTS: {
            std::vector<BundleActiveEvent> result;
            int size = 0;
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            result = QueryCurrentEvents(beginTime, endTime);
            size = static_cast<int>(result.size());
            reply.WriteInt32(size);
            for (int i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_BUNDLE_GROUP: {
            int result = -1;
            result = QueryPackageGroup();
            return reply.WriteInt32(result);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


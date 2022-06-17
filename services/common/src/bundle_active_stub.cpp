/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2022. All rights reserved.
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
#include "bundle_active_event_stats.h"
#include "bundle_active_module_record.h"

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
            int32_t userId = data.ReadInt32();
            std::shared_ptr<BundleActiveEvent> tmpEvent;
            tmpEvent = tmpEvent->UnMarshalling(data);
            if (!tmpEvent) {
                return -1;
            }
            int32_t result = ReportEvent(*tmpEvent, userId);
            return reply.WriteInt32(result);
        }
        case IS_BUNDLE_IDLE: {
            std::string bundleName = data.ReadString();
            int32_t errCode = data.ReadInt32();
            int32_t userId = data.ReadInt32();
            int32_t result = IsBundleIdle(bundleName, errCode, userId);
            reply.WriteInt32(result);
            return reply.WriteInt32(errCode);
        }
        case QUERY_USAGE_STATS: {
            std::vector<BundleActivePackageStats> result;
            int32_t intervalType = data.ReadInt32();
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_USAGE_STATS intervaltype is %{public}d", intervalType);
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            int32_t userId = data.ReadInt32();
            int32_t errCode = data.ReadInt32();
            result = QueryPackageStats(intervalType, beginTime, endTime, errCode, userId);
            reply.WriteInt32(errCode);
            int32_t size = static_cast<int32_t>(result.size());
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_USAGE_STATS result size is %{public}d", size);
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_EVENTS: {
            std::vector<BundleActiveEvent> result;
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            int32_t userId = data.ReadInt32();
            int32_t errCode = data.ReadInt32();
            result = QueryEvents(beginTime, endTime, errCode, userId);
            int32_t size = static_cast<int32_t>(result.size());
            reply.WriteInt32(errCode);
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case SET_BUNDLE_GROUP: {
            std::string bundleName = data.ReadString();
            int32_t newGroup = data.ReadInt32();
            int32_t errCode = data.ReadInt32();
            int32_t userId = data.ReadInt32();
            int32_t result = SetBundleGroup(bundleName, newGroup, errCode, userId);
            return reply.WriteInt32(result);
        }
        case QUERY_CURRENT_USAGE_STATS: {
            std::vector<BundleActivePackageStats> result;
            int32_t intervalType = data.ReadInt32();
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_CURRENT_USAGE_STATS intervaltype is %{public}d", intervalType);
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            result = QueryCurrentPackageStats(intervalType, beginTime, endTime);
            int32_t size = static_cast<int32_t>(result.size());
            BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_CURRENT_USAGE_STATS result size is %{public}d", size);
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_CURRENT_EVENTS: {
            std::vector<BundleActiveEvent> result;
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            result = QueryCurrentEvents(beginTime, endTime);
            int32_t size = static_cast<int32_t>(result.size());
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_BUNDLE_GROUP: {
            std::string bundleName = data.ReadString();
            int32_t userId = data.ReadInt32();
            int32_t result = QueryPackageGroup(bundleName, userId);
            return reply.WriteInt32(result);
        }
        case QUERY_FORM_STATS: {
            std::vector<BundleActiveModuleRecord> results;
            int32_t maxNum = data.ReadInt32();
            int32_t userId = data.ReadInt32();
            int32_t errCode = QueryFormStatistics(maxNum, results, userId);
            int32_t size = static_cast<int32_t>(results.size());
            reply.WriteInt32(errCode);
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = results[i].Marshalling(reply);
                if (tmp == false) {
                    return 1;
                }
            }
            return size == 0;
        }
        case REGISTER_GROUP_CALLBACK: {
            auto observer = iface_cast<IBundleActiveGroupCallback>(data.ReadRemoteObject());
            if (!observer) {
                BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack observer is null, return");
                return false;
            }
            BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack observer is ok");
            int32_t result = RegisterGroupCallBack(observer);
            return reply.WriteInt32(result);
        }
        case UNREGISTER_GROUP_CALLBACK: {
            auto observer = iface_cast<IBundleActiveGroupCallback>(data.ReadRemoteObject());
            if (!observer) {
                BUNDLE_ACTIVE_LOGE("unRegisterGroupCallBack observer is null, return");
                return false;
            }
            int32_t result = UnregisterGroupCallBack(observer);
            return reply.WriteInt32(result);
        }
        case QUERY_EVENT_STATS: {
            std::vector<BundleActiveEventStats> result;
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            int32_t userId = data.ReadInt32();
            int32_t errCode = QueryEventStats(beginTime, endTime, result, userId);
            int32_t size = static_cast<int32_t>(result.size());
            reply.WriteInt32(errCode);
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (!tmp) {
                    return 1;
                }
            }
            return size == 0;
        }
        case QUERY_APP_NOTIFICATION_NUMBER: {
            std::vector<BundleActiveEventStats> result;
            int64_t beginTime = data.ReadInt64();
            int64_t endTime = data.ReadInt64();
            int32_t userId = data.ReadInt32();
            int32_t errCode = QueryAppNotificationNumber(beginTime, endTime, result, userId);
            int32_t size = static_cast<int32_t>(result.size());
            reply.WriteInt32(errCode);
            reply.WriteInt32(size);
            for (int32_t i = 0; i < size; i++) {
                bool tmp = result[i].Marshalling(reply);
                if (!tmp) {
                    return 1;
                }
            }
            return size == 0;
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


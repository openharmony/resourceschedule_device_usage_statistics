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

#include "ipc_object_stub.h"
#include "iremote_broker.h"

#include "bundle_active_event.h"
#include "bundle_active_event_stats.h"
#include "bundle_state_inner_errors.h"
#include "bundle_active_log.h"
#include "bundle_active_module_record.h"
#include "bundle_active_package_stats.h"
#include "iapp_group_callback.h"
#include "ibundle_active_service_ipc_interface_code.h"

namespace OHOS {
namespace DeviceUsageStats {
namespace {
    constexpr int32_t EVENT_MAX_SIZE = 100000;
    constexpr int32_t PACKAGE_MAX_SIZE = 1000;
}

int32_t BundleActiveStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel &reply,
    MessageOption &option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return -1;
    }
    switch (code) {
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::REPORT_EVENT): {
            return HandleReportEvent(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::IS_BUNDLE_IDLE): {
            return HandleIsBundleIdle(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_BUNDLE_STATS_INFO_BY_INTERVAL): {
            return HandleQueryBundleStatsInfoByInterval(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_BUNDLE_EVENTS): {
            return HandleQueryBundleEvents(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::SET_APP_GROUP): {
            return HandleSetAppGroup(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_BUNDLE_STATS_INFOS): {
            return HandleQueryBundleStatsInfos(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_CURRENT_BUNDLE_EVENTS): {
            return HandleQueryCurrentBundleEvents(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_APP_GROUP): {
            return HandleQueryAppGroup(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_MODULE_USAGE_RECORDS): {
            return HandleQueryModuleUsageRecords(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::REGISTER_APP_GROUP_CALLBACK): {
            return HandleRegisterAppGroupCallBack(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::UNREGISTER_APP_GROUP_CALLBACK): {
            return HandleUnRegisterAppGroupCallBack(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_DEVICE_EVENT_STATES): {
            return HandleQueryDeviceEventStats(data, reply);
        }
        case static_cast<uint32_t>(IBundleActiveServiceInterfaceCode::QUERY_NOTIFICATION_NUMBER): {
            return HandleQueryNotificationEventStats(data, reply);
        }
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_OK;
}

ErrCode BundleActiveStub::HandleReportEvent(MessageParcel& data, MessageParcel& reply)
{
    int32_t userId = data.ReadInt32();
    std::shared_ptr<BundleActiveEvent> tmpEvent = BundleActiveEvent::UnMarshalling(data);
    if (!tmpEvent) {
        return -1;
    }
    int32_t result = ReportEvent(*tmpEvent, userId);
    return reply.WriteInt32(result);
}

ErrCode BundleActiveStub::HandleIsBundleIdle(MessageParcel& data, MessageParcel& reply)
{
    bool isBundleIdle = false;
    std::string bundleName = data.ReadString();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = IsBundleIdle(isBundleIdle, bundleName, userId);
    reply.WriteInt32(isBundleIdle);
    return reply.WriteInt32(errCode);
}

ErrCode BundleActiveStub::HandleQueryBundleStatsInfoByInterval(MessageParcel& data, MessageParcel& reply)
{
    std::vector<BundleActivePackageStats> result;
    int32_t intervalType = data.ReadInt32();
    BUNDLE_ACTIVE_LOGI("OnRemoteRequest intervaltype is %{public}d", intervalType);
    int64_t beginTime = data.ReadInt64();
    int64_t endTime = data.ReadInt64();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = QueryBundleStatsInfoByInterval(result, intervalType, beginTime, endTime, userId);
    int32_t size = static_cast<int32_t>(result.size());
    if (size > PACKAGE_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
    BUNDLE_ACTIVE_LOGI("OnRemoteRequest result size is %{public}d", size);
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

ErrCode BundleActiveStub::HandleQueryBundleEvents(MessageParcel& data, MessageParcel& reply)
{
    std::vector<BundleActiveEvent> result;
    int64_t beginTime = data.ReadInt64();
    int64_t endTime = data.ReadInt64();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = QueryBundleEvents(result, beginTime, endTime, userId);
    int32_t size = static_cast<int32_t>(result.size());
    if (size > EVENT_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
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

ErrCode BundleActiveStub::HandleQueryBundleStatsInfos(MessageParcel& data, MessageParcel& reply)
{
    std::vector<BundleActivePackageStats> result;
    int32_t intervalType = data.ReadInt32();
    BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_BUNDLE_STATS_INFOS intervaltype is %{public}d", intervalType);
    int64_t beginTime = data.ReadInt64();
    int64_t endTime = data.ReadInt64();
    ErrCode errCode = QueryBundleStatsInfos(result, intervalType, beginTime, endTime);
    int32_t size = static_cast<int32_t>(result.size());
    if (size > PACKAGE_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
    BUNDLE_ACTIVE_LOGI("OnRemoteRequest QUERY_BUNDLE_STATS_INFOS result size is %{public}d", size);
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
ErrCode BundleActiveStub::HandleSetAppGroup(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = data.ReadString();
    int32_t newGroup = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = SetAppGroup(bundleName, newGroup, userId);
    return reply.WriteInt32(errCode);
}

ErrCode BundleActiveStub::HandleQueryCurrentBundleEvents(MessageParcel &data, MessageParcel &reply)
{
    std::vector<BundleActiveEvent> result;
    int64_t beginTime = data.ReadInt64();
    int64_t endTime = data.ReadInt64();
    ErrCode errCode = QueryCurrentBundleEvents(result, beginTime, endTime);
    int32_t size = static_cast<int32_t>(result.size());
    if (size > EVENT_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
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

ErrCode BundleActiveStub::HandleQueryModuleUsageRecords(MessageParcel &data, MessageParcel &reply)
{
    std::vector<BundleActiveModuleRecord> results;
    int32_t maxNum = data.ReadInt32();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = QueryModuleUsageRecords(maxNum, results, userId);
    int32_t size = static_cast<int32_t>(results.size());
    if (size > PACKAGE_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
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

ErrCode BundleActiveStub::HandleQueryAppGroup(MessageParcel& data, MessageParcel& reply)
{
    int32_t appGroup = -1;
    std::string bundleName = data.ReadString();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = QueryAppGroup(appGroup, bundleName, userId);
    reply.WriteInt32(appGroup);
    return reply.WriteInt32(errCode);
}

ErrCode BundleActiveStub::HandleRegisterAppGroupCallBack(MessageParcel& data, MessageParcel& reply)
{
    auto observer = iface_cast<IAppGroupCallback>(data.ReadRemoteObject());
    if (!observer) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack observer is null, return");
        return false;
    }
    BUNDLE_ACTIVE_LOGI("RegisterAppGroupCallBack observer is ok");
    ErrCode errCode = RegisterAppGroupCallBack(observer);
    return reply.WriteInt32(errCode);
}

ErrCode BundleActiveStub::HandleUnRegisterAppGroupCallBack(MessageParcel& data, MessageParcel& reply)
{
    auto observer = iface_cast<IAppGroupCallback>(data.ReadRemoteObject());
    if (!observer) {
        BUNDLE_ACTIVE_LOGE("UnRegisterAppGroupCallBack observer is null, return");
        return false;
    }
    ErrCode errCode = UnRegisterAppGroupCallBack(observer);
    return reply.WriteInt32(errCode);
}

ErrCode BundleActiveStub::HandleQueryDeviceEventStats(MessageParcel& data, MessageParcel& reply)
{
    std::vector<BundleActiveEventStats> result;
    int64_t beginTime = data.ReadInt64();
    int64_t endTime = data.ReadInt64();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = QueryDeviceEventStats(beginTime, endTime, result, userId);
    int32_t size = static_cast<int32_t>(result.size());
    if (size > EVENT_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
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

ErrCode BundleActiveStub::HandleQueryNotificationEventStats(MessageParcel& data, MessageParcel& reply)
{
    std::vector<BundleActiveEventStats> result;
    int64_t beginTime = data.ReadInt64();
    int64_t endTime = data.ReadInt64();
    int32_t userId = data.ReadInt32();
    ErrCode errCode = QueryNotificationEventStats(beginTime, endTime, result, userId);
    int32_t size = static_cast<int32_t>(result.size());
    if (size > PACKAGE_MAX_SIZE) {
        errCode = ERR_QUERY_RESULT_TOO_LARGE;
        reply.WriteInt32(errCode);
        return -1;
    }
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
}  // namespace DeviceUsageStats
}  // namespace OHOS


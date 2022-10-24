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

#include "bundle_active_proxy.h"

namespace OHOS {
namespace DeviceUsageStats {
ErrCode BundleActiveProxy::ReportEvent(BundleActiveEvent& event, const int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt32(userId);
    event.Marshalling(data);
    Remote() -> SendRequest(REPORT_EVENT, data, reply, option);

    int32_t result = reply.ReadInt32();
    return result;
}

ErrCode BundleActiveProxy::IsBundleIdle(bool& isBundleIdle, const std::string& bundleName, int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor()) ||
        !data.WriteString(bundleName) ||
        !data.WriteInt32(userId)) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    Remote() -> SendRequest(IS_BUNDLE_IDLE, data, reply, option);
    isBundleIdle = reply.ReadInt32();
    return reply.ReadInt32();
}

ErrCode BundleActiveProxy::QueryBundleStatsInfoByInterval(std::vector<BundleActivePackageStats>& PackageStats,
    const int32_t intervalType, const int64_t beginTime, const int64_t endTime, int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt32(intervalType);
    data.WriteInt64(beginTime);
    data.WriteInt64(endTime);
    data.WriteInt32(userId);
    Remote() -> SendRequest(QUERY_BUNDLE_STATS_INFO_BY_INTERVAL, data, reply, option);
    ErrCode errCode = reply.ReadInt32();
    int32_t size = reply.ReadInt32();
    std::shared_ptr<BundleActivePackageStats> tmp;
    for (int32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(reply);
        if (tmp == nullptr) {
            continue;
        }
        PackageStats.push_back(*tmp);
    }
    for (uint32_t i = 0; i < PackageStats.size(); i++) {
        BUNDLE_ACTIVE_LOGD("QueryBundleStatsInfoByInterval PackageStats idx is %{public}d, bundleName_ is %{public}s, "
            "lastTimeUsed_ is %{public}lld, lastContiniousTaskUsed_ is %{public}lld, "
            "totalInFrontTime_ is %{public}lld, totalContiniousTaskUsedTime_ is %{public}lld",
            i + 1, PackageStats[i].bundleName_.c_str(),
            (long long)PackageStats[i].lastTimeUsed_, (long long)PackageStats[i].lastContiniousTaskUsed_,
            (long long)PackageStats[i].totalInFrontTime_, (long long)PackageStats[i].totalContiniousTaskUsedTime_);
    }
    return errCode;
}

ErrCode BundleActiveProxy::QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents,
    const int64_t beginTime, const int64_t endTime, int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt64(beginTime);
    data.WriteInt64(endTime);
    data.WriteInt32(userId);
    Remote() -> SendRequest(QUERY_BUNDLE_EVENTS, data, reply, option);
    ErrCode errCode = reply.ReadInt32();
    int32_t size = reply.ReadInt32();
    std::shared_ptr<BundleActiveEvent> tmp;
    for (int32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(reply);
        if (tmp == nullptr) {
            continue;
        }
        bundleActiveEvents.push_back(*tmp);
    }
    for (uint32_t i = 0; i < bundleActiveEvents.size(); i++) {
        bundleActiveEvents[i].PrintEvent(true);
    }
    return errCode;
}

ErrCode BundleActiveProxy::SetAppGroup(const std::string& bundleName, int32_t newGroup, int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteString(bundleName);
    data.WriteInt32(newGroup);
    data.WriteInt32(userId);

    Remote() -> SendRequest(SET_APP_GROUP, data, reply, option);
    return reply.ReadInt32();
}

ErrCode BundleActiveProxy::QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& bundleActivePackageStats,
    const int32_t intervalType, const int64_t beginTime, const int64_t endTime)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt32(intervalType);
    data.WriteInt64(beginTime);
    data.WriteInt64(endTime);
    Remote() -> SendRequest(QUERY_BUNDLE_STATS_INFOS, data, reply, option);
    ErrCode errCode = reply.ReadInt32();
    int32_t size = reply.ReadInt32();
    std::shared_ptr<BundleActivePackageStats> tmp;
    for (int32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(reply);
        if (tmp == nullptr) {
            continue;
        }
        bundleActivePackageStats.push_back(*tmp);
    }
    for (uint32_t i = 0; i < bundleActivePackageStats.size(); i++) {
        BUNDLE_ACTIVE_LOGD("bundleActivePackageStats idx is %{public}d, bundleName_ is %{public}s, "
            "lastTimeUsed_ is %{public}lld, lastContiniousTaskUsed_ is %{public}lld, "
            "totalInFrontTime_ is %{public}lld, totalContiniousTaskUsedTime_ is %{public}lld",
            i + 1, bundleActivePackageStats[i].bundleName_.c_str(),
            (long long)bundleActivePackageStats[i].lastTimeUsed_,
            (long long)bundleActivePackageStats[i].lastContiniousTaskUsed_,
            (long long)bundleActivePackageStats[i].totalInFrontTime_,
            (long long)bundleActivePackageStats[i].totalContiniousTaskUsedTime_);
    }
    return errCode;
}

ErrCode BundleActiveProxy::QueryCurrentBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents,
    const int64_t beginTime, const int64_t endTime)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt64(beginTime);
    data.WriteInt64(endTime);
    Remote() -> SendRequest(QUERY_CURRENT_BUNDLE_EVENTS, data, reply, option);
    ErrCode errCode = reply.ReadInt32();
    int32_t size = reply.ReadInt32();
    std::shared_ptr<BundleActiveEvent> tmp;
    for (int32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(reply);
        if (tmp == nullptr) {
            continue;
        }
        bundleActiveEvents.push_back(*tmp);
    }
    for (uint32_t i = 0; i < bundleActiveEvents.size(); i++) {
        BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents event id is %{public}d, bundle name is %{public}s,"
            "time stamp is %{public}lld", bundleActiveEvents[i].eventId_, bundleActiveEvents[i].bundleName_.c_str(),
            (long long)bundleActiveEvents[i].timeStamp_);
    }
    return errCode;
}

ErrCode BundleActiveProxy::QueryAppGroup(int32_t& appGroup, std::string& bundleName, const int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }

    data.WriteString(bundleName);
    data.WriteInt32(userId);
    Remote() -> SendRequest(QUERY_APP_GROUP, data, reply, option);
    appGroup = reply.ReadInt32();
    return reply.ReadInt32();
}

ErrCode BundleActiveProxy::QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
    int32_t userId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt32(maxNum);
    data.WriteInt32(userId);
    Remote() -> SendRequest(QUERY_MODULE_USAGE_RECORDS, data, reply, option);
    ErrCode errCode = reply.ReadInt32();
    int32_t size = reply.ReadInt32();
    std::shared_ptr<BundleActiveModuleRecord> tmp;
    for (int32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(reply);
        if (tmp == nullptr) {
            continue;
        }
        results.emplace_back(*tmp);
    }
    for (const auto& oneModule : results) {
        BUNDLE_ACTIVE_LOGD("bundle name is %{public}s, module name is %{public}s, "
            "lastusedtime is %{public}lld, launchcount is %{public}d", oneModule.bundleName_.c_str(),
            oneModule.moduleName_.c_str(), (long long)oneModule.lastModuleUsedTime_, oneModule.launchedCount_);
        for (const auto& oneForm : oneModule.formRecords_) {
            BUNDLE_ACTIVE_LOGD("form name is %{public}s, form dimension is %{public}d, form id is %{public}lld, "
                "lasttouchtime is %{public}lld, touchcount is %{public}d", oneForm.formName_.c_str(),
                oneForm.formDimension_, (long long)oneForm.formId_,
                (long long)oneForm.formLastUsedTime_, oneForm.count_);
        }
    }
    return errCode;
}

ErrCode BundleActiveProxy::RegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer)
{
    if (!observer) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack observer is nullptr");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack WriteInterfaceToken fail");
        return ERR_PARCEL_WRITE_FALIED;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack observer write failed.");
        return ERR_PARCEL_WRITE_FALIED;
    }
    int32_t ret = Remote()->SendRequest(REGISTER_APP_GROUP_CALLBACK, data, reply, option);
    if (ret!= ERR_OK) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack SendRequest failed, error code: %{public}d", ret);
    }
    return reply.ReadInt32();
}

ErrCode BundleActiveProxy::UnRegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer)
{
    if (!observer) {
        BUNDLE_ACTIVE_LOGE("UnRegisterAppGroupCallBack observer is nullptr");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        BUNDLE_ACTIVE_LOGE("UnRegisterAppGroupCallBack observer write failed.");
        return ERR_PARCEL_WRITE_FALIED;
    }
    Remote()->SendRequest(UNREGISTER_APP_GROUP_CALLBACK, data, reply, option);
    return reply.ReadInt32();
}

ErrCode BundleActiveProxy::QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    ErrCode errCode = IPCCommunication(beginTime, endTime, eventStats, userId, QUERY_DEVICE_EVENT_STATES);
    for (const auto& singleEvent : eventStats) {
        BUNDLE_ACTIVE_LOGD("QueryDeviceEventStats name is %{public}s, eventId is %{public}d, count is %{public}d",
            singleEvent.name_.c_str(), singleEvent.eventId_, singleEvent.count_);
    }
    return errCode;
}

ErrCode BundleActiveProxy::QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    ErrCode errCode = IPCCommunication(beginTime, endTime, eventStats, userId, QUERY_NOTIFICATION_NUMBER);
    for (const auto& singleEvent : eventStats) {
        BUNDLE_ACTIVE_LOGD("QueryNotificationEventStats name is %{public}s, eventId is %{public}d, count is %{public}d",
            singleEvent.name_.c_str(), singleEvent.eventId_, singleEvent.count_);
    }
    return errCode;
}

ErrCode BundleActiveProxy::IPCCommunication(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId, int32_t communicationFlag)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return ERR_PARCEL_WRITE_FALIED;
    }
    data.WriteInt64(beginTime);
    data.WriteInt64(endTime);
    data.WriteInt32(userId);
    Remote() -> SendRequest(communicationFlag, data, reply, option);
    ErrCode errCode = reply.ReadInt32();
    int32_t size = reply.ReadInt32();
    std::shared_ptr<BundleActiveEventStats> tmp;
    for (int32_t i = 0; i < size; i++) {
        tmp = tmp->UnMarshalling(reply);
        if (!tmp) {
            continue;
        }
        eventStats.emplace_back(*tmp);
    }
    return errCode;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


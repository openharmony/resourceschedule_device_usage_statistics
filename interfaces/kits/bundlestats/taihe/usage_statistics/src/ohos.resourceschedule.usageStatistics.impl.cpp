/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ohos.resourceschedule.usageStatistics.proj.hpp"
#include "ohos.resourceschedule.usageStatistics.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include <mutex>

#include "bundle_active_client.h"
#include "bundle_active_log.h"
#include "bundle_state_idl_condition.h"
#include "bundle_state_idl_err_code.h"
#include "bundle_state_idl_common.h"
#include "app_group_callback_stub.h"

using namespace taihe;
using namespace OHOS::DeviceUsageStats;
using namespace ohos::resourceschedule::usageStatistics;
using namespace OHOS;
const int32_t MAXNUM_UP_LIMIT = 1000;
const int64_t TIME_NUMBER_MIN = 0;
const int32_t INTERVAL_NUMBER_MIN = 0;
const int32_t INTERVAL_NUMBER_MAX = 4;
const std::vector<int32_t> GROUP_TYPE_VALUE {10, 20, 30, 40, 50, 60};
namespace {
class GroupChangeObserver : public DeviceUsageStats::AppGroupCallbackStub {
public:
    ErrCode OnAppGroupChanged(const DeviceUsageStats::AppGroupCallbackInfo& appGroupCallbackInfo);
    explicit GroupChangeObserver(std::shared_ptr<taihe::callback<void(
        ::ohos::resourceschedule::usageStatistics::AppGroupCallbackInfo const&)>> callback)
        : innerCallback_(callback) {}
    virtual ~GroupChangeObserver() = default;
    static std::mutex callbackMutex_;
private:
    std::shared_ptr<taihe::callback<void(
        ::ohos::resourceschedule::usageStatistics::AppGroupCallbackInfo const&)>> innerCallback_ = nullptr;
};

sptr<GroupChangeObserver> groupChangeObserver_;
std::mutex GroupChangeObserver::callbackMutex_;

bool CheckBeginTimeAndEndTime(int64_t beginTime, int64_t endTime)
{
    int32_t errCode = 0;
    if (beginTime < TIME_NUMBER_MIN) {
        BUNDLE_ACTIVE_LOGE("CheckBeginTimeAndEndTime failed, beginTime value is invalid.");
        errCode = ParamError::ERR_BEGIN_TIME_LESS_THEN_ZERO;
        set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
        return false;
    }
    if (endTime <= beginTime) {
        BUNDLE_ACTIVE_LOGE("CheckBeginTimeAndEndTime endTime(%{public}lld) <= beginTime(%{public}lld)",
            static_cast<long long>(endTime), static_cast<long long>(beginTime));
        errCode = ParamError::ERR_END_TIME_LESS_THEN_BEGIN_TIME;
        set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
        return false;
    }
    return true;
}

bool CheckInterval(int32_t interval)
{
    if (((interval < INTERVAL_NUMBER_MIN) || (interval > INTERVAL_NUMBER_MAX))) {
        BUNDLE_ACTIVE_LOGE("CheckInterval failed, intervalType number is invalid.");
        int32_t errCode = ParamError::ERR_INTERVAL_OUT_OF_RANGE;
        set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
        return false;
    }
    return true;
}

bool CheckMaxNum(int32_t maxNum)
{
    if (maxNum > MAXNUM_UP_LIMIT || maxNum <= 0) {
        BUNDLE_ACTIVE_LOGE("CheckMaxNum failed, maxNum is larger than 1000 or less/equal than 0");
        int32_t errCode = ParamError::ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND;
        set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
        return false;
    }
    return true;
}

bool CheckNewGroupType(int32_t newGroupType)
{
    for (const auto& item : GROUP_TYPE_VALUE) {
        if (item == newGroupType) {
            return true;
        }
    }
    int32_t errCode = ParamError::ERR_NEW_GROUP_OUT_OF_RANGE;
    set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
    return false;
}

bool IsIdleStateSync(string bundleName)
{
    int32_t appGroup = 0;
    bool isBundleIdle = false;
    int32_t errCode = BundleActiveClient::GetInstance().IsBundleIdle(isBundleIdle,
        std::string(bundleName.c_str()));
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return false;
    }
    return isBundleIdle;
}

double QueryAppGroupSync()
{
    int32_t appGroup = 0;
    std::string bundleName = "";
    int32_t errCode = BundleActiveClient::GetInstance().QueryAppGroup(appGroup, bundleName);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return static_cast<double>(DeviceUsageStats::GroupType::NEVER_GROUP);
    }
    return static_cast<double>(appGroup);
}

int32_t QueryAppGroupSyncByBundleName(string bundleName)
{
    int32_t appGroup = 0;
    int32_t errCode = BundleActiveClient::GetInstance().QueryAppGroup(appGroup, std::string(bundleName.c_str()));
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return static_cast<double>(DeviceUsageStats::GroupType::NEVER_GROUP);
    }
    return static_cast<double>(appGroup);
}

void SetAppGroupSync(string bundleName, double newGroup)
{
    int32_t intNewGroup = static_cast<int32_t>(newGroup);
    if (!CheckNewGroupType(intNewGroup)) {
        return;
    }
    int32_t appGroup = 0;
    int32_t errCode = BundleActiveClient::GetInstance().SetAppGroup(std::string(bundleName.c_str()), intNewGroup);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return;
    }
}

map<string, BundleStatsInfo> QueryBundleStatsInfosAsync(double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    map<string, BundleStatsInfo> bundleStatsInfoMap;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return bundleStatsInfoMap;
    }
    int32_t errCode = 0;
    auto packageStatsMap = BundleStateIDLCommon::QueryBundleStatsInfos(intBeginTime, intEndTime, errCode);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return bundleStatsInfoMap;
    }
    if (packageStatsMap == nullptr) {
        errCode = ServiceError::ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return bundleStatsInfoMap;
    }
    for (auto& iter : *packageStatsMap) {
        auto packageStats = iter.second;
        BundleStatsInfo bundleStatsInfo {
            .id = packageStats.userId_,
        };
        BundleStateIDLCommon::ParseBundleStatsInfo(packageStats, bundleStatsInfo);
        bundleStatsInfoMap.emplace(iter.first, bundleStatsInfo);
    }
    return bundleStatsInfoMap;
}

map<string, array<BundleStatsInfo>> QueryAppStatsInfosAsync(double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    map<string, array<BundleStatsInfo>> appStatsInfosMap;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return appStatsInfosMap;
    }
    int32_t errCode = 0;
    std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> appStatsMap = nullptr;
    appStatsMap = BundleStateIDLCommon::QueryAppStatsInfos(intBeginTime, intEndTime, errCode);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return appStatsInfosMap;
    }
    if (appStatsMap == nullptr) {
        errCode = ServiceError::ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return appStatsInfosMap;
    }
    for (auto& iter : *appStatsMap) {
        std::vector<BundleStatsInfo> packageStatsVector;
        for (auto packageStats: iter.second) {
            BundleStatsInfo bundleStatsInfo {
                .id = static_cast<double>(packageStats.userId_),
            };
            BundleStateIDLCommon::ParseBundleStatsInfo(packageStats, bundleStatsInfo);
            packageStatsVector.push_back(bundleStatsInfo);
        }
        array<BundleStatsInfo> bundleStatsInfoArray(packageStatsVector);
        appStatsInfosMap.emplace(iter.first, bundleStatsInfoArray);
    }
    return appStatsInfosMap;
}

map<string, array<BundleStatsInfo>> QueryLastUseTimeAsync(map<string, array<double>> appInfos)
{
    map<string, array<BundleStatsInfo>> appStatsInfosMap;
    std::map<std::string, std::vector<int64_t>> queryInfos;
    BundleStateIDLCommon::ParseQueryInfosMap(appInfos, queryInfos);
    int32_t errCode = 0;
    std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> appStatsMap = nullptr;
    appStatsMap = BundleStateIDLCommon::QueryLastUseTime(queryInfos, errCode);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return appStatsInfosMap;
    }
    if (appStatsMap == nullptr) {
        errCode = ServiceError::ERR_SYSTEM_ABILITY_SUPPORT_FAILED;
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return appStatsInfosMap;
    }
    for (auto& iter : *appStatsMap) {
        std::vector<BundleStatsInfo> packageStatsVector;
        for (auto packageStats: iter.second) {
            BundleStatsInfo bundleStatsInfo {
                .id = static_cast<double>(packageStats.userId_),
            };
            BundleStateIDLCommon::ParseBundleStatsInfo(packageStats, bundleStatsInfo);
            packageStatsVector.push_back(bundleStatsInfo);
        }
        array<BundleStatsInfo> bundleStatsInfoArray(packageStatsVector);
        appStatsInfosMap.emplace(iter.first, bundleStatsInfoArray);
    }
    return appStatsInfosMap;
}

array<BundleStatsInfo> QueryBundleStatsInfoByIntervalAsync(
    ::ohos::resourceschedule::usageStatistics::IntervalType byInterval, double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    std::vector<BundleStatsInfo> bundleStatsInfoVector;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return array<BundleStatsInfo>(bundleStatsInfoVector);
    }
    int32_t interval = static_cast<int32_t>(byInterval);
    if (!CheckInterval(interval)) {
        return array<BundleStatsInfo>(bundleStatsInfoVector);
    }
    std::vector<BundleActivePackageStats> bundleActivePackageStats;
    int32_t errCode = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(bundleActivePackageStats,
        interval, intBeginTime, intEndTime);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<BundleStatsInfo>(bundleStatsInfoVector);
    }
    for (auto &packageStats: bundleActivePackageStats) {
        BundleStatsInfo bundleStatsInfo {
            .id = packageStats.userId_,
        };
        BundleStateIDLCommon::ParseBundleStatsInfo(packageStats, bundleStatsInfo);
        bundleStatsInfoVector.push_back(bundleStatsInfo);
    }
    return array<BundleStatsInfo>(bundleStatsInfoVector);
}

array<BundleEvents> QueryBundleEventsAsync(double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    std::vector<BundleEvents> bundleEventVector;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return array<BundleEvents>(bundleEventVector);
    }
    std::vector<BundleActiveEvent> bundleActiveEvents;
    int32_t errCode = BundleActiveClient::GetInstance().QueryBundleEvents(bundleActiveEvents,
        intBeginTime, intEndTime);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<BundleEvents>(bundleEventVector);
    }
    for (auto &bundleActiveEvent: bundleActiveEvents) {
        BundleEvents bundleEvent;
        BundleStateIDLCommon::ParseBundleEvents(bundleActiveEvent, bundleEvent);
        bundleEventVector.push_back(bundleEvent);
    }
    return array<BundleEvents>(bundleEventVector);
}

array<BundleEvents> QueryCurrentBundleEventsAsync(double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    std::vector<BundleEvents> bundleEventVector;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return array<BundleEvents>(bundleEventVector);
    }
    std::vector<BundleActiveEvent> bundleActiveEvents;
    int32_t errCode = BundleActiveClient::GetInstance().QueryCurrentBundleEvents(bundleActiveEvents,
        intBeginTime, intEndTime);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<BundleEvents>(bundleEventVector);
    }
    for (auto &bundleActiveEvent: bundleActiveEvents) {
        BundleEvents bundleEvent;
        BundleStateIDLCommon::ParseBundleEvents(bundleActiveEvent, bundleEvent);
        bundleEventVector.push_back(bundleEvent);
    }
    return array<BundleEvents>(bundleEventVector);
}

array<DeviceEventStats> QueryDeviceEventStatsAsync(double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    std::vector<DeviceEventStats> deviceEventStatsVector;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return array<DeviceEventStats>(deviceEventStatsVector);
    }
    std::vector<BundleActiveEventStats> bundleActiveEventStatsVector;
    int32_t errCode = BundleActiveClient::GetInstance().QueryDeviceEventStats(intBeginTime, intEndTime,
        bundleActiveEventStatsVector);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<DeviceEventStats>(deviceEventStatsVector);
    }
    for (auto &bundleActiveEventStats: bundleActiveEventStatsVector) {
        DeviceEventStats deviceEventStats {
            .name = bundleActiveEventStats.name_,
            .eventId = static_cast<double>(bundleActiveEventStats.eventId_),
            .count = static_cast<double>(bundleActiveEventStats.count_),
        };
        deviceEventStatsVector.push_back(deviceEventStats);
    }
    return array<DeviceEventStats>(deviceEventStatsVector);
}

array<DeviceEventStats> QueryNotificationEventStatsAsync(double beginTime, double endTime)
{
    int64_t intBeginTime = static_cast<int64_t>(beginTime);
    int64_t intEndTime = static_cast<int64_t>(endTime);
    std::vector<DeviceEventStats> deviceEventStatsVector;
    if (!CheckBeginTimeAndEndTime(intBeginTime, intEndTime)) {
        return array<DeviceEventStats>(deviceEventStatsVector);
    }
    std::vector<BundleActiveEventStats> bundleActiveEventStatsVector;
    
    int32_t errCode = BundleActiveClient::GetInstance().QueryNotificationEventStats(intBeginTime, intEndTime,
        bundleActiveEventStatsVector);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<DeviceEventStats>(deviceEventStatsVector);
    }
    for (auto &bundleActiveEventStats: bundleActiveEventStatsVector) {
        DeviceEventStats deviceEventStats {
            .name = bundleActiveEventStats.name_,
            .eventId = static_cast<double>(bundleActiveEventStats.eventId_),
            .count = static_cast<double>(bundleActiveEventStats.count_),
        };
        deviceEventStatsVector.push_back(deviceEventStats);
    }
    return array<DeviceEventStats>(deviceEventStatsVector);
}

array<HapModuleInfo> QueryModuleUsageRecordsAsync()
{
    std::vector<BundleActiveModuleRecord> moduleRecords;
    std::vector<HapModuleInfo> hapModuleInfos;
    int32_t errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(MAXNUM_UP_LIMIT, moduleRecords);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<HapModuleInfo>(hapModuleInfos);
    }
    for (auto& moduleRecord: moduleRecords) {
        array<HapFormInfo> hapFromInfos = BundleStateIDLCommon::ParseformRecords(moduleRecord.formRecords_);
        HapModuleInfo hapModuleInfo {
            .bundleName = moduleRecord.bundleName_,
            .moduleName = moduleRecord.moduleName_,
            .launchedCount = static_cast<double>(moduleRecord.launchedCount_),
            .lastModuleUsedTime = static_cast<double>(moduleRecord.lastModuleUsedTime_),
            .formRecords = hapFromInfos,
        };
        hapModuleInfos.emplace_back(hapModuleInfo);
    }
    return array<HapModuleInfo>(hapModuleInfos);
}

array<HapModuleInfo> QueryModuleUsageRecordsAsyncByMaxNum(double maxNum)
{
    int32_t intMaxNum = static_cast<int32_t>(maxNum);
    std::vector<HapModuleInfo> hapModuleInfos;
    if (!CheckMaxNum(intMaxNum)) {
        return array<HapModuleInfo>(hapModuleInfos);
    }
    std::vector<BundleActiveModuleRecord> moduleRecords;
    int32_t errCode = BundleActiveClient::GetInstance().QueryModuleUsageRecords(intMaxNum, moduleRecords);
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return array<HapModuleInfo>(hapModuleInfos);
    }
    for (auto& moduleRecord: moduleRecords) {
        array<HapFormInfo> hapFromInfos = BundleStateIDLCommon::ParseformRecords(moduleRecord.formRecords_);
        HapModuleInfo hapModuleInfo {
            .bundleName = moduleRecord.bundleName_,
            .moduleName = moduleRecord.moduleName_,
            .launchedCount = static_cast<double>(moduleRecord.launchedCount_),
            .lastModuleUsedTime = static_cast<double>(moduleRecord.lastModuleUsedTime_),
            .formRecords = hapFromInfos,
        };
        hapModuleInfos.emplace_back(hapModuleInfo);
    }
    return array<HapModuleInfo>(hapModuleInfos);
}

ErrCode GroupChangeObserver::OnAppGroupChanged(const DeviceUsageStats::AppGroupCallbackInfo& appGroupCallbackInfo)
{
    std::lock_guard<std::mutex> autoLock(GroupChangeObserver::callbackMutex_);
    ohos::resourceschedule::usageStatistics::AppGroupCallbackInfo innerGroupInfo {
        .appOldGroup = static_cast<double>(appGroupCallbackInfo.GetOldGroup()),
        .appNewGroup = static_cast<double>(appGroupCallbackInfo.GetNewGroup()),
        .userId = static_cast<double>(appGroupCallbackInfo.GetUserId()),
        .changeReason = static_cast<double>(appGroupCallbackInfo.GetChangeReason()),
        .bundleName = appGroupCallbackInfo.GetBundleName(),
    };
    if (innerCallback_ != nullptr) {
        (*innerCallback_)(innerGroupInfo);
    }
    return ERR_OK;
}

void RegisterAppGroupCallBackAsync(
    callback_view<void(::ohos::resourceschedule::usageStatistics::AppGroupCallbackInfo const&)> callback)
{
    std::lock_guard<std::mutex> autoLock(GroupChangeObserver::callbackMutex_);
    if (groupChangeObserver_ != nullptr) {
        BUNDLE_ACTIVE_LOGI("registerAppGroupCallBack repeat!");
        int32_t errCode = ParamError::ERR_REPEAT_REGISTER_APP_GROUP_OBSERVER;
        set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
        return;
    }
    std::shared_ptr<taihe::callback<void(
        ::ohos::resourceschedule::usageStatistics::AppGroupCallbackInfo const&)>> taiheCallback =
        std::make_shared<taihe::callback<void(
            ::ohos::resourceschedule::usageStatistics::AppGroupCallbackInfo const&)>>(callback);
    if (taiheCallback == nullptr) {
        int32_t errCode = ParamError::ERR_ASYNC_CALLBACK_NULLPTR;
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return;
    }
    groupChangeObserver_ = new(std::nothrow) GroupChangeObserver(taiheCallback);
    if (groupChangeObserver_ == nullptr) {
        int32_t errCode = ParamError::ERR_ASYNC_CALLBACK_NULLPTR;
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return;
    }
    int32_t errCode = BundleActiveClient::GetInstance().RegisterAppGroupCallBack(groupChangeObserver_);
    if (errCode != ERR_OK) {
        BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(groupChangeObserver_);
        groupChangeObserver_ = nullptr;
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return;
    }
    BUNDLE_ACTIVE_LOGD("registerAppGroupCallBack success!");
}

void UnregisterAppGroupCallBackAsync()
{
    std::lock_guard<std::mutex> autoLock(GroupChangeObserver::callbackMutex_);
    if (groupChangeObserver_ == nullptr) {
        int32_t errCode = ParamError::ERR_APP_GROUP_OBSERVER_IS_NULLPTR;
        BUNDLE_ACTIVE_LOGI("UnregisterAppGroupCallBackobserver is not exist!");
        set_business_error(errCode, BundleStateIDLErrCode::HandleParamErr(errCode, ""));
        return;
    }
    int32_t errCode = BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(groupChangeObserver_);
    groupChangeObserver_ = nullptr;
    if (errCode != ERR_OK) {
        set_business_error(errCode, BundleStateIDLErrCode::GetErrorCode(errCode));
        return;
    }
    BUNDLE_ACTIVE_LOGD("UnregisterAppGroupCallBack success!");
}

}
TH_EXPORT_CPP_API_IsIdleStateSync(IsIdleStateSync);
TH_EXPORT_CPP_API_QueryAppGroupSync(QueryAppGroupSync);
TH_EXPORT_CPP_API_QueryAppGroupSyncByBundleName(QueryAppGroupSyncByBundleName);
TH_EXPORT_CPP_API_SetAppGroupSync(SetAppGroupSync);
TH_EXPORT_CPP_API_QueryBundleStatsInfosAsync(QueryBundleStatsInfosAsync);
TH_EXPORT_CPP_API_QueryAppStatsInfosAsync(QueryAppStatsInfosAsync);
TH_EXPORT_CPP_API_QueryLastUseTimeAsync(QueryLastUseTimeAsync);
TH_EXPORT_CPP_API_QueryBundleStatsInfoByIntervalAsync(QueryBundleStatsInfoByIntervalAsync);
TH_EXPORT_CPP_API_QueryBundleEventsAsync(QueryBundleEventsAsync);
TH_EXPORT_CPP_API_QueryCurrentBundleEventsAsync(QueryCurrentBundleEventsAsync);
TH_EXPORT_CPP_API_QueryDeviceEventStatsAsync(QueryDeviceEventStatsAsync);
TH_EXPORT_CPP_API_QueryNotificationEventStatsAsync(QueryNotificationEventStatsAsync);
TH_EXPORT_CPP_API_QueryModuleUsageRecordsAsync(QueryModuleUsageRecordsAsync);
TH_EXPORT_CPP_API_QueryModuleUsageRecordsAsyncByMaxNum(QueryModuleUsageRecordsAsyncByMaxNum);
TH_EXPORT_CPP_API_RegisterAppGroupCallBackAsync(RegisterAppGroupCallBackAsync);
TH_EXPORT_CPP_API_UnregisterAppGroupCallBackAsync(UnregisterAppGroupCallBackAsync);
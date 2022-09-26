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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_DATA_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_DATA_H

#include <map>
#include <string>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "bundle_active_event.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_package_stats.h"
#include "app_group_observer_napi.h"

namespace OHOS {
namespace DeviceUsageStats {
#define ARGS_ONE 1
#define ARGS_TWO 2
#define ARGS_THREE 3
#define ARGS_FOUR 4

#define PARAM_FIRST 0
#define PARAM_SECOND 1
#define PARAM_THIRD 2
#define PARAM_FOURTH 3

#define BUNDLE_STATE_OK 0
#define INTERVAL_TYPE_DEFAULT 0
#define INTERVAL_NUMBER_MIN 0
#define INTERVAL_NUMBER_MAX 4
#define TIME_NUMBER_MIN 0
#define GROUP_NUMBER_MIN 5
#define GROUP_NUMBER_MAX 50

struct AsyncWorkData {
    explicit AsyncWorkData(napi_env napiEnv);
    virtual ~AsyncWorkData();
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callback = nullptr;
    bool isCallback = false;
    int32_t errorCode = 0;
};

struct CallbackReceiveDataWorker {
    napi_env env = nullptr;
    napi_ref ref = nullptr;
    int32_t oldGroup = 0;
    int32_t newGroup = 0;
    int32_t userId = -1;
    uint32_t changeReason = 0;
    std::string bundleName;
};

struct AsyncCallbackInfoIsIdleState : public AsyncWorkData {
    explicit AsyncCallbackInfoIsIdleState(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName = "";
    bool state = false;
};

struct AsyncQueryAppGroupCallbackInfo : public AsyncWorkData {
    explicit AsyncQueryAppGroupCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    std::string bundleName = "";
    int32_t priorityGroup = -1;
};

struct AsyncCallbackInfoStates : public AsyncWorkData {
    explicit AsyncCallbackInfoStates(napi_env env) : AsyncWorkData(env) {}
    int64_t beginTime = -1;
    int64_t endTime = -1;
    std::vector<BundleActiveEvent> BundleActiveState;
};

struct AsyncCallbackInfoAppUsageByInterval : public AsyncWorkData {
    explicit AsyncCallbackInfoAppUsageByInterval(napi_env env) : AsyncWorkData(env) {}
    int32_t intervalType = -1;
    int64_t beginTime = -1;
    int64_t endTime = -1;
    std::vector<BundleActivePackageStats> packageStats;
};

struct AsyncCallbackInfoEventStats : public AsyncWorkData {
    explicit AsyncCallbackInfoEventStats(napi_env env) : AsyncWorkData(env) {}
    int64_t beginTime = -1;
    int64_t endTime = -1;
    std::vector<BundleActiveEventStats> eventStats;
};

struct AsyncCallbackInfoAppUsage : public AsyncWorkData {
    explicit AsyncCallbackInfoAppUsage(napi_env env) : AsyncWorkData(env) {}
    int64_t beginTime = -1;
    int64_t endTime = -1;
    std::shared_ptr<std::map<std::string, BundleActivePackageStats>> packageStats;
};
struct AsyncCallbackInfoSetAppGroup : public AsyncWorkData {
    explicit AsyncCallbackInfoSetAppGroup(napi_env env) : AsyncWorkData(env) {}
    int32_t newGroup = -1;;
    std::string bundleName = "";
    bool state = true;
};

struct AsyncCallbackInfoModuleRecord : public AsyncWorkData {
    explicit AsyncCallbackInfoModuleRecord(napi_env env) : AsyncWorkData(env) {}
    int32_t maxNum = -1;
    std::vector<BundleActiveModuleRecord> moduleRecords;
};

struct AsyncUnRegisterCallbackInfo : public AsyncWorkData {
    explicit AsyncUnRegisterCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    sptr<AppGroupObserver> observer = nullptr;
    bool state = true;
};

struct AsyncRegisterCallbackInfo : public AsyncWorkData {
    explicit AsyncRegisterCallbackInfo(napi_env env) : AsyncWorkData(env) {}
    sptr<AppGroupObserver> observer = nullptr;
    bool state = true;
};

struct IsIdleStateParamsInfo {
    std::string bundleName = "";
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct QueryAppGroupParamsInfo {
    std::string bundleName;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct StatesParamsInfo {
    int64_t beginTime = -1;
    int64_t endTime = -1;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct AppUsageParamsByIntervalInfo {
    int32_t intervalType = -1;
    int64_t beginTime = -1;
    int64_t endTime = -1;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct QueryBundleStatsParamsInfo {
    int64_t beginTime = -1;
    int64_t endTime = -1;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct ModuleRecordParamsInfo {
    int32_t maxNum = -1;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct ParamsBundleGroupInfo {
    std::string bundleName = "";
    int32_t newGroup = -1;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct RegisterCallbackInfo {
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct UnRegisterCallbackInfo {
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};

struct EventStatesParamsInfo {
    int64_t beginTime = -1;
    int64_t endTime = -1;
    napi_ref callback = nullptr;
    int32_t errorCode = 0;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_DATA_H


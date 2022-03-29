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
#include "bundle_active_package_stats.h"

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

struct CallbackPromiseInfo {
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    bool isCallback = false;
    int errorCode = 0;
};

struct AsyncCallbackInfoIsIdleState {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    std::string bundleName;
    bool state;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoPriorityGroup {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t priorityGroup;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoStates {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int64_t beginTime;
    int64_t endTime;
    std::vector<BundleActiveEvent> BundleActiveState;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoAppUsageByInterval {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t intervalType;
    int64_t beginTime;
    int64_t endTime;
    std::vector<BundleActivePackageStats> packageStats;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoAppUsage {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int64_t beginTime;
    int64_t endTime;
    std::shared_ptr<std::map<std::string, BundleActivePackageStats>> packageStats;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoModuleRecord {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    int32_t maxNum;
    std::vector<BundleActiveModuleRecord> moduleRecords;
    CallbackPromiseInfo info;
};

struct IsIdleStateParamsInfo {
    std::string bundleName;
    napi_ref callback = nullptr;
    int errorCode = 0;
};

struct PriorityGroupParamsInfo {
    napi_ref callback = nullptr;
    int errorCode = 0;
};

struct StatesParamsInfo {
    int64_t beginTime;
    int64_t endTime;
    napi_ref callback = nullptr;
    int errorCode = 0;
};

struct AppUsageParamsByIntervalInfo {
    int32_t intervalType;
    int64_t beginTime;
    int64_t endTime;
    napi_ref callback = nullptr;
    int errorCode = 0;
};

struct AppUsageParamsInfo {
    int64_t beginTime;
    int64_t endTime;
    napi_ref callback = nullptr;
    int errorCode = 0;
};

struct ModuleRecordParamsInfo {
    int32_t maxNum;
    napi_ref callback = nullptr;
    int errorCode = 0;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_DATA_H


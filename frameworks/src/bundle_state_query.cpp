/*
 * Copyright (c) 2022  Huawei Device Co., Ltd.
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
#include "securec.h"

#include "bundle_active_log.h"
#include "bundle_state_common.h"
#include "bundle_state_data.h"
#include "app_group_observer_napi.h"
#include "bundle_state_inner_errors.h"

namespace OHOS {
namespace DeviceUsageStats {
const uint32_t IS_IDLE_STATE_MIN_PARAMS = 1;
const uint32_t IS_IDLE_STATE_PARAMS = 2;
const uint32_t PRIORITY_GROUP_MIN_PARAMS = 0;
const uint32_t PRIORITY_GROUP_MIDDLE_PARAMS = 1;
const uint32_t PRIORITY_GROUP_PARAMS = 2;
const uint32_t STATES_MIN_PARAMS = 2;
const uint32_t STATES_PARAMS = 3;
const uint32_t APP_USAGE_MIN_PARAMS_BY_INTERVAL = 3;
const uint32_t APP_USAGE_PARAMS_BY_INTERVAL = 4;
const uint32_t APP_USAGE_MIN_PARAMS = 2;
const uint32_t APP_USAGE_PARAMS = 3;
const uint32_t SECOND_ARG = 2;
const uint32_t THIRD_ARG = 3;

napi_value ParseIsIdleStateParameters(const napi_env &env, const napi_callback_info &info,
    IsIdleStateParamsInfo &params)
{
    size_t argc = IS_IDLE_STATE_PARAMS;
    napi_value argv[IS_IDLE_STATE_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == IS_IDLE_STATE_MIN_PARAMS || argc == IS_IDLE_STATE_PARAMS,
        "Invalid number of parameters");

    // argv[0] : bundleName
    std::string result = "";
    params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
    if (params.bundleName.empty()) {
        BUNDLE_ACTIVE_LOGE("ParseIsIdleStateParameters failed, bundleName is empty.");
        params.errorCode = ERR_USAGE_STATS_BUNDLENAME_EMPTY;
    }
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    if ((valuetype != napi_string) && (params.errorCode == ERR_OK)) {
        BUNDLE_ACTIVE_LOGE("Wrong argument type, string expected.");
        params.errorCode = ERR_USAGE_STATS_BUNDLENAME_TYPE;
    }

    // argv[1]: callback
    if (argc == IS_IDLE_STATE_PARAMS) {
        napi_valuetype inputValueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &inputValueType));
        NAPI_ASSERT(env, inputValueType == napi_function,
            "ParseIsIdleStateParameters invalid parameter type, function expected.");
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value IsIdleState(napi_env env, napi_callback_info info)
{
    IsIdleStateParamsInfo params;
    ParseIsIdleStateParameters(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoIsIdleState *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoIsIdleState(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo)) != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncCallbackInfoIsIdleState> callbackPtr {asyncCallbackInfo};
    callbackPtr->bundleName = params.bundleName;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "IsIdleState", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoIsIdleState *asyncCallbackInfo = (AsyncCallbackInfoIsIdleState *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance().IsBundleIdle(asyncCallbackInfo->state,
                    asyncCallbackInfo->bundleName);
            } else {
                BUNDLE_ACTIVE_LOGE("IsIdleState, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoIsIdleState *asyncCallbackInfo = (AsyncCallbackInfoIsIdleState *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_get_boolean(env, asyncCallbackInfo->state, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        static_cast<void*>(asyncCallbackInfo),
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    if (callbackPtr->isCallback) {
        callbackPtr.release();
        return BundleStateCommon::NapiGetNull(env);
    } else {
        callbackPtr.release();
        return promise;
    }
}

napi_value ParsePriorityGroupParameters(const napi_env &env, const napi_callback_info &info,
    QueryAppGroupParamsInfo &params, AsyncQueryAppGroupCallbackInfo* &asyncCallbackInfo)
{
    size_t argc = PRIORITY_GROUP_PARAMS;
    napi_value argv[PRIORITY_GROUP_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == PRIORITY_GROUP_MIN_PARAMS || argc == PRIORITY_GROUP_MIDDLE_PARAMS ||
        argc == PRIORITY_GROUP_PARAMS, "Invalid number of parameters");
    std::string result = "";
    params.bundleName = "";
    if (argc == PRIORITY_GROUP_MIDDLE_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        if (valuetype == napi_function) {
            napi_create_reference(env, argv[0], 1, &params.callback);
        } else {
            params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
            if (params.bundleName.empty()) {
                BUNDLE_ACTIVE_LOGE("ParsePriorityGroupParameters failed, bundleName is empty.");
                params.errorCode = ERR_USAGE_STATS_BUNDLENAME_EMPTY;
            }
        }
    } else if (argc == PRIORITY_GROUP_PARAMS) {
        // argv[0] : bundleName
        params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
        if (params.bundleName.empty()) {
            BUNDLE_ACTIVE_LOGE("ParsePriorityGroupParameters failed, bundleName is empty.");
            params.errorCode = ERR_USAGE_STATS_BUNDLENAME_EMPTY;
        }

        // argv[1]: callback
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParsePriorityGroupParameters invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryAppUsagePriorityGroup(napi_env env, napi_callback_info info)
{
    QueryAppGroupParamsInfo params;
    AsyncQueryAppGroupCallbackInfo *asyncCallbackInfo = nullptr;
    ParsePriorityGroupParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK && !asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncQueryAppGroupCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->bundleName = params.bundleName;
    BUNDLE_ACTIVE_LOGD("QueryAppUsagePriorityGroup callbackPtr->bundleName: %{public}s",
        callbackPtr->bundleName.c_str());
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryAppUsagePriorityGroup", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncQueryAppGroupCallbackInfo *asyncCallbackInfo = (AsyncQueryAppGroupCallbackInfo *)data;
            if (asyncCallbackInfo) {
                asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance().QueryAppGroup(
                    asyncCallbackInfo->priorityGroup, asyncCallbackInfo->bundleName);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryAppUsagePriorityGroup, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncQueryAppGroupCallbackInfo *asyncCallbackInfo = (AsyncQueryAppGroupCallbackInfo *)data;
            if (asyncCallbackInfo) {
                if (asyncCallbackInfo->priorityGroup == -1) {
                    asyncCallbackInfo->errorCode = ERR_SERVICE_FAILED;
                }
                napi_value result = nullptr;
                napi_create_int32(env, asyncCallbackInfo->priorityGroup, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        static_cast<void*>(asyncCallbackInfo),
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    if (callbackPtr->isCallback) {
        callbackPtr.release();
        return BundleStateCommon::NapiGetNull(env);
    } else {
        callbackPtr.release();
        return promise;
    }
}

napi_value ParseStatesParameters(const napi_env &env, const napi_callback_info &info, StatesParamsInfo &params)
{
    size_t argc = STATES_PARAMS;
    napi_value argv[STATES_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == STATES_MIN_PARAMS || argc == STATES_PARAMS,
        "Invalid number of parameters");

    // argv[0] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters failed, beginTime type is invalid.");
        params.errorCode = ERR_USAGE_STATS_BEGIN_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK)
        && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters failed, beginTime value is invalid.");
        params.errorCode = ERR_USAGE_STATS_BEGIN_TIME_INVALID;
    }

    // argv[1] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters failed, endTime type is invalid.");
        params.errorCode = ERR_USAGE_STATS_END_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK)
        && (params.endTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters failed, endTime value is invalid.");
        params.errorCode = ERR_USAGE_STATS_END_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_USAGE_STATS_TIME_INTERVAL;
    }

    // argv[SECOND_ARG]: callback
    if (argc == STATES_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[SECOND_ARG], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseStatesParameters invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[SECOND_ARG], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryCurrentBundleActiveStates(napi_env env, napi_callback_info info)
{
    StatesParamsInfo params;
    ParseStatesParameters(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoStates *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoStates(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo)) != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncCallbackInfoStates> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleActiveStates callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleActiveStates callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryCurrentBundleActiveStates", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().QueryCurrentBundleEvents(asyncCallbackInfo->BundleActiveState,
                        asyncCallbackInfo->beginTime, asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryCurrentBundleActiveStates, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventForResult(
                    env, asyncCallbackInfo->BundleActiveState, result, false);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        static_cast<void*>(asyncCallbackInfo),
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    if (callbackPtr->isCallback) {
        callbackPtr.release();
        return BundleStateCommon::NapiGetNull(env);
    } else {
        callbackPtr.release();
        return promise;
    }
}

napi_value QueryBundleActiveStates(napi_env env, napi_callback_info info)
{
    StatesParamsInfo params;
    ParseStatesParameters(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoStates *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoStates(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo)) != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncCallbackInfoStates> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleActiveStates callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleActiveStates callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryBundleActiveStates", NAPI_AUTO_LENGTH, &resourceName));

    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().QueryBundleEvents(asyncCallbackInfo->BundleActiveState,
                        asyncCallbackInfo->beginTime, asyncCallbackInfo->endTime, asyncCallbackInfo->errorCode);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleActiveStates, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventForResult(
                    env, asyncCallbackInfo->BundleActiveState, result, false);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        static_cast<void*>(asyncCallbackInfo),
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    if (callbackPtr->isCallback) {
        callbackPtr.release();
        return BundleStateCommon::NapiGetNull(env);
    } else {
        callbackPtr.release();
        return promise;
    }
}

napi_value ParseAppUsageParametersByInterval(const napi_env &env, const napi_callback_info &info,
    AppUsageParamsByIntervalInfo &params)
{
    size_t argc = APP_USAGE_PARAMS_BY_INTERVAL;
    napi_value argv[APP_USAGE_PARAMS_BY_INTERVAL] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == APP_USAGE_MIN_PARAMS_BY_INTERVAL || argc == APP_USAGE_PARAMS_BY_INTERVAL,
        "Invalid number of parameters");

    // argv[0] : intervalType
    if (BundleStateCommon::GetInt32NumberValue(env, argv[0], params.intervalType) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, intervalType is invalid.");
        params.errorCode = ERR_USAGE_STATS_INTERVAL_TYPE;
    }
    if ((params.errorCode == ERR_OK) && ((params.intervalType < INTERVAL_NUMBER_MIN)
        || (params.intervalType > INTERVAL_NUMBER_MAX))) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, intervalType number is invalid.");
        params.errorCode = ERR_USAGE_STATS_INTERVAL_NUMBER;
    }

    // argv[1] : beginTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.beginTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, beginTime type is invalid.");
        params.errorCode = ERR_USAGE_STATS_BEGIN_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK)
        && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, beginTime value is invalid.");
        params.errorCode = ERR_USAGE_STATS_BEGIN_TIME_INVALID;
    }

    // argv[SECOND_ARG] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[SECOND_ARG], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, endTime type is invalid.");
        params.errorCode = ERR_USAGE_STATS_END_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK)
        && (params.endTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, endTime value is invalid.");
        params.errorCode = ERR_USAGE_STATS_END_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_USAGE_STATS_TIME_INTERVAL;
    }

    // argv[THIRD_ARG]: callback
    if (argc == APP_USAGE_PARAMS_BY_INTERVAL) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[THIRD_ARG], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseAppUsageParametersByInterval invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[THIRD_ARG], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryBundleStateInfoByInterval(napi_env env, napi_callback_info info)
{
    AppUsageParamsByIntervalInfo params;
    ParseAppUsageParametersByInterval(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoAppUsageByInterval(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo)) != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncCallbackInfoAppUsageByInterval> callbackPtr {asyncCallbackInfo};
    callbackPtr->intervalType = params.intervalType;
    BUNDLE_ACTIVE_LOGD("QueryBundleStateInfoByInterval callbackPtr->intervalType: %{public}d",
        callbackPtr->intervalType);
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleStateInfoByInterval callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleStateInfoByInterval callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryBundleStateInfoByInterval", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo = (AsyncCallbackInfoAppUsageByInterval *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(asyncCallbackInfo->packageStats,
                        asyncCallbackInfo->intervalType, asyncCallbackInfo->beginTime, asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleStateInfoByInterval, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo = (AsyncCallbackInfoAppUsageByInterval *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleStateInfoByIntervalForResult(env, asyncCallbackInfo->packageStats, result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        static_cast<void*>(asyncCallbackInfo),
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    if (callbackPtr->isCallback) {
        callbackPtr.release();
        return BundleStateCommon::NapiGetNull(env);
    } else {
        callbackPtr.release();
        return promise;
    }
}

napi_value ParseAppUsageParameters(const napi_env &env, const napi_callback_info &info,
    QueryBundleStatsParamsInfo &params)
{
    size_t argc = APP_USAGE_PARAMS;
    napi_value argv[APP_USAGE_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == APP_USAGE_MIN_PARAMS || argc == APP_USAGE_PARAMS,
        "Invalid number of parameters");

    // argv[0] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters failed, beginTime type is invalid.");
        params.errorCode = ERR_USAGE_STATS_BEGIN_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK) && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters failed failed, beginTime value is invalid.");
        params.errorCode = ERR_USAGE_STATS_BEGIN_TIME_INVALID;
    }

    // argv[1] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters failed, endTime type is invalid.");
        params.errorCode = ERR_USAGE_STATS_END_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK) && (params.endTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters failed failed, endTime value is invalid.");
        params.errorCode = ERR_USAGE_STATS_END_TIME_INVALID;
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_USAGE_STATS_TIME_INTERVAL;
    }

    // argv[SECOND_ARG]: callback
    if (argc == APP_USAGE_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[SECOND_ARG], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseAppUsageParameters invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[SECOND_ARG], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryBundleStateInfos(napi_env env, napi_callback_info info)
{
    QueryBundleStatsParamsInfo params;
    ParseAppUsageParameters(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoAppUsage *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoAppUsage(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo)) != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncCallbackInfoAppUsage> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleStateInfos callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleStateInfos callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryBundleStateInfos", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoAppUsage *asyncCallbackInfo = (AsyncCallbackInfoAppUsage *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->packageStats = BundleStateCommon::QueryBundleStatsInfos(asyncCallbackInfo->beginTime,
                    asyncCallbackInfo->endTime, asyncCallbackInfo->errorCode);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleStateInfos asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoAppUsage *asyncCallbackInfo = (AsyncCallbackInfoAppUsage *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_object(env, &result);
                BundleStateCommon::GetBundleStateInfoForResult(env, asyncCallbackInfo->packageStats, result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        static_cast<void*>(asyncCallbackInfo),
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    if (callbackPtr->isCallback) {
        callbackPtr.release();
        return BundleStateCommon::NapiGetNull(env);
    } else {
        callbackPtr.release();
        return promise;
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


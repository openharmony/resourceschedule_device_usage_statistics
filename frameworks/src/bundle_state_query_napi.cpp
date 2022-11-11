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
#include "bundle_state_query_napi.h"
#include "app_group_observer_napi.h"
#include "bundle_state_inner_errors.h"

namespace OHOS {
namespace DeviceUsageStats {
const uint32_t IS_IDLE_STATE_MIN_PARAMS = 1;
const uint32_t IS_IDLE_STATE_PARAMS = 2;
const uint32_t STATES_MIN_PARAMS = 2;
const uint32_t STATES_PARAMS = 3;
const uint32_t APP_USAGE_MIN_PARAMS_BY_INTERVAL = 3;
const uint32_t APP_USAGE_PARAMS_BY_INTERVAL = 4;
const uint32_t APP_USAGE_MIN_PARAMS = 2;
const uint32_t APP_USAGE_PARAMS = 3;
const uint32_t MODULE_RECORDS_MIN_PARAMS = 0;
const uint32_t MODULE_RECORDS_MIDDLE_PARAMS = 1;
const uint32_t MODULE_RECORDS_PARAMS = 2;
const uint32_t SECOND_ARG = 2;
const uint32_t THIRD_ARG = 3;
const int32_t MAXNUM_UP_LIMIT = 1000;
const std::vector<int32_t> GROUP_TYPE {10, 20, 30, 40, 50, 60};
const uint32_t EVENT_STATES_MIN_PARAMS = 2;
const uint32_t EVENT_STATES_PARAMS = 3;

napi_value ParseQueryModuleUsageRecords(const napi_env &env, const napi_callback_info &info,
    ModuleRecordParamsInfo &params, AsyncCallbackInfoModuleRecord*& asyncCallbackInfo)
{
    size_t argc = MODULE_RECORDS_PARAMS;
    napi_value argv[MODULE_RECORDS_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != MODULE_RECORDS_MIN_PARAMS) && (argc != MODULE_RECORDS_MIDDLE_PARAMS) &&
        (argc != MODULE_RECORDS_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    if (argc == MODULE_RECORDS_MIN_PARAMS) {
        params.maxNum = MAXNUM_UP_LIMIT;
    }
    if (argc == MODULE_RECORDS_MIDDLE_PARAMS) {
        if (BundleStateCommon::GetInt32NumberValue(env, argv[0], params.maxNum) == nullptr) {
            BUNDLE_ACTIVE_LOGI("get module info has only one callback param");
            napi_valuetype valuetype = napi_undefined;
            NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
            if (valuetype != napi_function) {
                params.errorCode = ERR_CALL_BACK_TYPE;
                return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
            }
            params.maxNum = MAXNUM_UP_LIMIT;
            napi_create_reference(env, argv[0], 1, &params.callback);
        } else if (params.maxNum > MAXNUM_UP_LIMIT || params.maxNum <= 0) {
            BUNDLE_ACTIVE_LOGI("get module info has only one maxNum param");
            BUNDLE_ACTIVE_LOGE("ParseQueryModuleUsageRecords failed, maxNum is larger than 1000 or less/equal than 0");
            params.errorCode = ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND;
            return BundleStateCommon::HandleParamErr(env, ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND, "");
        }
    }
    if (argc == MODULE_RECORDS_PARAMS) {
        // argv[0] : maxNum
        if (BundleStateCommon::GetInt32NumberValue(env, argv[0], params.maxNum) == nullptr) {
            BUNDLE_ACTIVE_LOGE("ParseQueryModuleUsageRecords failed, maxNum type is invalid.");
            params.errorCode = ERR_MAX_RECORDS_NUM_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_MAX_RECORDS_NUM_TYPE, "");
        }

        if (params.maxNum > MAXNUM_UP_LIMIT || params.maxNum <= 0) {
            BUNDLE_ACTIVE_LOGE("ParseQueryModuleUsageRecords failed, maxNum is larger than 1000 or less/equal than 0");
            params.errorCode = ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND;
            return BundleStateCommon::HandleParamErr(env, ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND, "");
        }

        // argv[1] : callback
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        if (valuetype != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryModuleUsageRecords(napi_env env, napi_callback_info info)
{
    ModuleRecordParamsInfo params;
    AsyncCallbackInfoModuleRecord *asyncCallbackInfo = nullptr;
    ParseQueryModuleUsageRecords(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    std::unique_ptr<AsyncCallbackInfoModuleRecord> callbackPtr {asyncCallbackInfo};
    callbackPtr->maxNum = params.maxNum;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryModuleUsageRecords", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoModuleRecord *asyncCallbackInfo = (AsyncCallbackInfoModuleRecord *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().QueryModuleUsageRecords(asyncCallbackInfo->maxNum,
                    asyncCallbackInfo->moduleRecords);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleStatsInfoByInterval, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoModuleRecord *asyncCallbackInfo = (AsyncCallbackInfoModuleRecord *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetModuleRecordForResult(env, asyncCallbackInfo->moduleRecords, result);
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

napi_value ParseIsIdleStateParameters(const napi_env &env, const napi_callback_info &info,
    IsIdleStateParamsInfo &params, AsyncCallbackInfoIsIdleState*& asyncCallbackInfo)
{
    size_t argc = IS_IDLE_STATE_PARAMS;
    napi_value argv[IS_IDLE_STATE_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == IS_IDLE_STATE_MIN_PARAMS || argc == IS_IDLE_STATE_PARAMS,
        "Invalid number of parameters");
    if ((argc != IS_IDLE_STATE_MIN_PARAMS) && (argc != IS_IDLE_STATE_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    // argv[0] : bundleName
    std::string result = "";
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    if ((valuetype != napi_string) && (params.errorCode == ERR_OK)) {
        BUNDLE_ACTIVE_LOGE("Wrong argument type, string expected.");
        params.errorCode = ERR_BUNDLE_NAME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
    if (params.bundleName.empty()) {
        BUNDLE_ACTIVE_LOGE("ParseIsIdleStateParameters failed, bundleName is empty.");
        params.errorCode = ERR_PARAMETERS_EMPTY;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "bundleName");
    }

    // argv[1]: callback
    if (argc == IS_IDLE_STATE_PARAMS) {
        napi_valuetype inputValueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &inputValueType));
        if (inputValueType != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value IsIdleState(napi_env env, napi_callback_info info)
{
    IsIdleStateParamsInfo params;
    AsyncCallbackInfoIsIdleState *asyncCallbackInfo = nullptr;
    ParseIsIdleStateParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
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
                asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance().IsBundleIdle(
                    asyncCallbackInfo->state, asyncCallbackInfo->bundleName);
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

napi_value ParseQueryCurrentBundleEventsParameters(const napi_env &env, const napi_callback_info &info,
    StatesParamsInfo &params, AsyncCallbackInfoStates*& asyncCallbackInfo)
{
    size_t argc = STATES_PARAMS;
    napi_value argv[STATES_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != STATES_MIN_PARAMS) && (argc != STATES_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    // argv[0] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseQueryCurrentBundleEventsParameters failed, beginTime type is invalid.");
        params.errorCode = ERR_BEGIN_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK)
        && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryCurrentBundleEventsParameters failed, beginTime value is invalid.");
        params.errorCode = ERR_BEGIN_TIME_LESS_THEN_ZERO;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_LESS_THEN_ZERO, "");
    }

    // argv[1] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryCurrentBundleEventsParameters failed, endTime type is invalid.");
        params.errorCode = ERR_END_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_END_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryCurrentBundleEventsParameters endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_END_TIME_LESS_THEN_BEGIN_TIME;
        return BundleStateCommon::HandleParamErr(env, ERR_END_TIME_LESS_THEN_BEGIN_TIME, "");
    }

    // argv[SECOND_ARG]: callback
    if (argc == STATES_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[SECOND_ARG], &valuetype));
        if (valuetype != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[SECOND_ARG], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryCurrentBundleEvents(napi_env env, napi_callback_info info)
{
    StatesParamsInfo params;
    AsyncCallbackInfoStates *asyncCallbackInfo = nullptr;
    ParseQueryCurrentBundleEventsParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    std::unique_ptr<AsyncCallbackInfoStates> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("QueryCurrentBundleEvents callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryCurrentBundleEvents", NAPI_AUTO_LENGTH, &resourceName));
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
                BUNDLE_ACTIVE_LOGE("QueryCurrentBundleEvents, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventForResult(
                    env, asyncCallbackInfo->BundleActiveState, result, true);
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

napi_value QueryBundleEvents(napi_env env, napi_callback_info info)
{
    StatesParamsInfo params;
    AsyncCallbackInfoStates *asyncCallbackInfo = nullptr;
    ParseQueryCurrentBundleEventsParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    std::unique_ptr<AsyncCallbackInfoStates> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleEvents callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("QueryBundleEvents callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);

    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryBundleEvents", NAPI_AUTO_LENGTH, &resourceName));

    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().QueryBundleEvents(asyncCallbackInfo->BundleActiveState,
                        asyncCallbackInfo->beginTime, asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleEvents, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventForResult(
                    env, asyncCallbackInfo->BundleActiveState, result, true);
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

napi_value ParseQueryBundleStatsInfoByInterval(const napi_env &env, const napi_callback_info &info,
    AppUsageParamsByIntervalInfo &params, AsyncCallbackInfoAppUsageByInterval*& asyncCallbackInfo)
{
    size_t argc = APP_USAGE_PARAMS_BY_INTERVAL;
    napi_value argv[APP_USAGE_PARAMS_BY_INTERVAL] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != APP_USAGE_MIN_PARAMS_BY_INTERVAL) && (argc != APP_USAGE_PARAMS_BY_INTERVAL)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    // argv[0] : intervalType
    if (BundleStateCommon::GetInt32NumberValue(env, argv[0], params.intervalType) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfoByInterval failed, intervalType is invalid.");
        params.errorCode = ERR_INTERVAL_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_INTERVAL_TYPE, "");
    }
    if ((params.errorCode == ERR_OK) && ((params.intervalType < INTERVAL_NUMBER_MIN)
        || (params.intervalType > INTERVAL_NUMBER_MAX))) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfoByInterval failed, intervalType number is invalid.");
        params.errorCode = ERR_INTERVAL_OUT_OF_RANGE;
        return BundleStateCommon::HandleParamErr(env, ERR_INTERVAL_OUT_OF_RANGE, "");
    }

    // argv[1] : beginTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.beginTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfoByInterval failed, beginTime type is invalid.");
        params.errorCode = ERR_BEGIN_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK)
        && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfoByInterval failed, beginTime value is invalid.");
        params.errorCode = ERR_BEGIN_TIME_LESS_THEN_ZERO;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_LESS_THEN_ZERO, "");
    }

    // argv[SECOND_ARG] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[SECOND_ARG], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfoByInterval failed, endTime type is invalid.");
        params.errorCode = ERR_END_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_END_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfoByInterval endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_END_TIME_LESS_THEN_BEGIN_TIME;
        return BundleStateCommon::HandleParamErr(env, ERR_END_TIME_LESS_THEN_BEGIN_TIME, "");
    }

    // argv[THIRD_ARG]: callback
    if (argc == APP_USAGE_PARAMS_BY_INTERVAL) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[THIRD_ARG], &valuetype));
        if (valuetype != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[THIRD_ARG], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryBundleStatsInfoByInterval(napi_env env, napi_callback_info info)
{
    AppUsageParamsByIntervalInfo params;
    AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo = nullptr;
    ParseQueryBundleStatsInfoByInterval(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    std::unique_ptr<AsyncCallbackInfoAppUsageByInterval> callbackPtr {asyncCallbackInfo};
    callbackPtr->intervalType = params.intervalType;
    callbackPtr->beginTime = params.beginTime;
    callbackPtr->endTime = params.endTime;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryBundleStatsInfoByInterval", NAPI_AUTO_LENGTH, &resourceName));
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
                BUNDLE_ACTIVE_LOGE("QueryBundleStatsInfoByInterval, asyncCallbackInfo == nullptr");
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

napi_value ParseQueryBundleStatsInfos(const napi_env &env, const napi_callback_info &info,
    QueryBundleStatsParamsInfo &params, AsyncCallbackInfoAppUsage*& asyncCallbackInfo)
{
    size_t argc = APP_USAGE_PARAMS;
    napi_value argv[APP_USAGE_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != APP_USAGE_MIN_PARAMS) && (argc != APP_USAGE_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    // argv[0] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfos failed, beginTime type is invalid.");
        params.errorCode = ERR_BEGIN_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK) && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfos failed failed, beginTime value is invalid.");
        params.errorCode = ERR_BEGIN_TIME_LESS_THEN_ZERO;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_LESS_THEN_ZERO, "");
    }

    // argv[1] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfos failed, endTime type is invalid.");
        params.errorCode = ERR_END_TIME_TYPE;
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseQueryBundleStatsInfos endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_END_TIME_LESS_THEN_BEGIN_TIME;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_LESS_THEN_ZERO, "");
    }

    // argv[SECOND_ARG]: callback
    if (argc == APP_USAGE_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[SECOND_ARG], &valuetype));
        if (valuetype != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[SECOND_ARG], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryBundleStatsInfos(napi_env env, napi_callback_info info)
{
    QueryBundleStatsParamsInfo params;
    AsyncCallbackInfoAppUsage *asyncCallbackInfo = nullptr;
    ParseQueryBundleStatsInfos(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    std::unique_ptr<AsyncCallbackInfoAppUsage> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGD("queryBundleStatsInfos callbackPtr->beginTime: %{public}lld",
        (long long)callbackPtr->beginTime);
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGD("queryBundleStatsInfos callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->endTime);
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "queryBundleStatsInfos", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoAppUsage *asyncCallbackInfo = (AsyncCallbackInfoAppUsage *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->packageStats = BundleStateCommon::QueryBundleStatsInfos(asyncCallbackInfo->beginTime,
                    asyncCallbackInfo->endTime, asyncCallbackInfo->errorCode);
            } else {
                BUNDLE_ACTIVE_LOGE("queryBundleStatsInfos asyncCallbackInfo == nullptr");
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

napi_value ParseDeviceEventStates(const napi_env &env, const napi_callback_info &info,
    EventStatesParamsInfo &params)
{
    size_t argc = EVENT_STATES_PARAMS;
    napi_value argv[EVENT_STATES_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != EVENT_STATES_MIN_PARAMS) && (argc != EVENT_STATES_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    // argv[0] : beginTime
    if ((params.errorCode == ERR_OK)
        && BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseDeviceEventStates failed, beginTime is invalid.");
        params.errorCode = ERR_BEGIN_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK) && (params.beginTime < TIME_NUMBER_MIN)) {
        BUNDLE_ACTIVE_LOGE("ParseDeviceEventStates failed, beginTime less then 0.");
        params.errorCode = ERR_BEGIN_TIME_LESS_THEN_ZERO;
        return BundleStateCommon::HandleParamErr(env, ERR_BEGIN_TIME_LESS_THEN_ZERO, "");
    }

    // argv[1] : endTime
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseDeviceEventStates failed, endTime is invalid.");
        params.errorCode = ERR_END_TIME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_END_TIME_TYPE, "");
    }
    if ((params.errorCode == ERR_OK) && (params.endTime <= params.beginTime)) {
        BUNDLE_ACTIVE_LOGE("ParseDeviceEventStates endTime(%{public}lld) <= beginTime(%{public}lld)",
            (long long)params.endTime, (long long)params.beginTime);
        params.errorCode = ERR_END_TIME_LESS_THEN_BEGIN_TIME;
        return BundleStateCommon::HandleParamErr(env, ERR_END_TIME_LESS_THEN_BEGIN_TIME, "");
    }

    // argv[SECOND_ARG]: callback
    if (argc == EVENT_STATES_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[SECOND_ARG], &valuetype));
        if (valuetype != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[SECOND_ARG], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryDeviceEventStats(napi_env env, napi_callback_info info)
{
    EventStatesParamsInfo params;
    ParseDeviceEventStates(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoEventStats *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoEventStats(env);
    std::unique_ptr<AsyncCallbackInfoEventStats> callbackPtr =
        BundleStateCommon::HandleEventStatsInfo(env, asyncCallbackInfo, params);
    if (!callbackPtr) {
        return BundleStateCommon::NapiGetNull(env);
    }
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryDeviceEventStats", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoEventStats *asyncCallbackInfo = (AsyncCallbackInfoEventStats *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance()
                    .QueryDeviceEventStats(asyncCallbackInfo->beginTime,
                    asyncCallbackInfo->endTime, asyncCallbackInfo->eventStats);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryDeviceEventStats, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoEventStats *asyncCallbackInfo = (AsyncCallbackInfoEventStats *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventStatsForResult(env, asyncCallbackInfo->eventStats, result);
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

napi_value QueryNotificationEventStats(napi_env env, napi_callback_info info)
{
    EventStatesParamsInfo params;
    ParseDeviceEventStates(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoEventStats *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoEventStats(env);
    std::unique_ptr<AsyncCallbackInfoEventStats> callbackPtr =
        BundleStateCommon::HandleEventStatsInfo(env, asyncCallbackInfo, params);
    if (!callbackPtr) {
        return BundleStateCommon::NapiGetNull(env);
    }
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryNotificationEventStats", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoEventStats *asyncCallbackInfo = (AsyncCallbackInfoEventStats *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance()
                    .QueryNotificationEventStats(asyncCallbackInfo->beginTime,
                    asyncCallbackInfo->endTime, asyncCallbackInfo->eventStats);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryNotificationEventStats, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoEventStats *asyncCallbackInfo = (AsyncCallbackInfoEventStats *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveNotificationNumberForResult(env,
                    asyncCallbackInfo->eventStats, result);
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
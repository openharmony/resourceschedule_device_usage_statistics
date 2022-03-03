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

#include "bundle_state_common.h"
#include "bundle_active_log.h"
#include "bundle_state_data.h"

namespace OHOS {
namespace DeviceUsageStats {
static const int32_t Is_Idle_State_MIN_PARAMS = 1;
static const int32_t Is_Idle_State_PARAMS = 2;
static const int32_t Priority_Group_MIN_PARAMS = 0;
static const int32_t Priority_Group_PARAMS = 1;
static const int32_t States_MIN_PARAMS = 2;
static const int32_t States_PARAMS = 3;
static const int32_t App_Usage_MIN_PARAMS_BY_INTERVAL = 3;
static const int32_t App_Usage_PARAMS_BY_INTERVAL = 4;
static const int32_t App_Usage_MIN_PARAMS = 2;
static const int32_t App_Usage_PARAMS = 3;
static const int32_t SECOND_ARG = 2;
static const int32_t THIRD_ARG = 3;

napi_value ParseIsIdleStateParameters(const napi_env &env, const napi_callback_info &info,
    IsIdleStateParamsInfo &params)
{
    size_t argc = Is_Idle_State_PARAMS;
    napi_value argv[Is_Idle_State_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == Is_Idle_State_MIN_PARAMS || argc == Is_Idle_State_PARAMS,
        "invalid number of parameters");

    // argv[0] : bundleName
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    NAPI_ASSERT(env, valuetype == napi_string, "ParseIsIdleStateParameters invalid parameter type. "
        "String expected.");
    std::string result = "";
    params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
    if (params.bundleName.empty()) {
        BUNDLE_ACTIVE_LOGI("ParseIsIdleStateParameters failed, bundleName is invalid.");
        return nullptr;
    }

    // argv[1]: callback
    if (argc == Is_Idle_State_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseIsIdleStateParameters invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value IsIdleState(napi_env env, napi_callback_info info)
{
    IsIdleStateParamsInfo params;
    if (ParseIsIdleStateParameters(env, info, params) == nullptr) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoIsIdleState *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoIsIdleState {.env = env, .asyncWork = nullptr};
    if (!asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    asyncCallbackInfo->bundleName = params.bundleName;
    BUNDLE_ACTIVE_LOGI("asyncCallbackInfo->bundleName: %{public}s", asyncCallbackInfo->bundleName.c_str());
    BundleStateCommon::SettingCallbackPromiseInfo(env, params.callback, asyncCallbackInfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "IsIdleState", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            BUNDLE_ACTIVE_LOGI("IsIdleState, worker pool thread execute.");
            AsyncCallbackInfoIsIdleState *asyncCallbackInfo = (AsyncCallbackInfoIsIdleState *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->state = BundleActiveClient::GetInstance().IsBundleIdle(
                    asyncCallbackInfo->bundleName);
            } else {
                BUNDLE_ACTIVE_LOGE("IsIdleState, asyncCallbackInfo == nullptr");
            }
            BUNDLE_ACTIVE_LOGI("IsIdleState, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoIsIdleState *asyncCallbackInfo = (AsyncCallbackInfoIsIdleState *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_get_boolean(env, asyncCallbackInfo->state, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, asyncCallbackInfo->info, result);

                if (asyncCallbackInfo->info.callback != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->info.callback);
                }

                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    if (asyncCallbackInfo->info.isCallback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParsePriorityGroupParameters(const napi_env &env, const napi_callback_info &info,
    PriorityGroupParamsInfo &params)
{
    size_t argc = Priority_Group_PARAMS;
    napi_value argv[Priority_Group_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == Priority_Group_MIN_PARAMS || argc == Priority_Group_PARAMS,
        "invalid number of parameters");

    // argv[0]: callback
    if (argc == Priority_Group_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParsePriorityGroupParameters invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[0], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value QueryAppUsagePriorityGroup(napi_env env, napi_callback_info info)
{
    PriorityGroupParamsInfo params;
    if (ParsePriorityGroupParameters(env, info, params) == nullptr) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoPriorityGroup *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoPriorityGroup {.env = env, .asyncWork = nullptr};
    if (!asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    BundleStateCommon::SettingCallbackPromiseInfo(env, params.callback, asyncCallbackInfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "QueryAppUsagePriorityGroup", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            BUNDLE_ACTIVE_LOGI("QueryAppUsagePriorityGroup, worker pool thread execute.");
            AsyncCallbackInfoPriorityGroup *asyncCallbackInfo = (AsyncCallbackInfoPriorityGroup *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->priorityGroup = BundleActiveClient::GetInstance().QueryPackageGroup();
            } else {
                BUNDLE_ACTIVE_LOGE("QueryAppUsagePriorityGroup, asyncCallbackInfo == nullptr");
            }
            BUNDLE_ACTIVE_LOGI("QueryAppUsagePriorityGroup, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoPriorityGroup *asyncCallbackInfo = (AsyncCallbackInfoPriorityGroup *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_int32(env, asyncCallbackInfo->priorityGroup, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, asyncCallbackInfo->info, result);

                if (asyncCallbackInfo->info.callback != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->info.callback);
                }

                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    if (asyncCallbackInfo->info.isCallback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParseStatesParameters(const napi_env &env, const napi_callback_info &info, StatesParamsInfo &params)
{
    size_t argc = States_PARAMS;
    napi_value argv[States_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == States_MIN_PARAMS || argc == States_PARAMS,
        "invalid number of parameters");

    // argv[0] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters failed, beginTime is invalid.");
        return nullptr;
    }

    // argv[1] : endTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseStatesParameters failed, endTime is invalid.");
        return nullptr;
    }

    // argv[SECOND_ARG]: callback
    if (argc == States_PARAMS) {
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
    if (ParseStatesParameters(env, info, params) == nullptr) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoStates *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoStates {.env = env, .asyncWork = nullptr};
    if (!asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    asyncCallbackInfo->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGI("QueryCurrentBundleActiveStates asyncCallbackInfo->beginTime: %{public}lld",
        asyncCallbackInfo->beginTime);
    asyncCallbackInfo->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGI("QueryCurrentBundleActiveStates asyncCallbackInfo->endTime: %{public}lld",
        asyncCallbackInfo->endTime);
    BundleStateCommon::SettingCallbackPromiseInfo(env, params.callback, asyncCallbackInfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "QueryCurrentBundleActiveStates", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            BUNDLE_ACTIVE_LOGI("QueryCurrentBundleActiveStates, worker pool thread execute.");
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->BundleActiveState =
                    BundleActiveClient::GetInstance().QueryCurrentEvents(asyncCallbackInfo->beginTime,
                        asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryCurrentBundleActiveStates, asyncCallbackInfo == nullptr");
            }
            BUNDLE_ACTIVE_LOGI("QueryCurrentBundleActiveStates, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventForResult(env, asyncCallbackInfo->BundleActiveState, result);
                BundleStateCommon::GetCallbackPromiseResult(env, asyncCallbackInfo->info, result);

                if (asyncCallbackInfo->info.callback != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->info.callback);
                }

                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    if (asyncCallbackInfo->info.isCallback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value QueryBundleActiveStates(napi_env env, napi_callback_info info)
{
    StatesParamsInfo params;
    if (ParseStatesParameters(env, info, params) == nullptr) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoStates *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoStates {.env = env, .asyncWork = nullptr};
    if (!asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    asyncCallbackInfo->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGI("QueryBundleActiveStates asyncCallbackInfo->beginTime: %{public}lld",
        asyncCallbackInfo->beginTime);
    asyncCallbackInfo->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGI("QueryBundleActiveStates asyncCallbackInfo->endTime: %{public}lld",
        asyncCallbackInfo->endTime);
    BundleStateCommon::SettingCallbackPromiseInfo(env, params.callback, asyncCallbackInfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "QueryBundleActiveStates", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            BUNDLE_ACTIVE_LOGI("QueryBundleActiveStates, worker pool thread execute.");
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->BundleActiveState =
                    BundleActiveClient::GetInstance().QueryEvents(asyncCallbackInfo->beginTime,
                        asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleActiveStates, asyncCallbackInfo == nullptr");
            }
            BUNDLE_ACTIVE_LOGI("QueryBundleActiveStates, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoStates *asyncCallbackInfo = (AsyncCallbackInfoStates *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleActiveEventForResult(env, asyncCallbackInfo->BundleActiveState, result);
                BundleStateCommon::GetCallbackPromiseResult(env, asyncCallbackInfo->info, result);

                if (asyncCallbackInfo->info.callback != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->info.callback);
                }

                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    if (asyncCallbackInfo->info.isCallback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParseAppUsageParametersByInterval(const napi_env &env, const napi_callback_info &info,
    AppUsageParamsByIntervalInfo &params)
{
    size_t argc = App_Usage_PARAMS_BY_INTERVAL;
    napi_value argv[App_Usage_PARAMS_BY_INTERVAL] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == App_Usage_MIN_PARAMS_BY_INTERVAL || argc == App_Usage_PARAMS_BY_INTERVAL,
        "invalid number of parameters");

    // argv[0] : intervalType
    if (BundleStateCommon::GetInt32NumberValue(env, argv[0], params.intervalType) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, beginTime is invalid.");
        return nullptr;
    }

    // argv[1] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, beginTime is invalid.");
        return nullptr;
    }

    // argv[SECOND_ARG] : endTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[SECOND_ARG], params.endTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParametersByInterval failed, endTime is invalid.");
        return nullptr;
    }

    // argv[THIRD_ARG]: callback
    if (argc == App_Usage_PARAMS_BY_INTERVAL) {
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
    if (ParseAppUsageParametersByInterval(env, info, params) == nullptr) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }

    napi_value promise = nullptr;
    AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoAppUsageByInterval {.env = env, .asyncWork = nullptr};
    if (!asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    asyncCallbackInfo->intervalType = params.intervalType;
    BUNDLE_ACTIVE_LOGI("QueryBundleStateInfoByInterval asyncCallbackInfo->intervalType: %{public}d",
        asyncCallbackInfo->intervalType);
    asyncCallbackInfo->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGI("QueryBundleStateInfoByInterval asyncCallbackInfo->beginTime: %{public}lld",
        asyncCallbackInfo->beginTime);
    asyncCallbackInfo->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGI("QueryBundleStateInfoByInterval asyncCallbackInfo->endTime: %{public}lld",
        asyncCallbackInfo->endTime);
    BundleStateCommon::SettingCallbackPromiseInfo(env, params.callback, asyncCallbackInfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "QueryBundleStateInfoByInterval", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            BUNDLE_ACTIVE_LOGI("QueryBundleStateInfoByInterval, worker pool thread execute.");
            AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo = (AsyncCallbackInfoAppUsageByInterval *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->packageStats =
                    BundleActiveClient::GetInstance().QueryPackageStats(asyncCallbackInfo->intervalType,
                        asyncCallbackInfo->beginTime, asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleStateInfoByInterval, asyncCallbackInfo == nullptr");
            }
            BUNDLE_ACTIVE_LOGI("QueryBundleStateInfoByInterval, worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoAppUsageByInterval *asyncCallbackInfo = (AsyncCallbackInfoAppUsageByInterval *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_array(env, &result);
                BundleStateCommon::GetBundleStateInfoByIntervalForResult(env, asyncCallbackInfo->packageStats, result);
                BundleStateCommon::GetCallbackPromiseResult(env, asyncCallbackInfo->info, result);

                if (asyncCallbackInfo->info.callback != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->info.callback);
                }

                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));

    if (asyncCallbackInfo->info.isCallback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value ParseAppUsageParameters(const napi_env &env, const napi_callback_info &info, AppUsageParamsInfo &params)
{
    size_t argc = App_Usage_PARAMS;
    napi_value argv[App_Usage_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == App_Usage_MIN_PARAMS || argc == App_Usage_PARAMS,
        "invalid number of parameters");

    // argv[0] : beginTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[0], params.beginTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters failed, beginTime is invalid.");
        return nullptr;
    }

    // argv[1] : endTime
    if (BundleStateCommon::GetInt64NumberValue(env, argv[1], params.endTime) == nullptr) {
        BUNDLE_ACTIVE_LOGE("ParseAppUsageParameters failed, endTime is invalid.");
        return nullptr;
    }

    // argv[SECOND_ARG]: callback
    if (argc == App_Usage_PARAMS) {
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
    AppUsageParamsInfo params;
    if (ParseAppUsageParameters(env, info, params) == nullptr) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    napi_value promise = nullptr;
    AsyncCallbackInfoAppUsage *asyncCallbackInfo =
        new (std::nothrow) AsyncCallbackInfoAppUsage {.env = env, .asyncWork = nullptr};
    if (!asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback);
    }
    asyncCallbackInfo->beginTime = params.beginTime;
    BUNDLE_ACTIVE_LOGI("QueryBundleStateInfos asyncCallbackInfo->beginTime: %{public}lld",
        asyncCallbackInfo->beginTime);
    asyncCallbackInfo->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGI("QueryBundleStateInfos asyncCallbackInfo->endTime: %{public}lld",
        asyncCallbackInfo->endTime);
    BundleStateCommon::SettingCallbackPromiseInfo(env, params.callback, asyncCallbackInfo->info, promise);
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "QueryBundleStateInfos", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            BUNDLE_ACTIVE_LOGI("QueryBundleStateInfos, worker pool thread execute.");
            AsyncCallbackInfoAppUsage *asyncCallbackInfo = (AsyncCallbackInfoAppUsage *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->packageStats =
                    BundleStateCommon::GetPackageStats(asyncCallbackInfo->beginTime, asyncCallbackInfo->endTime);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryBundleStateInfos asyncCallbackInfo == nullptr");
            }
            BUNDLE_ACTIVE_LOGI("QueryBundleStateInfos worker pool thread execute end.");
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoAppUsage *asyncCallbackInfo = (AsyncCallbackInfoAppUsage *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                napi_create_object(env, &result);
                BundleStateCommon::GetBundleStateInfoForResult(env, asyncCallbackInfo->packageStats, result);
                BundleStateCommon::GetCallbackPromiseResult(env, asyncCallbackInfo->info, result);
                if (asyncCallbackInfo->info.callback != nullptr) {
                    napi_delete_reference(env, asyncCallbackInfo->info.callback);
                }
                napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
                delete asyncCallbackInfo;
                asyncCallbackInfo = nullptr;
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asyncCallbackInfo->asyncWork));
    if (asyncCallbackInfo->info.isCallback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        return promise;
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
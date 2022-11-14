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

#include <uv.h>
#include "securec.h"

#include "bundle_active_log.h"
#include "bundle_state_common.h"
#include "bundle_state_data.h"
#include "bundle_state_inner_errors.h"
#include "app_group_callback_info.h"

#include "bundle_active_app_group_napi.h"

namespace OHOS {
namespace DeviceUsageStats {
const uint32_t UN_REGISTER_GROUP_CALLBACK_MIN_PARAMS = 0;
const uint32_t UN_REGISTER_GROUP_CALLBACK_PARAMS = 1;
const uint32_t REGISTER_GROUP_CALLBACK_MIN_PARAMS = 1;
const uint32_t REGISTER_GROUP_CALLBACK_PARAMS = 2;
const uint32_t PRIORITY_GROUP_MIN_PARAMS = 0;
const uint32_t PRIORITY_GROUP_MIDDLE_PARAMS = 1;
const uint32_t PRIORITY_GROUP_PARAMS = 2;
const uint32_t APP_USAGE_MIN_PARAMS_BUNDLE_GROUP = 2;
const uint32_t APP_USAGE_PARAMS_BUNDLE_GROUP = 3;
const uint32_t SECOND_ARG = 2;
const std::vector<int32_t> GROUP_TYPE {10, 20, 30, 40, 50, 60};

static sptr<AppGroupObserver> registerObserver = nullptr;
std::mutex g_observerMutex_;

napi_value ParseQueryAppGroupParameters(const napi_env &env, const napi_callback_info &info,
    QueryAppGroupParamsInfo &params, AsyncQueryAppGroupCallbackInfo* &asyncCallbackInfo)
{
    size_t argc = PRIORITY_GROUP_PARAMS;
    napi_value argv[PRIORITY_GROUP_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != PRIORITY_GROUP_MIN_PARAMS) && (argc != PRIORITY_GROUP_MIDDLE_PARAMS) &&
        (argc != PRIORITY_GROUP_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }
    std::string result = "";
    params.bundleName = "";
    if (argc == PRIORITY_GROUP_MIDDLE_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        if (valuetype == napi_function) {
            napi_valuetype inputValueType = napi_undefined;
            NAPI_CALL(env, napi_typeof(env, argv[0], &inputValueType));
            if (inputValueType != napi_function) {
                params.errorCode = ERR_CALL_BACK_TYPE;
                return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
            }
            napi_create_reference(env, argv[0], 1, &params.callback);
        } else {
            params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
            if (params.bundleName.empty()) {
                BUNDLE_ACTIVE_LOGE("ParseQueryAppGroupParameters failed, bundleName is empty.");
                params.errorCode = ERR_PARAMETERS_EMPTY;
                return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_EMPTY, "bundleName");
            }
        }
    } else if (argc == PRIORITY_GROUP_PARAMS) {
        // argv[0] : bundleName
        params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
        if (params.bundleName.empty()) {
            BUNDLE_ACTIVE_LOGE("ParseQueryAppGroupParameters failed, bundleName is empty.");
            params.errorCode = ERR_PARAMETERS_EMPTY;
            return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_EMPTY, "bundleName");
        }
        // argv[1]: callback
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

napi_value QueryAppGroup(napi_env env, napi_callback_info info)
{
    BUNDLE_ACTIVE_LOGI("QueryAppGroup");
    QueryAppGroupParamsInfo params;
    AsyncQueryAppGroupCallbackInfo *asyncCallbackInfo = nullptr;
    ParseQueryAppGroupParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    std::unique_ptr<AsyncQueryAppGroupCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->bundleName = params.bundleName;
    BUNDLE_ACTIVE_LOGD("QueryAppGroup callbackPtr->bundleName: %{public}s",
        callbackPtr->bundleName.c_str());
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "QueryAppGroup", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncQueryAppGroupCallbackInfo *asyncCallbackInfo = (AsyncQueryAppGroupCallbackInfo *)data;
            if (asyncCallbackInfo) {
                asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance().QueryAppGroup(
                    asyncCallbackInfo->priorityGroup, asyncCallbackInfo->bundleName);
            } else {
                BUNDLE_ACTIVE_LOGE("QueryAppGroup, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncQueryAppGroupCallbackInfo *asyncCallbackInfo = (AsyncQueryAppGroupCallbackInfo *)data;
            if (asyncCallbackInfo) {
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

napi_value ParseSetAppGroupParameters(const napi_env &env, const napi_callback_info &info,
    ParamsBundleGroupInfo &params, AsyncCallbackInfoSetAppGroup* &asyncCallbackInfo)
{
    size_t argc = APP_USAGE_PARAMS_BUNDLE_GROUP;
    napi_value argv[APP_USAGE_PARAMS_BUNDLE_GROUP] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != APP_USAGE_MIN_PARAMS_BUNDLE_GROUP) && (argc != APP_USAGE_PARAMS_BUNDLE_GROUP)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }
    // argv[0] : bundleName
    std::string result = "";
    params.bundleName = BundleStateCommon::GetTypeStringValue(env, argv[0], result);
    if (params.bundleName.empty()) {
        BUNDLE_ACTIVE_LOGE("ParseSetAppGroupParameters failed, bundleName is empty.");
        params.errorCode = ERR_PARAMETERS_EMPTY;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_EMPTY, "bundleName");
    }
    napi_valuetype valuetype;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    if ((valuetype != napi_string) && (params.errorCode == ERR_OK)) {
        BUNDLE_ACTIVE_LOGE("Wrong argument type, string expected.");
        params.errorCode = ERR_BUNDLE_NAME_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_BUNDLE_NAME_TYPE, "");
    }
    // argv[1] : newGroup
    if ((params.errorCode == ERR_OK)
        && (BundleStateCommon::GetInt32NumberValue(env, argv[1], params.newGroup) == nullptr)) {
        BUNDLE_ACTIVE_LOGE("ParseSetAppGroupParameters failed, newGroup type is invalid.");
        params.errorCode = ERR_NEW_GROUP_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_NEW_GROUP_TYPE, "");
    }
    bool flag = false;
    if (params.errorCode == ERR_OK) {
        for (const auto& item : GROUP_TYPE) {
            if (item == params.newGroup) {
                flag = true;
                break;
            }
        }
    }
    if ((params.errorCode == ERR_OK) && !flag) {
        BUNDLE_ACTIVE_LOGE("ParseSetAppGroupParameters failed, newGroup value is invalid.");
        params.errorCode = ERR_NEW_GROUP_OUT_OF_RANGE;
        return BundleStateCommon::HandleParamErr(env, ERR_NEW_GROUP_OUT_OF_RANGE, "");
    }
    // argv[SECOND_ARG]: callback
    if (argc == APP_USAGE_PARAMS_BUNDLE_GROUP) {
        napi_valuetype inputValueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[SECOND_ARG], &inputValueType));
        if (inputValueType != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[SECOND_ARG], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value SetAppGroup(napi_env env, napi_callback_info info)
{
    ParamsBundleGroupInfo params;
    AsyncCallbackInfoSetAppGroup *asyncCallbackInfo = nullptr;
    ParseSetAppGroupParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    std::unique_ptr<AsyncCallbackInfoSetAppGroup> callbackPtr {asyncCallbackInfo};
    callbackPtr->newGroup = params.newGroup;
    callbackPtr->bundleName = params.bundleName;
    BUNDLE_ACTIVE_LOGD("SetAppGroup, bundleName is %{public}s, newGroup is %{public}d",
        callbackPtr->bundleName.c_str(), callbackPtr->newGroup);
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "SetAppGroup", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncCallbackInfoSetAppGroup *asyncCallbackInfo = (AsyncCallbackInfoSetAppGroup *)data;
            if (asyncCallbackInfo) {
                    asyncCallbackInfo->errorCode = BundleActiveClient::GetInstance().SetAppGroup(
                        asyncCallbackInfo->bundleName, asyncCallbackInfo->newGroup);
            } else {
                BUNDLE_ACTIVE_LOGE("SetAppGroup, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfoSetAppGroup *asyncCallbackInfo = (AsyncCallbackInfoSetAppGroup *)data;
            if (asyncCallbackInfo) {
                napi_value result = nullptr;
                napi_get_null(env, &result);
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

napi_value GetAppGroupChangeCallback(const napi_env &env, const napi_value &value)
{
    napi_ref result = nullptr;

    registerObserver = new (std::nothrow) AppGroupObserver();
    if (!registerObserver) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack callback is null");
        return nullptr;
    }
    napi_create_reference(env, value, 1, &result);
    registerObserver->SetCallbackInfo(env, result);

    return BundleStateCommon::NapiGetNull(env);
}

napi_value ParseRegisterAppGroupCallBackParameters(const napi_env &env, const napi_callback_info &info,
    RegisterCallbackInfo &params, AsyncRegisterCallbackInfo* &asyncCallbackInfo)
{
    size_t argc = REGISTER_GROUP_CALLBACK_PARAMS;
    napi_value argv[REGISTER_GROUP_CALLBACK_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != REGISTER_GROUP_CALLBACK_MIN_PARAMS) && (argc != REGISTER_GROUP_CALLBACK_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    // arg[0] : callback
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    std::lock_guard<std::mutex> lock(g_observerMutex_);
    if (registerObserver) {
        BUNDLE_ACTIVE_LOGI("RegisterAppGroupCallBack repeat!");
        params.errorCode = ERR_REPEAT_REGISTER_APP_GROUP_OBSERVER;
        return BundleStateCommon::HandleParamErr(env, ERR_REPEAT_REGISTER_APP_GROUP_OBSERVER, "");
    } else if (valuetype != napi_function || !GetAppGroupChangeCallback(env, argv[0])) {
        BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack bundleActiveGroupObserverInfo parse failed");
        params.errorCode = ERR_APP_GROUP_OBSERVER_CALLBACK_TYPE;
        return BundleStateCommon::HandleParamErr(env, ERR_APP_GROUP_OBSERVER_CALLBACK_TYPE, "");
    }

    // argv[1]: asyncCallback
    if (argc == REGISTER_GROUP_CALLBACK_PARAMS) {
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

napi_value RegisterAppGroupCallBack(napi_env env, napi_callback_info info)
{
    RegisterCallbackInfo params;
    AsyncRegisterCallbackInfo *asyncCallbackInfo = nullptr;
    ParseRegisterAppGroupCallBackParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        // return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
        return BundleStateCommon::NapiGetNull(env);
    }
    std::unique_ptr<AsyncRegisterCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->observer = registerObserver;
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "RegisterAppGroupCallBack", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncRegisterCallbackInfo *asyncCallbackInfo = (AsyncRegisterCallbackInfo *)data;
            if (asyncCallbackInfo) {
               asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().RegisterAppGroupCallBack(asyncCallbackInfo->observer);
            } else {
                BUNDLE_ACTIVE_LOGE("RegisterAppGroupCallBack, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncRegisterCallbackInfo *asyncCallbackInfo = (AsyncRegisterCallbackInfo *)data;
            if (asyncCallbackInfo) {
                if (asyncCallbackInfo->errorCode != ERR_OK) {
                    std::lock_guard<std::mutex> lock(g_observerMutex_);
                    registerObserver = nullptr;
                }
                napi_value result = nullptr;
                napi_get_null(env, &result);
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

napi_value ParseUnRegisterAppGroupCallBackParameters(const napi_env &env, const napi_callback_info &info,
    UnRegisterCallbackInfo &params, AsyncUnRegisterCallbackInfo* &asyncCallbackInfo)
{
    size_t argc = UN_REGISTER_GROUP_CALLBACK_PARAMS;
    napi_value argv[UN_REGISTER_GROUP_CALLBACK_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    if ((argc != UN_REGISTER_GROUP_CALLBACK_MIN_PARAMS) && (argc != UN_REGISTER_GROUP_CALLBACK_PARAMS)) {
        params.errorCode = ERR_PARAMETERS_NUMBER;
        return BundleStateCommon::HandleParamErr(env, ERR_PARAMETERS_NUMBER, "");
    }

    if (argc == UN_REGISTER_GROUP_CALLBACK_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        if (valuetype != napi_function) {
            params.errorCode = ERR_CALL_BACK_TYPE;
            return BundleStateCommon::HandleParamErr(env, ERR_CALL_BACK_TYPE, "");
        }
        napi_create_reference(env, argv[0], 1, &params.callback);
    }
    std::lock_guard<std::mutex> lock(g_observerMutex_);
    if (!registerObserver) {
        BUNDLE_ACTIVE_LOGI("UnRegisterAppGroupCallBack observer is not exist");
        params.errorCode = ERR_APP_GROUP_OBSERVER_IS_NULLPTR;
        return BundleStateCommon::HandleParamErr(env, ERR_APP_GROUP_OBSERVER_IS_NULLPTR, "");
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value UnRegisterAppGroupCallBack(napi_env env, napi_callback_info info)
{
    UnRegisterCallbackInfo params;
    AsyncUnRegisterCallbackInfo *asyncCallbackInfo = nullptr;
    ParseUnRegisterAppGroupCallBackParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::NapiGetNull(env);
    }
    std::unique_ptr<AsyncUnRegisterCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->observer = registerObserver;
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "UnRegisterAppGroupCallBack", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncUnRegisterCallbackInfo *asyncCallbackInfo = (AsyncUnRegisterCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(asyncCallbackInfo->observer);
            } else {
                BUNDLE_ACTIVE_LOGE("UnRegisterAppGroupCallBack, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncUnRegisterCallbackInfo *asyncCallbackInfo = (AsyncUnRegisterCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                if (asyncCallbackInfo->errorCode == ERR_OK) {
                    std::lock_guard<std::mutex> lock(g_observerMutex_);
                    registerObserver = nullptr;
                }
                napi_value result = nullptr;
                napi_get_null(env, &result);
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
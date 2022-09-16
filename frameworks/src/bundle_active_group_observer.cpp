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
#include "bundle_active_group_callback_info.h"
#include "app_group_observer_napi.h"

namespace OHOS {
namespace DeviceUsageStats {
const uint32_t UN_REGISTER_GROUP_CALLBACK_MIN_PARAMS = 0;
const uint32_t UN_REGISTER_GROUP_CALLBACK_PARAMS = 1;
const uint32_t REGISTER_GROUP_CALLBACK_MIN_PARAMS = 1;
const uint32_t REGISTER_GROUP_CALLBACK_PARAMS = 2;

static sptr<AppGroupObserver> registerObserver = nullptr;
static std::mutex g_observerMutex_;

napi_value GetBundleGroupChangeCallback(const napi_env &env, const napi_value &value)
{
    napi_ref result = nullptr;
    
    registerObserver = new (std::nothrow) BundleActiveGroupObserver();
    if (!registerObserver) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack callback is null");
        return nullptr;
    }
    napi_create_reference(env, value, 1, &result);
    registerObserver->SetCallbackInfo(env, result);

    return BundleStateCommon::NapiGetNull(env);
}

napi_value ParseRegisterGroupCallBackParameters(const napi_env &env, const napi_callback_info &info,
    RegisterCallbackInfo &params, AsyncRegisterCallbackInfo* &asyncCallbackInfo)
{
    size_t argc = REGISTER_GROUP_CALLBACK_PARAMS;
    napi_value argv[REGISTER_GROUP_CALLBACK_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == REGISTER_GROUP_CALLBACK_MIN_PARAMS || argc == REGISTER_GROUP_CALLBACK_PARAMS,
        "Invalid number of parameters");
        
    // arg[0] : callback
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    std::lock_guard<std::mutex> lock(g_observerMutex_);
    if (registerObserver) {
        BUNDLE_ACTIVE_LOGI("RegisterGroupCallBack repeat!");
        params.errorCode = ERR_REPEAT_OPERATION;
    } else if (valuetype != napi_function || !GetBundleGroupChangeCallback(env, argv[0])) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack bundleActiveGroupObserverInfo parse failed");
        params.errorCode = ERR_OBSERVER_CALLBACK_IS_INVALID;
    }

    // argv[1]: asyncCallback
    if (argc == REGISTER_GROUP_CALLBACK_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseStatesParameters invalid parameter type. Function expected");
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value RegisterGroupCallBack(napi_env env, napi_callback_info info)
{
    RegisterCallbackInfo params;
    AsyncRegisterCallbackInfo *asyncCallbackInfo = nullptr;
    ParseRegisterGroupCallBackParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK && !asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncRegisterCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->observer = registerObserver;
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "RegisterGroupCallBack", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncRegisterCallbackInfo *asyncCallbackInfo = (AsyncRegisterCallbackInfo *)data;
            if (asyncCallbackInfo) {
               asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().RegisterAppGroupCallBack(asyncCallbackInfo->observer);
            } else {
                BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncRegisterCallbackInfo *asyncCallbackInfo = (AsyncRegisterCallbackInfo *)data;
            if (asyncCallbackInfo) {
                std::lock_guard<std::mutex> lock(g_observerMutex_);
                if (asyncCallbackInfo->errorCode != ERR_OK) {
                    registerObserver = nullptr;
                }
                napi_value result = nullptr;
                napi_get_null(env, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        (void *)asyncCallbackInfo,
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

napi_value ParseUnRegisterGroupCallBackParameters(const napi_env &env, const napi_callback_info &info,
    UnRegisterCallbackInfo &params, AsyncUnRegisterCallbackInfo* &asyncCallbackInfo)
{
    size_t argc = UN_REGISTER_GROUP_CALLBACK_PARAMS;
    napi_value argv[UN_REGISTER_GROUP_CALLBACK_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == UN_REGISTER_GROUP_CALLBACK_MIN_PARAMS || argc == UN_REGISTER_GROUP_CALLBACK_PARAMS,
        "Invalid number of parameters");

    // argv[1]: callback
    if (argc == UN_REGISTER_GROUP_CALLBACK_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseStatesParameters invalid parameter type. "
            "Function expected.");
        napi_create_reference(env, argv[0], 1, &params.callback);
    }
    std::lock_guard<std::mutex> lock(g_observerMutex_);
    if (!registerObserver) {
        BUNDLE_ACTIVE_LOGI("UnRegisterGroupCallBack observer is not exist");
        params.errorCode = ERR_REGISTER_OBSERVER_IS_NULL;
        return nullptr;
    }
    BundleStateCommon::AsyncInit(env, params, asyncCallbackInfo);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value UnRegisterGroupCallBack(napi_env env, napi_callback_info info)
{
    UnRegisterCallbackInfo params;
    AsyncUnRegisterCallbackInfo *asyncCallbackInfo = nullptr;
    ParseUnRegisterGroupCallBackParameters(env, info, params, asyncCallbackInfo);
    if (params.errorCode != ERR_OK && !asyncCallbackInfo) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncUnRegisterCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->observer = registerObserver;
    napi_value promise = nullptr;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "UnRegisterGroupCallBack", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncUnRegisterCallbackInfo *asyncCallbackInfo = (AsyncUnRegisterCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().UnRegisterAppGroupCallBack(asyncCallbackInfo->observer);
            } else {
                BUNDLE_ACTIVE_LOGE("UnRegisterGroupCallBack, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncUnRegisterCallbackInfo *asyncCallbackInfo = (AsyncUnRegisterCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                std::lock_guard<std::mutex> lock(g_observerMutex_);
                if (asyncCallbackInfo->errorCode == ERR_OK) {
                    registerObserver = nullptr;
                }
                napi_value result = nullptr;
                napi_get_null(env, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        (void *)asyncCallbackInfo,
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
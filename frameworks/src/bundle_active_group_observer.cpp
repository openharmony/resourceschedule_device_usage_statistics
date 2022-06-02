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

#include "bundle_active_group_observer.h"

namespace OHOS {
namespace DeviceUsageStats {
const uint32_t UN_REGISTER_GROUP_CALLBACK_MIN_PARAMS = 0;
const uint32_t UN_REGISTER_GROUP_CALLBACK_PARAMS = 1;
const uint32_t REGISTER_GROUP_CALLBACK_MIN_PARAMS = 1;
const uint32_t REGISTER_GROUP_CALLBACK_PARAMS = 2;

static sptr<BundleActiveGroupObserver> registerObserver = nullptr;

BundleActiveGroupObserver::~BundleActiveGroupObserver()
{
    if (bundleGroupCallbackInfo_.ref) {
        napi_delete_reference(bundleGroupCallbackInfo_.env, bundleGroupCallbackInfo_.ref);
    }
}

void BundleActiveGroupObserver::SetCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    bundleGroupCallbackInfo_.env = env;
    bundleGroupCallbackInfo_.ref = ref;
}

napi_value SetBundleGroupChangedData(const CallbackReceiveDataWorker *commonEventDataWorkerData, napi_value &result)
{
    BUNDLE_ACTIVE_LOGI("enter");

    if (!commonEventDataWorkerData) {
        BUNDLE_ACTIVE_LOGE("commonEventDataWorkerData is null");
        return nullptr;
    }
    napi_value value = nullptr;

    // oldGroup
    napi_create_int32(commonEventDataWorkerData->env, commonEventDataWorkerData->oldGroup, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "oldGroup", value);

    // newGroup
    napi_create_int32(commonEventDataWorkerData->env, commonEventDataWorkerData->newGroup, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "newGroup", value);

    // userId
    napi_create_int32(commonEventDataWorkerData->env, commonEventDataWorkerData->userId, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "userId", value);

    // changeReason
    napi_create_uint32(commonEventDataWorkerData->env, commonEventDataWorkerData->changeReason, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "changeReason", value);
    // bundleName
    napi_create_string_utf8(
        commonEventDataWorkerData->env, commonEventDataWorkerData->bundleName.c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "bundleName", value);
    BUNDLE_ACTIVE_LOGI(
        "RegisterGroupCallBack oldGroup=%{public}d, newGroup=%{public}d, userId=%{public}d, "
        "changeReason=%{public}d, bundleName=%{public}s",
        commonEventDataWorkerData->oldGroup, commonEventDataWorkerData->newGroup, commonEventDataWorkerData->userId,
        commonEventDataWorkerData->changeReason, commonEventDataWorkerData->bundleName.c_str());

    return BundleStateCommon::NapiGetNull(commonEventDataWorkerData->env);
}

void UvQueueWorkOnBundleGroupChanged(uv_work_t *work, int status)
{
    BUNDLE_ACTIVE_LOGI("OnBundleGroupChanged uv_work_t start");
    if (!work) {
        return;
    }
    CallbackReceiveDataWorker *callbackReceiveDataWorkerData = (CallbackReceiveDataWorker *)work->data;
    if (!callbackReceiveDataWorkerData || !callbackReceiveDataWorkerData->ref) {
        BUNDLE_ACTIVE_LOGE("OnBundleGroupChanged commonEventDataWorkerData or ref is null");
        delete work;
        work = nullptr;
        return;
    }

    napi_value result = nullptr;
    napi_create_object(callbackReceiveDataWorkerData->env, &result);
    if (!SetBundleGroupChangedData(callbackReceiveDataWorkerData, result)) {
        delete work;
        work = nullptr;
        delete callbackReceiveDataWorkerData;
        callbackReceiveDataWorkerData = nullptr;
        return;
    }

    napi_value undefined = nullptr;
    napi_get_undefined(callbackReceiveDataWorkerData->env, &undefined);

    napi_value callback = nullptr;
    napi_value resultout = nullptr;
    napi_get_reference_value(callbackReceiveDataWorkerData->env, callbackReceiveDataWorkerData->ref, &callback);

    napi_value results[ARGS_TWO] = {nullptr};
    results[PARAM_FIRST] = BundleStateCommon::GetErrorValue(callbackReceiveDataWorkerData->env, NO_ERROR);
    results[PARAM_SECOND] = result;
    NAPI_CALL_RETURN_VOID(callbackReceiveDataWorkerData->env, napi_call_function(callbackReceiveDataWorkerData->env,
        undefined, callback, ARGS_TWO, &results[PARAM_FIRST], &resultout));
    delete callbackReceiveDataWorkerData;
    callbackReceiveDataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

/*
* observer callback when group change
*/
void BundleActiveGroupObserver::OnBundleGroupChanged(const BundleActiveGroupCallbackInfo &bundleActiveGroupCallbackInfo)
{
    BUNDLE_ACTIVE_LOGI("OnBundleGroupChanged start");

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(bundleGroupCallbackInfo_.env, &loop);
    if (!loop) {
        BUNDLE_ACTIVE_LOGE("loop instance is nullptr");
        return;
    }

    uv_work_t* work = new (std::nothrow) uv_work_t;
    if (!work) {
        BUNDLE_ACTIVE_LOGE("work is null");
        return;
    }
    CallbackReceiveDataWorker* callbackReceiveDataWorker = new (std::nothrow) CallbackReceiveDataWorker();
    if (!callbackReceiveDataWorker) {
        BUNDLE_ACTIVE_LOGE("callbackReceiveDataWorker is null");
        delete work;
        work = nullptr;
        return;
    }
    MessageParcel data;
    if (!bundleActiveGroupCallbackInfo.Marshalling(data)) {
        BUNDLE_ACTIVE_LOGE("Marshalling fail");
    }
    BundleActiveGroupCallbackInfo* callBackInfo = bundleActiveGroupCallbackInfo.Unmarshalling(data);
    callbackReceiveDataWorker->oldGroup     = callBackInfo->GetOldGroup();
    callbackReceiveDataWorker->newGroup     = callBackInfo->GetNewGroup();
    callbackReceiveDataWorker->changeReason = callBackInfo->GetChangeReason();
    callbackReceiveDataWorker->userId       = callBackInfo->GetUserId();
    callbackReceiveDataWorker->bundleName   = callBackInfo->GetBundleName();

    callbackReceiveDataWorker->env = bundleGroupCallbackInfo_.env;
    callbackReceiveDataWorker->ref = bundleGroupCallbackInfo_.ref;

    work->data = (void *)callbackReceiveDataWorker;

    BUNDLE_ACTIVE_LOGI("OnReceiveEvent this = %{public}p", this);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnBundleGroupChanged);
    if (ret != 0) {
        delete callbackReceiveDataWorker;
        callbackReceiveDataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

napi_value GetBundleGroupChangeCallback(
    const napi_env &env, const napi_value &value, BundleActiveGroupObserverInfo &bundleActiveGroupObserverInfo)
{
    napi_ref result = nullptr;
    
    bundleActiveGroupObserverInfo.callback = new (std::nothrow) BundleActiveGroupObserver();
    if (!bundleActiveGroupObserverInfo.callback) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack callback is null");
        return BundleStateCommon::NapiGetNull(env);
    }

    napi_create_reference(env, value, 1, &result);
    bundleActiveGroupObserverInfo.callback->SetCallbackInfo(env, result);

    return BundleStateCommon::NapiGetNull(env);
}

napi_value ParseRegisterGroupCallBackParameters(const napi_env &env, const napi_callback_info &info,
    RegisterCallbackInfo &params, sptr<BundleActiveGroupObserver> &observer)
{
    BUNDLE_ACTIVE_LOGI("enter ParseRegisterGroupCallBackParameters");
    size_t argc = REGISTER_GROUP_CALLBACK_PARAMS;
    napi_value argv[REGISTER_GROUP_CALLBACK_PARAMS] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, NULL, NULL));
    NAPI_ASSERT(env, argc == REGISTER_GROUP_CALLBACK_MIN_PARAMS || argc == REGISTER_GROUP_CALLBACK_PARAMS,
        "Invalid number of parameters");
        
    // arg[0] : callback
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valuetype));
    BundleActiveGroupObserverInfo bundleActiveGroupObserverInfo;
    if (valuetype != napi_function || !GetBundleGroupChangeCallback(env, argv[0], bundleActiveGroupObserverInfo)) {
        BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack bundleActiveGroupObserverInfo parse failed");
        params.errorCode = ERR_OBSERVER_CALLBACK_IS_INVALID;
    } else {
        observer = bundleActiveGroupObserverInfo.callback;
    }

    // argv[1]: asyncCallback
    if (argc == REGISTER_GROUP_CALLBACK_PARAMS) {
        napi_valuetype valuetype = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[1], &valuetype));
        NAPI_ASSERT(env, valuetype == napi_function, "ParseStatesParameters invalid parameter type. Function expected");
        napi_create_reference(env, argv[1], 1, &params.callback);
    }
    return BundleStateCommon::NapiGetNull(env);
}

napi_value RegisterGroupCallBack(napi_env env, napi_callback_info info)
{
    BUNDLE_ACTIVE_LOGI("enter RegisterGroupCallBack");
    RegisterCallbackInfo params;
    ParseRegisterGroupCallBackParameters(env, info, params, registerObserver);

    if (params.errorCode != ERR_OK || !registerObserver) {
        if (registerObserver) {
            delete registerObserver;
            registerObserver = nullptr;
        }
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncRegisterCallbackInfo *asyncCallbackInfo = new (std::nothrow) AsyncRegisterCallbackInfo(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo))
        != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncRegisterCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->observer = registerObserver;
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
                    BundleActiveClient::GetInstance().RegisterGroupCallBack(asyncCallbackInfo->observer);
            } else {
                BUNDLE_ACTIVE_LOGE("RegisterGroupCallBack, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncRegisterCallbackInfo *asyncCallbackInfo = (AsyncRegisterCallbackInfo *)data;
            if (asyncCallbackInfo) {
                napi_value result = nullptr;
                asyncCallbackInfo->state = (asyncCallbackInfo->errorCode == ERR_OK) ? true : false;
                napi_get_boolean(env, asyncCallbackInfo->state, &result);
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
    UnRegisterCallbackInfo &params)
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
    return BundleStateCommon::NapiGetNull(env);
}

napi_value UnRegisterGroupCallBack(napi_env env, napi_callback_info info)
{
    BUNDLE_ACTIVE_LOGI("enter UnRegisterGroupCallBack");

    UnRegisterCallbackInfo params;
    ParseUnRegisterGroupCallBackParameters(env, info, params);
    if (params.errorCode != ERR_OK) {
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (!registerObserver) {
        BUNDLE_ACTIVE_LOGI("UnRegisterGroupCallBack observer is not");
        params.errorCode = ERR_REGISTER_OBSERVER_IS_NULL;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    napi_value promise = nullptr;
    AsyncUnRegisterCallbackInfo *asyncCallbackInfo =
        new (std::nothrow) AsyncUnRegisterCallbackInfo(env);
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo))
        != EOK) {
        params.errorCode = ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED;
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return BundleStateCommon::JSParaError(env, params.callback, params.errorCode);
    }
    std::unique_ptr<AsyncUnRegisterCallbackInfo> callbackPtr {asyncCallbackInfo};
    callbackPtr->observer = registerObserver;
    BundleStateCommon::SettingAsyncWorkData(env, params.callback, *asyncCallbackInfo, promise);
    BUNDLE_ACTIVE_LOGI("UnRegisterGroupCallBack will Async");
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_latin1(env, "UnRegisterGroupCallBack", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncUnRegisterCallbackInfo *asyncCallbackInfo = (AsyncUnRegisterCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                asyncCallbackInfo->errorCode =
                    BundleActiveClient::GetInstance().UnregisterGroupCallBack(asyncCallbackInfo->observer);
            } else {
                BUNDLE_ACTIVE_LOGE("UnRegisterGroupCallBack, asyncCallbackInfo == nullptr");
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncUnRegisterCallbackInfo *asyncCallbackInfo = (AsyncUnRegisterCallbackInfo *)data;
            if (asyncCallbackInfo != nullptr) {
                napi_value result = nullptr;
                asyncCallbackInfo->state = (asyncCallbackInfo->errorCode == ERR_OK) ? true : false;
                napi_get_boolean(env, asyncCallbackInfo->state, &result);
                BundleStateCommon::GetCallbackPromiseResult(env, *asyncCallbackInfo, result);
            }
        },
        (void *)asyncCallbackInfo,
        &asyncCallbackInfo->asyncWork));
    NAPI_CALL(env, napi_queue_async_work(env, callbackPtr->asyncWork));
    registerObserver = nullptr;
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
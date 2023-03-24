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

#include "app_group_observer_napi.h"

namespace OHOS {
namespace DeviceUsageStats {
AppGroupObserver::~AppGroupObserver()
{
    if (bundleGroupCallbackInfo_.ref) {
        napi_delete_reference(bundleGroupCallbackInfo_.env, bundleGroupCallbackInfo_.ref);
    }
}

void AppGroupObserver::SetCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    bundleGroupCallbackInfo_.env = env;
    bundleGroupCallbackInfo_.ref = ref;
}

napi_value SetBundleGroupChangedData(const CallbackReceiveDataWorker *commonEventDataWorkerData, napi_value &result)
{
    if (!commonEventDataWorkerData) {
        BUNDLE_ACTIVE_LOGE("commonEventDataWorkerData is null");
        return nullptr;
    }
    napi_value value = nullptr;

    // oldGroup
    napi_create_int32(commonEventDataWorkerData->env, commonEventDataWorkerData->oldGroup, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "appOldGroup", value);

    // newGroup
    napi_create_int32(commonEventDataWorkerData->env, commonEventDataWorkerData->newGroup, &value);
    napi_set_named_property(commonEventDataWorkerData->env, result, "appNewGroup", value);

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
    BUNDLE_ACTIVE_LOGD(
        "RegisterGroupCallBack appOldGroup = %{public}d, appNewGroup = %{public}d, userId=%{public}d, "
        "changeReason=%{public}d, bundleName=%{public}s",
        commonEventDataWorkerData->oldGroup, commonEventDataWorkerData->newGroup, commonEventDataWorkerData->userId,
        commonEventDataWorkerData->changeReason, commonEventDataWorkerData->bundleName.c_str());

    return BundleStateCommon::NapiGetNull(commonEventDataWorkerData->env);
}

void UvQueueWorkOnAppGroupChanged(uv_work_t *work, int status)
{
    BUNDLE_ACTIVE_LOGD("OnAppGroupChanged uv_work_t start");
    if (!work) {
        return;
    }
    CallbackReceiveDataWorker *callbackReceiveDataWorkerData = (CallbackReceiveDataWorker *)work->data;
    if (!callbackReceiveDataWorkerData || !callbackReceiveDataWorkerData->ref) {
        BUNDLE_ACTIVE_LOGE("OnAppGroupChanged commonEventDataWorkerData or ref is null");
        delete work;
        work = nullptr;
        return;
    }
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(callbackReceiveDataWorkerData->env, &scope);
    if (scope == nullptr) {
        return;
    }

    napi_value result = nullptr;
    napi_create_object(callbackReceiveDataWorkerData->env, &result);
    if (!SetBundleGroupChangedData(callbackReceiveDataWorkerData, result)) {
        delete work;
        work = nullptr;
        napi_close_handle_scope(callbackReceiveDataWorkerData->env, scope);
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

    napi_close_handle_scope(callbackReceiveDataWorkerData->env, scope);
    delete callbackReceiveDataWorkerData;
    callbackReceiveDataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

/*
* observer callback when group change
*/
void AppGroupObserver::OnAppGroupChanged(const AppGroupCallbackInfo &appGroupCallbackInfo)
{
    BUNDLE_ACTIVE_LOGD("OnAppGroupChanged start");
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
    if (!appGroupCallbackInfo.Marshalling(data)) {
        BUNDLE_ACTIVE_LOGE("Marshalling fail");
    }
    AppGroupCallbackInfo* callBackInfo = appGroupCallbackInfo.Unmarshalling(data);
    callbackReceiveDataWorker->oldGroup     = callBackInfo->GetOldGroup();
    callbackReceiveDataWorker->newGroup     = callBackInfo->GetNewGroup();
    callbackReceiveDataWorker->changeReason = callBackInfo->GetChangeReason();
    callbackReceiveDataWorker->userId       = callBackInfo->GetUserId();
    callbackReceiveDataWorker->bundleName   = callBackInfo->GetBundleName();
    callbackReceiveDataWorker->env = bundleGroupCallbackInfo_.env;
    callbackReceiveDataWorker->ref = bundleGroupCallbackInfo_.ref;
    delete callBackInfo;

    work->data = static_cast<void*>(callbackReceiveDataWorker);
    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnAppGroupChanged);
    if (ret != 0) {
        delete callbackReceiveDataWorker;
        callbackReceiveDataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
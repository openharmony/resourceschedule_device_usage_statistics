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

#include "securec.h"

#include "bundle_active_log.h"
#include "bundle_state_data.h"
#include "bundle_state_common.h"

namespace OHOS {
namespace DeviceUsageStats {
napi_value BundleStateCommon::NapiGetNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

void BundleStateCommon::GetCallbackPromiseResult(const napi_env &env,
    const CallbackPromiseInfo &info, const napi_value &result)
{
    if (info.isCallback) {
        SetCallbackInfo(env, info.callback, info.errorCode, result);
    } else {
        SetPromiseInfo(env, info.deferred, result);
    }
}

void BundleStateCommon::SetCallbackInfo(
    const napi_env &env, const napi_ref &callbackIn, const int &errorCode, const napi_value &result)
{
    napi_value undefined = nullptr;
    napi_get_undefined(env, &undefined);

    napi_value callback = nullptr;
    napi_value resultout = nullptr;
    napi_get_reference_value(env, callbackIn, &callback);
    napi_value results[ARGS_TWO] = {nullptr};
    results[PARAM_FIRST] = GetCallbackErrorValue(env, errorCode);
    results[PARAM_SECOND] = result;
    NAPI_CALL_RETURN_VOID(env, napi_call_function(env, undefined, callback, ARGS_TWO, &results[PARAM_FIRST],
        &resultout));
}

void BundleStateCommon::GetBundleActiveEventForResult(
    napi_env env, const std::vector<BundleActiveEvent> &bundleActiveStates, napi_value result)
{
    int32_t index = 0;
    for (const auto &item : bundleActiveStates) {
        napi_value bundleActiveState = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &bundleActiveState));

        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.bundleName_.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "bundleName", bundleName));

        napi_value stateType = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.eventId_, &stateType));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "stateType", stateType));

        napi_value stateOccurredTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.timeStamp_, &stateOccurredTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "stateOccurredTime",
            stateOccurredTime));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, bundleActiveState));
        index++;
    }
}

void BundleStateCommon::GetBundleStateInfoByIntervalForResult(
    napi_env env, const std::vector<BundleActivePackageStats> &packageStats, napi_value result)
{
    int32_t index = 0;
    for (const auto &item : packageStats) {
        napi_value packageObject = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &packageObject));

        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.bundleName_.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "bundleName", bundleName));

        napi_value abilityPrevAccessTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.lastTimeUsed_, &abilityPrevAccessTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "abilityPrevAccessTime",
            abilityPrevAccessTime));

        napi_value abilityInFgTotalTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.totalInFrontTime_, &abilityInFgTotalTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "abilityInFgTotalTime",
            abilityInFgTotalTime));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, packageObject));
        index++;
    }
}

void BundleStateCommon::GetBundleStateInfoForResult(
    napi_env env, const std::vector<BundleActivePackageStats> &packageStats, napi_value result)
{
    for (const auto &item : packageStats) {
        napi_value packageObject = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &packageObject));
        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.bundleName_.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "bundleName", bundleName));

        napi_value abilityPrevAccessTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.lastTimeUsed_, &abilityPrevAccessTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "abilityPrevAccessTime",
            abilityPrevAccessTime));

        napi_value abilityInFgTotalTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.totalInFrontTime_, &abilityInFgTotalTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "abilityInFgTotalTime",
            abilityInFgTotalTime));

        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, item.bundleName_.c_str(), packageObject));
    }
}

void BundleStateCommon::SetPromiseInfo(const napi_env &env, const napi_deferred &deferred, const napi_value &result)
{
    napi_resolve_deferred(env, deferred, result);
}

napi_value BundleStateCommon::GetCallbackErrorValue(napi_env env, int errCode)
{
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "data", eCode));
    return result;
}

napi_value BundleStateCommon::JSParaError(const napi_env &env, const napi_ref &callback)
{
    if (callback) {
        return BundleStateCommon::NapiGetNull(env);
    } else {
        napi_value promise = nullptr;
        napi_deferred deferred = nullptr;
        napi_create_promise(env, &deferred, &promise);
        napi_resolve_deferred(env, deferred, BundleStateCommon::NapiGetNull(env));
        return promise;
    }
}

std::string BundleStateCommon::GetTypeStringValue(napi_env env, napi_value param, const std::string &result)
{
    BUNDLE_ACTIVE_LOGI("GetTypeStringValue start");
    size_t size = 0;
    if (napi_get_value_string_utf8(env, param, nullptr, 0, &size) != BUNDLE_STATE_OK) {
        return result;
    }

    std::string value("");
    if (size == 0) {
        return result;
    }

    char *buf = new (std::nothrow) char[size + 1];
    if (buf == nullptr) {
        return value;
    }

    if (memset_s(buf, size + 1, 0, size + 1) != EOK) {
        return value;
    }

    bool rev = napi_get_value_string_utf8(env, param, buf, size + 1, &size) == BUNDLE_STATE_OK;
    if (rev) {
        value = buf;
    } else {
        value = result;
    }

    delete[] buf;
    buf = nullptr;
    BUNDLE_ACTIVE_LOGI("string result: %{public}s", value.c_str());
    return value;
}

napi_value BundleStateCommon::GetInt64NumberValue(const napi_env &env, const napi_value &value, int64_t &result)
{
    BUNDLE_ACTIVE_LOGI("GetInt64NumberValue start");
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number or function expected.");
    napi_get_value_int64(env, value, &result);
    BUNDLE_ACTIVE_LOGI("number result: %{public}lld", result);

    return BundleStateCommon::NapiGetNull(env);
}

napi_value BundleStateCommon::GetInt32NumberValue(const napi_env &env, const napi_value &value, int32_t &result)
{
    BUNDLE_ACTIVE_LOGI("GetInt32NumberValue start");
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    NAPI_ASSERT(env, valuetype == napi_number, "Wrong argument type. Number or function expected.");
    napi_get_value_int32(env, value, &result);
    BUNDLE_ACTIVE_LOGI("number result: %{public}d", result);

    return BundleStateCommon::NapiGetNull(env);
}

void BundleStateCommon::SettingCallbackPromiseInfo(
    const napi_env &env, const napi_ref &callback, CallbackPromiseInfo &info, napi_value &promise)
{
    if (callback) {
        info.callback = callback;
        info.isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        info.deferred = deferred;
        info.isCallback = false;
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
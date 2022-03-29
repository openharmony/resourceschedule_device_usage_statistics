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
        SetPromiseInfo(env, info.deferred, result, info.errorCode);
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
    results[PARAM_FIRST] = GetErrorValue(env, errorCode);
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

void BundleStateCommon::GetBundleStateInfoForResult(napi_env env,
    const std::shared_ptr<std::map<std::string, BundleActivePackageStats>> &packageStats, napi_value result)
{
    if (packageStats == nullptr) {
        BUNDLE_ACTIVE_LOGE("PackageStats is invalid");
        return;
    }
    for (const auto &item : *packageStats) {
        napi_value packageObject = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &packageObject));
        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.second.bundleName_.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "bundleName", bundleName));

        napi_value abilityPrevAccessTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.second.lastTimeUsed_, &abilityPrevAccessTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "abilityPrevAccessTime",
            abilityPrevAccessTime));

        napi_value abilityInFgTotalTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.second.totalInFrontTime_, &abilityInFgTotalTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, packageObject, "abilityInFgTotalTime",
            abilityInFgTotalTime));

        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, result, item.first.c_str(), packageObject));
    }
}

void BundleStateCommon::GetModuleRecordBasicForResult(napi_env env,
    const BundleActiveModuleRecord &oneModuleRecord, napi_value &moduleObject)
{
        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, oneModuleRecord.bundleName_.c_str(),
            NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "bundleName", bundleName));
        napi_value appLabelId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, oneModuleRecord.appLabelId_, &appLabelId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "appLabelId", appLabelId));
        napi_value moduleName = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, oneModuleRecord.moduleName_.c_str(), NAPI_AUTO_LENGTH,
            &moduleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "name", moduleName));
        napi_value labelId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, oneModuleRecord.labelId_, &labelId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "labelId", labelId));
        napi_value descriptionId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, oneModuleRecord.descriptionId_, &descriptionId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "descriptionId", descriptionId));
        napi_value abilityName = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, oneModuleRecord.abilityName_.c_str(), NAPI_AUTO_LENGTH,
            &abilityName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityName", abilityName));
        napi_value abilityLableId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, oneModuleRecord.abilityLableId_, &abilityLableId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityLableId", abilityLableId));
        napi_value abilityDescriptionId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, oneModuleRecord.abilityDescriptionId_,
            &abilityDescriptionId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityDescriptionId",
            abilityDescriptionId));
        napi_value abilityIconId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, oneModuleRecord.abilityIconId_, &abilityIconId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityIconId", abilityIconId));
        napi_value launchedCount = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, oneModuleRecord.launchedCount_, &launchedCount));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "launchedCount", launchedCount));
        napi_value lastModuleUsedTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, oneModuleRecord.lastModuleUsedTime_, &lastModuleUsedTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "lastLaunchTime", lastModuleUsedTime));
        napi_value removed = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, oneModuleRecord.removed_, &removed));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "isRemoved", removed));
}

void BundleStateCommon::GetModuleRecordForResult(napi_env env,
    const std::vector<BundleActiveModuleRecord> &moduleRecords, napi_value result)
{
    int32_t index = 0;
    for (const auto &item : moduleRecords) {
        napi_value moduleObject = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &moduleObject));
        GetModuleRecordBasicForResult(env, item, moduleObject);

        napi_value formRecords = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &formRecords));
        int32_t formIdx = 0;
        for (const auto& oneFormRecord : item.formRecords_) {
            napi_value formObject = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &formObject));
            napi_value formName = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, oneFormRecord.formName_.c_str(), NAPI_AUTO_LENGTH,
                &formName));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObject, "formName", formName));

            napi_value formDimension = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, oneFormRecord.formDimension_, &formDimension));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObject, "formDimension", formDimension));

            napi_value formId = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, oneFormRecord.formId_, &formId));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObject, "formId", formId));

            napi_value formLastUsedTime = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, oneFormRecord.formLastUsedTime_, &formLastUsedTime));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObject, "formLastUsedTime", formLastUsedTime));

            napi_value count = nullptr;
            NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, oneFormRecord.count_, &count));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObject, "formTouchedCount", count));
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, formRecords, formIdx, formObject));
            formIdx++;
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "formRecords", formRecords));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, moduleObject));
        index++;
    }
}

void BundleStateCommon::SetPromiseInfo(const napi_env &env, const napi_deferred &deferred,
    const napi_value &result, const int &errorCode)
{
    if (errorCode == ERR_OK) {
        napi_resolve_deferred(env, deferred, result);
    } else {
        napi_reject_deferred(env, deferred, GetErrorValue(env, errorCode));
    }
}

napi_value BundleStateCommon::GetErrorValue(napi_env env, int errCode)
{
    if (errCode == ERR_OK) {
        return NapiGetNull(env);
    }
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    NAPI_CALL(env, napi_create_int32(env, errCode, &eCode));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    return result;
}

napi_value BundleStateCommon::JSParaError(const napi_env &env, const napi_ref &callback, const int &errorCode)
{
    if (callback) {
        napi_value result = nullptr;
        napi_create_array(env, &result);
        SetCallbackInfo(env, callback, errorCode, result);
        return result;
    } else {
        napi_value promise = nullptr;
        napi_deferred deferred = nullptr;
        napi_create_promise(env, &deferred, &promise);
        napi_reject_deferred(env, deferred, GetErrorValue(env, errorCode));
        return promise;
    }
}

std::string BundleStateCommon::GetTypeStringValue(napi_env env, napi_value param, const std::string &result)
{
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
        delete[] buf;
        buf = nullptr;
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
    return value;
}

napi_value BundleStateCommon::GetInt64NumberValue(const napi_env &env, const napi_value &value, int64_t &result)
{
    napi_valuetype valuetype = napi_undefined;

    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_number) {
        BUNDLE_ACTIVE_LOGE("Wrong argument type, number expected.");
        return nullptr;
    }
    napi_get_value_int64(env, value, &result);
    return BundleStateCommon::NapiGetNull(env);
}

napi_value BundleStateCommon::GetInt32NumberValue(const napi_env &env, const napi_value &value, int32_t &result)
{
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_number) {
        BUNDLE_ACTIVE_LOGE("Wrong argument type. Number expected.");
        return nullptr;
    }
    napi_get_value_int32(env, value, &result);
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

std::shared_ptr<std::map<std::string, BundleActivePackageStats>> BundleStateCommon::GetPackageStats(
    int64_t &beginTime, int64_t &endTime, int32_t &errCode)
{
    std::vector<BundleActivePackageStats> packageStats =
        BundleActiveClient::GetInstance().QueryPackageStats(INTERVAL_TYPE_DEFAULT, beginTime, endTime, errCode);
    std::shared_ptr<std::map<std::string, BundleActivePackageStats>> mergedPackageStats =
        std::make_shared<std::map<std::string, BundleActivePackageStats>>();
    if (packageStats.empty()) {
        return nullptr;
    }
    for (auto packageStat : packageStats) {
        std::map<std::string, BundleActivePackageStats>::iterator iter =
            mergedPackageStats->find(packageStat.bundleName_);
        if (iter != mergedPackageStats->end()) {
            MergePackageStats(iter->second, packageStat);
        } else {
            mergedPackageStats->
                insert(std::pair<std::string, BundleActivePackageStats>(packageStat.bundleName_, packageStat));
        }
    }
    return mergedPackageStats;
}

void BundleStateCommon::MergePackageStats(BundleActivePackageStats &left, const BundleActivePackageStats &right)
{
    if (left.bundleName_ != right.bundleName_) {
        BUNDLE_ACTIVE_LOGE("Merge package stats failed, existing packageName : %{public}s,"
            " new packageName : %{public}s,", left.bundleName_.c_str(), right.bundleName_.c_str());
        return;
    }
    left.lastTimeUsed_ = std::max(left.lastTimeUsed_, right.lastTimeUsed_);
    left.lastContiniousTaskUsed_ = std::max(left.lastContiniousTaskUsed_, right.lastContiniousTaskUsed_);
    left.totalInFrontTime_ += right.totalInFrontTime_;
    left.totalContiniousTaskUsedTime_ += right.totalContiniousTaskUsedTime_;
    left.bundleStartedCount_ += right.bundleStartedCount_;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


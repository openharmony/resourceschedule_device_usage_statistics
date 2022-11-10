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
#include "bundle_state_common.h"

namespace OHOS {
namespace DeviceUsageStats {
const int ERR_MULTIPLE = 100;
AsyncWorkData::AsyncWorkData(napi_env napiEnv)
{
    env = napiEnv;
}

AsyncWorkData::~AsyncWorkData()
{
    if (callback) {
        BUNDLE_ACTIVE_LOGI("delete callback");
        napi_delete_reference(env, callback);
        callback = nullptr;
    }
    if (asyncWork) {
        BUNDLE_ACTIVE_LOGI("delete asyncwork");
        napi_delete_async_work(env, asyncWork);
        asyncWork = nullptr;
    }
}

napi_value BundleStateCommon::HandleParamErr(const napi_env &env, int32_t errCode, const std::string& operation)
{
    if (errCode == ERR_OK) {
        return nullptr;
    }
    BUNDLE_ACTIVE_LOGE("HandleParamErr %{public}d", errCode);
    auto iter = paramErrCodeMsgMap.find(errCode);
    if (iter != paramErrCodeMsgMap.end()) {
        std::string errMessage = "BussinessError 401: Parameter error. ";
        errMessage.append(operation);
        errMessage.append(iter->second);
        napi_throw_error(env, std::to_string(ERR_PARAM_ERROR).c_str(), errMessage.c_str());
    }
    return nullptr;
}

std::string BundleStateCommon::GetSaErrCodeMsg(int32_t errCode, int32_t reflectCode)
{
    BUNDLE_ACTIVE_LOGE("GetSaErrCodeMsg %{public}d", errCode);
    auto iter = saErrCodeMsgMap.find(errCode);
    std::string errMessage;
    if (iter != saErrCodeMsgMap.end()) {
        errMessage.append("BussinessError ");
        errMessage.append(std::to_string(reflectCode)).append(":").append(iter->second);
    }
    return errMessage;
}

napi_value BundleStateCommon::NapiGetNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

void BundleStateCommon::GetCallbackPromiseResult(const napi_env &env,
    const AsyncWorkData &workData, const napi_value &result)
{
    if (workData.isCallback) {
        SetCallbackInfo(env, workData.callback, workData.errorCode, result);
    } else {
        SetPromiseInfo(env, workData.deferred, result, workData.errorCode);
    }
}

void BundleStateCommon::SetCallbackInfo(
    const napi_env &env, const napi_ref &callbackIn, const int32_t &errorCode, const napi_value &result)
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
    napi_env env, const std::vector<BundleActiveEvent> &bundleActiveStates, napi_value result, bool isNewVersion)
{
    int32_t index = 0;
    for (const auto &item : bundleActiveStates) {
        napi_value bundleActiveState = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &bundleActiveState));

        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.bundleName_.c_str(), NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "bundleName", bundleName));

        napi_value eventId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.eventId_, &eventId));

        napi_value eventOccurredTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, item.timeStamp_, &eventOccurredTime));

        if (isNewVersion) {
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "eventId", eventId));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "eventOccurredTime",
                eventOccurredTime));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "stateType", eventId));
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, bundleActiveState, "stateOccurredTime",
                eventOccurredTime));
        }

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

void BundleStateCommon::GetBundleActiveEventStatsForResult(napi_env env,
    const std::vector<BundleActiveEventStats> &eventStats, napi_value result)
{
    int32_t index = 0;
    for (const auto &item : eventStats) {
        napi_value eventStatsObject = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &eventStatsObject));

        napi_value name = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.name_.c_str(), NAPI_AUTO_LENGTH, &name));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, eventStatsObject, "name", name));

        napi_value eventId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.eventId_, &eventId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, eventStatsObject, "eventId", eventId));

        napi_value count = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.count_, &count));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, eventStatsObject, "count", count));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, eventStatsObject));
        index++;
    }
}

void BundleStateCommon::GetBundleActiveNotificationNumberForResult(napi_env env,
    const std::vector<BundleActiveEventStats> &eventStats, napi_value result)
{
    int32_t index = 0;
    for (const auto &item : eventStats) {
        napi_value eventStatsObject = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &eventStatsObject));

        napi_value name = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_create_string_utf8(env, item.name_.c_str(), NAPI_AUTO_LENGTH, &name));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, eventStatsObject, "name", name));

        napi_value eventId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.eventId_, &eventId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, eventStatsObject, "eventId", eventId));

        napi_value count = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, item.count_, &count));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, eventStatsObject, "count", count));

        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, eventStatsObject));
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
    const BundleActiveModuleRecord &moduleRecords, napi_value moduleObject)
{
        napi_value bundleName = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moduleRecords.bundleName_.c_str(),
            NAPI_AUTO_LENGTH, &bundleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "bundleName", bundleName));
        napi_value appLabelId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, moduleRecords.appLabelId_, &appLabelId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "appLabelId", appLabelId));
        napi_value moduleName = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moduleRecords.moduleName_.c_str(), NAPI_AUTO_LENGTH,
            &moduleName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "moduleName", moduleName));
        napi_value labelId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, moduleRecords.labelId_, &labelId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "labelId", labelId));
        napi_value descriptionId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, moduleRecords.descriptionId_, &descriptionId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "descriptionId", descriptionId));
        napi_value abilityName = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, moduleRecords.abilityName_.c_str(), NAPI_AUTO_LENGTH,
            &abilityName));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityName", abilityName));
        napi_value abilityLableId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, moduleRecords.abilityLableId_, &abilityLableId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityLableId", abilityLableId));
        napi_value abilityDescriptionId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, moduleRecords.abilityDescriptionId_,
            &abilityDescriptionId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityDescriptionId",
            abilityDescriptionId));
        napi_value abilityIconId = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_uint32(env, moduleRecords.abilityIconId_, &abilityIconId));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "abilityIconId", abilityIconId));
        napi_value launchedCount = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, moduleRecords.launchedCount_, &launchedCount));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "launchedCount", launchedCount));
        napi_value lastModuleUsedTime = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_int64(env, moduleRecords.lastModuleUsedTime_, &lastModuleUsedTime));
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject,
            "lastModuleUsedTime", lastModuleUsedTime));
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
        NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &formRecords));
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
            NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, formObject, "count", count));
            NAPI_CALL_RETURN_VOID(env, napi_set_element(env, formRecords, formIdx, formObject));
            formIdx++;
        }
        NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, moduleObject, "formRecords", formRecords));
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, result, index, moduleObject));
        index++;
    }
}

void BundleStateCommon::SetPromiseInfo(const napi_env &env, const napi_deferred &deferred,
    const napi_value &result, const int32_t &errorCode)
{
    switch (errorCode) {
        case ERR_OK:
            napi_resolve_deferred(env, deferred, result);
            break;
        default:
            napi_reject_deferred(env, deferred, GetErrorValue(env, errorCode));
            break;
    }
}

int32_t BundleStateCommon::GetReflectErrCode(int32_t errCode)
{
    if (errCode < ERR_GET_SYSTEM_ABILITY_MANAGER_FAILED) {
        return errCode;
    }
    return errCode / ERR_MULTIPLE;
}

napi_value BundleStateCommon::GetErrorValue(napi_env env, int32_t errCode)
{
    if (errCode == ERR_OK) {
        return NapiGetNull(env);
    }
    napi_value result = nullptr;
    napi_value eCode = nullptr;
    napi_value eMsg = nullptr;
    int32_t reflectCode = GetReflectErrCode(errCode);
    std::string errMsg = GetSaErrCodeMsg(errCode, reflectCode);
    NAPI_CALL(env, napi_create_int32(env, reflectCode, &eCode));
    NAPI_CALL(env, napi_create_string_utf8(env, errMsg.c_str(), errMsg.length(), &eMsg));
    NAPI_CALL(env, napi_create_object(env, &result));
    NAPI_CALL(env, napi_set_named_property(env, result, "code", eCode));
    NAPI_CALL(env, napi_set_named_property(env, result, "message", eMsg));
    return result;
}

napi_value BundleStateCommon::JSParaError(const napi_env &env, const napi_ref &callback, const int32_t &errorCode)
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

void BundleStateCommon::SettingAsyncWorkData(
    const napi_env &env, const napi_ref &callback, AsyncWorkData &workData, napi_value &promise)
{
    if (callback) {
        workData.callback = callback;
        workData.isCallback = true;
    } else {
        napi_deferred deferred = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_create_promise(env, &deferred, &promise));
        workData.deferred = deferred;
        workData.isCallback = false;
    }
}

std::shared_ptr<std::map<std::string, BundleActivePackageStats>> BundleStateCommon::QueryBundleStatsInfos(
    int64_t &beginTime, int64_t &endTime, int32_t &errCode)
{
    std::vector<BundleActivePackageStats> packageStats;
    errCode = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats, INTERVAL_TYPE_DEFAULT,
        beginTime, endTime);
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

std::unique_ptr<AsyncCallbackInfoEventStats> BundleStateCommon::HandleEventStatsInfo(const napi_env &env,
    AsyncCallbackInfoEventStats *asyncCallbackInfo, EventStatesParamsInfo &params)
{
    if (!asyncCallbackInfo) {
        params.errorCode = ERR_ASYNC_CALLBACK_NULLPTR;
        BundleStateCommon::HandleParamErr(env, ERR_ASYNC_CALLBACK_NULLPTR, "");
        return nullptr;
    }
    if (memset_s(asyncCallbackInfo, sizeof(*asyncCallbackInfo), 0, sizeof(*asyncCallbackInfo)) != EOK) {
        params.errorCode = ERR_ASYNC_CALLBACK_INIT_FAILED;
        BundleStateCommon::HandleParamErr(env, ERR_ASYNC_CALLBACK_INIT_FAILED, "");
        delete asyncCallbackInfo;
        asyncCallbackInfo = nullptr;
        return nullptr;
    }
    std::unique_ptr<AsyncCallbackInfoEventStats> callbackPtr {asyncCallbackInfo};
    callbackPtr->beginTime = params.beginTime;
    callbackPtr->endTime = params.endTime;
    BUNDLE_ACTIVE_LOGI("CallbackPtr->beginTime: %{public}lld, callbackPtr->endTime: %{public}lld",
        (long long)callbackPtr->beginTime, (long long)callbackPtr->endTime);
    return callbackPtr;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


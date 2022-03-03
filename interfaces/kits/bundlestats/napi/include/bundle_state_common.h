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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_COMMON_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_COMMON_H

#include "bundle_active_client.h"
#include "bundle_state_data.h"
#include "bundle_state_query.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DeviceUsageStats {
const std::int32_t STR_MAX_SIZE = 64;
class BundleStateCommon {
public:
    static napi_value NapiGetboolean(napi_env env, const bool &isValue);

    static napi_value NapiGetNull(napi_env env);

    static napi_value NapiGetUndefined(napi_env env);

    static napi_value GetCallbackErrorValue(napi_env env, int errCode);

    static void SettingCallbackPromiseInfo(
        const napi_env &env, const napi_ref &callback, CallbackPromiseInfo &info, napi_value &promise);

    static void GetCallbackPromiseResult(const napi_env &env, const CallbackPromiseInfo &info,
        const napi_value &result);

    static void SetCallbackInfo(
        const napi_env &env, const napi_ref &callbackIn, const int &errorCode, const napi_value &result);

    static void GetBundleActiveEventForResult(
        napi_env env, const std::vector<BundleActiveEvent> &BundleActiveState, napi_value result);

    static void GetBundleStateInfoByIntervalForResult(
        napi_env env, const std::vector<BundleActivePackageStats> &packageStats, napi_value result);

    static void GetBundleStateInfoForResult(napi_env env,
        const std::shared_ptr<std::map<std::string, BundleActivePackageStats>> &packageStats, napi_value result);

    static void SetPromiseInfo(const napi_env &env, const napi_deferred &deferred, const napi_value &result);

    static napi_value JSParaError(const napi_env &env, const napi_ref &callback);

    static std::string GetTypeStringValue(napi_env env, napi_value param, const std::string &result);

    static napi_value GetInt64NumberValue(const napi_env &env, const napi_value &value, int64_t &result);

    static napi_value GetInt32NumberValue(const napi_env &env, const napi_value &value, int32_t &result);

    static std::shared_ptr<std::map<std::string, BundleActivePackageStats>> GetPackageStats(
	    int64_t &beginTime, int64_t &endTime);

    static void MergePackageStats(BundleActivePackageStats &left, const BundleActivePackageStats &right);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_COMMON_H
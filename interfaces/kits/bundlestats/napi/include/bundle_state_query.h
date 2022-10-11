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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_QUERY_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_QUERY_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DeviceUsageStats {
    // napi_value IsIdleState(napi_env env, napi_callback_info info);
    napi_value QueryAppUsagePriorityGroup(napi_env env, napi_callback_info info);
    napi_value QueryCurrentBundleActiveStates(napi_env env, napi_callback_info info);
    napi_value QueryBundleActiveStates(napi_env env, napi_callback_info info);
    napi_value QueryBundleStateInfoByInterval(napi_env env, napi_callback_info info);
    napi_value QueryBundleStateInfos(napi_env env, napi_callback_info info);
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_QUERY_H
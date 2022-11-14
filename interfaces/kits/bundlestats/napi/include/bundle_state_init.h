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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INIT_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INIT_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DeviceUsageStats {
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((constructor)) void RegisterModule(void);
static napi_value BundleStateInit(napi_env env, napi_value exports);
static napi_value InitIntervalType(napi_env env, napi_value exports);
static napi_value InitGroupType(napi_env env, napi_value exports);
static napi_value EnumTypeConstructor(napi_env env, napi_callback_info info);

#ifdef __cplusplus
}
#endif

/*
 * Module define
 */
napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = BundleStateInit,
    .nm_modname = "bundleState",
    .nm_priv = (static_cast<void*>(0)),
    .reserved = {0}
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INIT_H


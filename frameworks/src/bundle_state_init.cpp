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

#include "bundle_state_init.h"
#include "bundle_state_query.h"

namespace OHOS {
namespace DeviceUsageStats {
EXTERN_C_START

/*
 * Module export function
 */
static napi_value BundleStateInit(napi_env env, napi_value exports)
{
    /*
     * Propertise define
     */
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("isIdleState", IsIdleState),
        DECLARE_NAPI_FUNCTION("queryAppUsagePriorityGroup", QueryAppUsagePriorityGroup),
        DECLARE_NAPI_FUNCTION("queryCurrentBundleActiveStates", QueryCurrentBundleActiveStates),
        DECLARE_NAPI_FUNCTION("queryBundleActiveStates", QueryBundleActiveStates),
        DECLARE_NAPI_FUNCTION("queryBundleStateInfoByInterval", QueryBundleStateInfoByInterval),
        DECLARE_NAPI_FUNCTION("queryBundleStateInfos", QueryBundleStateInfos)
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

/*
 * Module register function
 */
__attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
EXTERN_C_END
}  // namespace BackgroundTaskMgr
}  // namespace OHOS
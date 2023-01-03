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

#include "bundle_state_condition.h"
#include "bundle_state_query.h"

namespace OHOS {
namespace DeviceUsageStats {
EXTERN_C_START

static const uint8_t ARG_FIRST = 1;

napi_ref typeConstructor_ = nullptr;

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
        DECLARE_NAPI_FUNCTION("queryBundleStateInfos", QueryBundleStateInfos),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    InitIntervalType(env, exports);
    return exports;
}

napi_value InitIntervalType(napi_env env, napi_value exports)
{
    napi_value by_optimized;
    napi_value by_daily;
    napi_value by_weekly;
    napi_value by_monthly;
    napi_value by_annually;
    int32_t refCount = 1;

    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::IntervalType::BY_OPTIMIZED),
        &by_optimized);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::IntervalType::BY_DAILY),
        &by_daily);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::IntervalType::BY_WEEKLY),
        &by_weekly);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::IntervalType::BY_MONTHLY),
        &by_monthly);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::IntervalType::BY_ANNUALLY),
        &by_annually);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("BY_OPTIMIZED", by_optimized),
        DECLARE_NAPI_STATIC_PROPERTY("BY_DAILY", by_daily),
        DECLARE_NAPI_STATIC_PROPERTY("BY_WEEKLY", by_weekly),
        DECLARE_NAPI_STATIC_PROPERTY("BY_MONTHLY", by_monthly),
        DECLARE_NAPI_STATIC_PROPERTY("BY_ANNUALLY", by_annually),
    };

    napi_value result = nullptr;
    napi_define_class(env, "IntervalType", NAPI_AUTO_LENGTH, EnumTypeConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_create_reference(env, result, refCount, &typeConstructor_);
    napi_set_named_property(env, exports, "IntervalType", result);
    return exports;
}

napi_value EnumTypeConstructor(napi_env env, napi_callback_info info)
{
    size_t argc = 0;
    napi_value args[ARG_FIRST] = {0};
    napi_value res = nullptr;
    void *data = nullptr;

    napi_status status = napi_get_cb_info(env, info, &argc, args, &res, &data);
    if (status != napi_ok) {
        return nullptr;
    }

    return res;
}

/*
 * Module register function
 */
__attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
EXTERN_C_END
}  // namespace DeviceUsageStats
}  // namespace OHOS


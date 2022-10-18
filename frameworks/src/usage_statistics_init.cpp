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

#include "usage_statistics_init.h"

#include "bundle_state_condition.h"
#include "bundle_state_query_napi.h"
#include "bundle_active_app_group_napi.h"

namespace OHOS {
namespace DeviceUsageStats {
EXTERN_C_START

static const uint8_t ARG_FIRST = 1;

napi_ref typeConstructor_ = nullptr;

/*
 * Module export function
 */
static napi_value UsageStatisticsInit(napi_env env, napi_value exports)
{
    /*
     * Propertise define
     */
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("isIdleState", IsIdleState),
        DECLARE_NAPI_FUNCTION("queryAppGroup", QueryAppGroup),
        DECLARE_NAPI_FUNCTION("queryCurrentBundleEvents", QueryCurrentBundleEvents),
        DECLARE_NAPI_FUNCTION("queryBundleEvents", QueryBundleEvents),
        DECLARE_NAPI_FUNCTION("queryBundleStatsInfoByInterval", QueryBundleStatsInfoByInterval),
        DECLARE_NAPI_FUNCTION("queryBundleStatsInfos", QueryBundleStatsInfos),
        DECLARE_NAPI_FUNCTION("queryModuleUsageRecords", QueryModuleUsageRecords),
        DECLARE_NAPI_FUNCTION("setAppGroup", SetAppGroup),
        DECLARE_NAPI_FUNCTION("registerAppGroupCallBack", RegisterAppGroupCallBack),
        DECLARE_NAPI_FUNCTION("unregisterAppGroupCallBack", UnRegisterAppGroupCallBack),
        DECLARE_NAPI_FUNCTION("queryDeviceEventStats", QueryDeviceEventStats),
        DECLARE_NAPI_FUNCTION("queryNotificationEventStats", QueryNotificationEventStats)
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));

    InitIntervalType(env, exports);
    InitAppGroupType(env, exports);
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

napi_value InitAppGroupType(napi_env env, napi_value exports)
{
    napi_value active_group_alive;
    napi_value active_group_daily;
    napi_value active_group_fixed;
    napi_value active_group_rare;
    napi_value active_group_limit;
    napi_value active_group_never;
    int32_t refCount = 1;

    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::AppGroupType::ALIVE_GROUP),
        &active_group_alive);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::AppGroupType::DAILY_GROUP),
        &active_group_daily);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::AppGroupType::FIXED_GROUP),
        &active_group_fixed);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::AppGroupType::RARE_GROUP),
        &active_group_rare);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::AppGroupType::LIMITED_GROUP),
        &active_group_limit);
    napi_create_uint32(env, static_cast<uint32_t>(BundleStateCondition::AppGroupType::NEVER_GROUP),
        &active_group_never);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("ACTIVE_GROUP_ALIVE", active_group_alive),
        DECLARE_NAPI_STATIC_PROPERTY("ACTIVE_GROUP_DAILY", active_group_daily),
        DECLARE_NAPI_STATIC_PROPERTY("ACTIVE_GROUP_FIXED", active_group_fixed),
        DECLARE_NAPI_STATIC_PROPERTY("ACTIVE_GROUP_RARE", active_group_rare),
        DECLARE_NAPI_STATIC_PROPERTY("ACTIVE_GROUP_LIMIT", active_group_limit),
        DECLARE_NAPI_STATIC_PROPERTY("ACTIVE_GROUP_NEVER", active_group_never),
    };

    napi_value result = nullptr;
    napi_define_class(env, "AppGroupType", NAPI_AUTO_LENGTH, EnumTypeConstructor,
        nullptr, sizeof(desc) / sizeof(*desc), desc, &result);
    napi_create_reference(env, result, refCount, &typeConstructor_);
    napi_set_named_property(env, exports, "AppGroupType", result);
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
    napi_module_register(&_usageStatisticsModule);
}
EXTERN_C_END
}  // namespace DeviceUsageStats
}  // namespace OHOS


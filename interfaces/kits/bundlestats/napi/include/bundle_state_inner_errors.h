/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INNER_ERRORS_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INNER_ERRORS_H

#include "errors.h"

namespace OHOS {
namespace DeviceUsageStats {
/**
 * ErrCode layout
 *
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * | Bit |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|01|00|
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 * |Field|Reserved|        Subsystem      |  Module      |                              Code             |
 * +-----+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
 */

// DeviceUsageStats's module const defined.
enum : int32_t {
    DEVICE_USAGE_STATS_MODULE_COMMON = 0x01,
};

// Offset of device usage stats sub-system's errCode base, number : 39911424.
constexpr ErrCode DEVICE_USAGE_STATS_COMMON_ERR_OFFSET =
    ErrCodeOffset(SUBSYS_IAWARE, DEVICE_USAGE_STATS_MODULE_COMMON);
// Device Usage Stats Common Error Code Defined.
enum : int32_t {
    ERR_USAGE_STATS_BUNDLENAME_EMPTY = DEVICE_USAGE_STATS_COMMON_ERR_OFFSET + 1,
    ERR_USAGE_STATS_BUNDLENAME_TYPE,
    ERR_USAGE_STATS_ASYNC_CALLBACK_NULLPTR,
    ERR_USAGE_STATS_ASYNC_CALLBACK_INIT_FAILED,
    ERR_USAGE_STATS_BEGIN_TIME_INVALID,
    ERR_USAGE_STATS_END_TIME_INVALID,
    ERR_USAGE_STATS_TIME_INTERVAL,
    ERR_USAGE_STATS_INTERVAL_TYPE,
    ERR_USAGE_STATS_INTERVAL_NUMBER,
    ERR_SERVICE_FAILED,
    ERR_USAGE_STATS_METHOD_CALLED_FAILED,
    ERR_USAGE_STATS_INVALID_PARAM,
};

enum {
    ERR_PERMISSION_DENIED = 201,
    ERR_NOT_SYSTEM_APP = 202,
    ERR_PARAM_ERROR = 401,
    ERR_MEMORY_OPERATION_FAILED = 10000001,
    ERR_PARCEL_WRITE_FALIED,
    ERR_SYSTEM_SERVICE_OPERATION_FAILED,
    ERR_IPC_COMMUNICATION_FAILED,
    ERR_APPLICATION_IS_NOT_INSTALLED,
    ERR_GET_APPLICATION_INFO_FAILED,
    ERR_TIME_OPERATION_FAILED,
    ERR_APPLICATION_GROUP_OPERATION_REPEATED = 10100001,
    ERR_NO_APP_GROUP_INFO_IN_DATABASE,
};

enum ParamError {
    ERR_PARAMETERS_NUMBER = 401001,
    ERR_CALL_BACK_TYPE,
    ERR_PARAMETERS_EMPTY,
    ERR_BUNDLE_NAME_TYPE,
    ERR_BEGIN_TIME_TYPE,
    ERR_BEGIN_TIME_LESS_THEN_ZERO,
    ERR_END_TIME_TYPE,
    ERR_END_TIME_LESS_THEN_BEGIN_TIME,
    ERR_INTERVAL_TYPE,
    ERR_INTERVAL_OUT_OF_RANGE,
    ERR_NEW_GROUP_TYPE,
    ERR_NEW_GROUP_OUT_OF_RANGE,
    ERR_MAX_RECORDS_NUM_TYPE,
    ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND,
    ERR_APP_GROUP_OBSERVER_CALLBACK_TYPE,
    ERR_REPEAT_REGISTER_APP_GROUP_OBSERVER,
    ERR_APP_GROUP_OBSERVER_IS_NULLPTR,
    ERR_ASYNC_CALLBACK_NULLPTR,
    ERR_ASYNC_CALLBACK_INIT_FAILED,
};

enum ServiceError {
    ERR_GET_SYSTEM_ABILITY_MANAGER_FAILED = 1000000301,
    ERR_GET_SYSTEM_ABILITY_FAILED,
    ERR_CHECK_SYSTEM_ABILITY_FAILED,
    ERR_SYSTEM_SERVICES_NOT_READY,
    ERR_REMOTE_OBJECT_IF_CAST_FAILED,
    ERR_SYSTEM_ABILITY_SUPPORT_FAILED,
    ERR_FIND_APP_USAGE_RECORDS_FAILED = 1000000601,
    ERR_NO_RECORDS_INFO_BY_INTERVAL,
    ERR_QUERY_TIME_OUT_OF_RANGE,
    ERR_GET_BUNDLE_USED_HISTORY_FAILED,
    ERR_REPEAT_REGISTER_OR_DEREGISTER_GROUP_CALLBACK = 1010000101,
    ERR_REPEAT_SET_APP_GROUP,
};

static std::map<int32_t, std::string> saErrCodeMsgMap = {
    {ERR_PERMISSION_DENIED, "Permission denied."},
    {ERR_MEMORY_OPERATION_FAILED, "Memory operation failed. create object failed."},
    {ERR_PARCEL_WRITE_FALIED, "Parcel operation failed. Failed to write the parcel."},
    {ERR_GET_SYSTEM_ABILITY_MANAGER_FAILED, "System service operation failed. Failed to get system ability manager."},
    {ERR_GET_SYSTEM_ABILITY_FAILED, "System service operation failed. Failed to get system ability manager."},
    {ERR_CHECK_SYSTEM_ABILITY_FAILED, "System service operation failed. Failed to get system ability."},
    {ERR_SYSTEM_SERVICES_NOT_READY, "System service operation failed. The service is not ready."},
    {ERR_REMOTE_OBJECT_IF_CAST_FAILED, "System service operation failed. The remote object if cast failed."},
    {ERR_SYSTEM_ABILITY_SUPPORT_FAILED, "System service operation failed. System ability support failed."},
    {ERR_IPC_COMMUNICATION_FAILED, "IPC failed. Failed to access the system service."},
    {ERR_APPLICATION_IS_NOT_INSTALLED, "The application is not installed. Failed to operation the application."},
    {ERR_FIND_APP_USAGE_RECORDS_FAILED, "Failed to get the application information. Cannot find app usage records."},
    {ERR_NO_RECORDS_INFO_BY_INTERVAL, "Failed to get the application information."
        "According to inetrvalType. no information is recorded in the related table."},
    {ERR_QUERY_TIME_OUT_OF_RANGE,
        "Failed to get the application information. According to beginTime and endTime. no information is recorded"},
    {ERR_GET_BUNDLE_USED_HISTORY_FAILED,
        "Failed to get the application information. According to userId、bundleName, no information is recorded"},
    {ERR_TIME_OPERATION_FAILED, "Get system or actual time operation failed."},
    {ERR_REPEAT_REGISTER_OR_DEREGISTER_GROUP_CALLBACK, "Repeated operation on the application group."
        "Repeat to register or deregister, an interception application group"},
    {ERR_REPEAT_SET_APP_GROUP, "Repeated operation on the application group."
        "The application group are the the same and do not need te be updated."},
    {ERR_NO_APP_GROUP_INFO_IN_DATABASE,
        "Failed to get the application group information. The application group infomation cannot be found."},
};

static std::map<int32_t, std::string> paramErrCodeMsgMap = {
    {ERR_PARAMETERS_NUMBER, " api interface parameters count is error."},
    {ERR_PARAMETERS_EMPTY, " cannot be empty."},
    {ERR_CALL_BACK_TYPE, " Callback type must be function."},
    {ERR_BUNDLE_NAME_TYPE, "The type of bundleName must be string."},
    {ERR_BEGIN_TIME_TYPE, "The type of beginTime must be int64."},
    {ERR_BEGIN_TIME_LESS_THEN_ZERO, "The beginTime value must be greater than or equal to 0."},
    {ERR_END_TIME_TYPE, "The type of endTime must be int64."},
    {ERR_END_TIME_LESS_THEN_BEGIN_TIME, "The endTime value must be greater than beginTime."},
    {ERR_INTERVAL_TYPE, "The type of byInterval must be int32."},
    {ERR_INTERVAL_OUT_OF_RANGE, "The value of byInterval must range from BY_OPTIMIZED to BY_ANNUALLY."},
    {ERR_NEW_GROUP_TYPE, "The type of newGroup must be int32."},
    {ERR_NEW_GROUP_OUT_OF_RANGE, "The type of newGroup must be {10,20,30,40,50,60}."},
    {ERR_MAX_RECORDS_NUM_TYPE, "The type of maxNum must be int32."},
    {ERR_MAX_RECORDS_NUM_BIGER_THEN_ONE_THOUSAND, "The maxNum cannot exceed 1000 or less then 1."},
    {ERR_APP_GROUP_OBSERVER_CALLBACK_TYPE, "The type of callback must be Callback<AppGroupCallbackInfo>."},
    {ERR_REPEAT_REGISTER_APP_GROUP_OBSERVER, "Repeat register app group observer, one process only hava one observer"},
    {ERR_APP_GROUP_OBSERVER_IS_NULLPTR, "App observer is nullptr, please register before unRegister."},
    {ERR_ASYNC_CALLBACK_NULLPTR, "Create async callback info object failed."},
    {ERR_ASYNC_CALLBACK_INIT_FAILED, "Init async callback info object failed."},
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_INNER_ERRORS_H


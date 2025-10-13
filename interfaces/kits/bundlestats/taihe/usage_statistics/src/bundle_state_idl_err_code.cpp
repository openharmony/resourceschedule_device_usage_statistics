/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "bundle_active_log.h"
#include "bundle_state_idl_err_code.h"
#include "bundle_state_inner_errors.h"

namespace OHOS {
namespace DeviceUsageStats {
const int32_t ERR_MULTIPLE = 100;
std::string BundleStateIDLErrCode::GetSaErrCodeMsg(int32_t errCode, int32_t reflectCode)
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

int32_t BundleStateIDLErrCode::GetReflectErrCode(int32_t errCode)
{
    if (errCode < ERR_GET_SYSTEM_ABILITY_MANAGER_FAILED) {
        return errCode;
    }
    return errCode / ERR_MULTIPLE;
}

std::string BundleStateIDLErrCode::GetErrorCode(int32_t errCode)
{
    if (errCode == ERR_OK) {
        return "";
    }
    int32_t reflectCode = GetReflectErrCode(errCode);
    return GetSaErrCodeMsg(errCode, reflectCode);
}

std::string BundleStateIDLErrCode::HandleParamErr(int32_t errCode, const std::string& operation)
{
    if (errCode == ERR_OK) {
        return "";
    }
    BUNDLE_ACTIVE_LOGE("HandleParamErr %{public}d", errCode);
    std::string errMessage = "BussinessError 401: Parameter error. ";
    auto iter = paramErrCodeMsgMap.find(errCode);
    if (iter != paramErrCodeMsgMap.end()) {
        errMessage.append(operation);
        errMessage.append(iter->second);
    }
    return errMessage;
}

}  // namespace DeviceUsageStats
}  // namespace OHOS
   
   
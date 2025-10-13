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

#ifndef FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_IDL_ERR_CODE_H
#define FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_IDL_ERR_CODE_H

#include "string_ex.h"
#include <map>

namespace OHOS {
namespace DeviceUsageStats {
class BundleStateIDLErrCode {
public:
    static std::string GetSaErrCodeMsg(int32_t errCode, int32_t reflectCode);
    static int32_t GetReflectErrCode(int32_t errCode);
    static std::string GetErrorCode(int32_t errCode);
    static std::string HandleParamErr(int32_t errCode, const std::string& operation);
};

}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // FOUNDATION_RESOURCESCHEDULE_DEVICE_USAGE_STATISTICS_BUNDLE_STATE_IDL_ERR_CODE_H
  
  
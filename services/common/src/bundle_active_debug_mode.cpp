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

#include <parameters.h>
#include <string_ex.h>

#include "bundle_active_debug_mode.h"

using OHOS::system::GetIntParameter;
using OHOS::system::GetParameter;

namespace OHOS {
namespace DeviceUsageStats {
constexpr int32_t DEBUG_ON_DEFAULT = 0;
std::string DEBUG_MODE = "persist.sys.device_usage_debug_on";
const bool DEBUG_ON = static_cast<bool>(GetIntParameter(DEBUG_MODE, DEBUG_ON_DEFAULT));
}  // namespace DeviceUsageStats
}  // namespace OHOS


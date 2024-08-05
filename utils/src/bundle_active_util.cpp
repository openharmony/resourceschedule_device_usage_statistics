/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
const int32_t MILLISECOND_TO_MICROSECOND = 1000;
std::string BundleActiveUtil::GetBundleUsageKey(const std::string &bundleName, const int32_t uid)
{
    return bundleName + std::to_string(uid);
}

int64_t BundleActiveUtil::GetFFRTDelayTime(const int64_t& delayTime)
{
    return delayTime * MILLISECOND_TO_MICROSECOND;
}

} // namespace DeviceUsageStats
}  // namespace OHOS


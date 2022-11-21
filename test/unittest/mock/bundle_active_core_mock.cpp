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

#include "bundle_active_core.h"
#include "bundle_state_inner_errors.h"

namespace OHOS {
namespace DeviceUsageStats {
static int64_t LARGE_NUM = 20000000000000;
int64_t BundleActiveCore::CheckTimeChangeAndGetWallTime(int32_t userId)
{
    int32_t userIdTemp = 100;
    if (userId == userIdTemp) {
        return LARGE_NUM;
    }
    return ERR_TIME_OPERATION_FAILED;
}

std::shared_ptr<BundleActiveUserService> BundleActiveCore::GetUserDataAndInitializeIfNeeded(const int32_t userId,
    const int64_t timeStamp, const bool debug)
{
    return nullptr;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
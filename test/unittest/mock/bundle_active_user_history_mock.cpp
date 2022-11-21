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

#include "bundle_active_user_history.h"
#include "bundle_active_package_history.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace std;
shared_ptr<map<string, shared_ptr<BundleActivePackageHistory>>> BundleActiveUserHistory::GetUserHistory(
    const int32_t userId, const bool create)
{
    if (userId == 0) {
        return nullptr;
    }
    string bundleName = "default";
    return make_shared<map<string, shared_ptr<BundleActivePackageHistory>>>(bundleName,
        make_shared<BundleActivePackageHistory>());
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
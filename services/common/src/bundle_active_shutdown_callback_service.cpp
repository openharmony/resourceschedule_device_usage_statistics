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
#ifdef DEVICE_USAGES_STATISTICS_POWERMANGER_ENABLE
#include "bundle_active_shutdown_callback_service.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveShutdownCallbackService::BundleActiveShutdownCallbackService(
    std::shared_ptr<BundleActiveCore> bundleActiveCore)
{
    if (bundleActiveCore != nullptr) {
        bundleActiveCore_ = bundleActiveCore;
    }
}

void BundleActiveShutdownCallbackService::OnAsyncShutdown()
{
    bundleActiveCore_->ShutDown();
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif


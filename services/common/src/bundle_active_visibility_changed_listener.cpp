/*
 * Copyright (c) 2026-2026 Huawei Device Co., Ltd.
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
#include "bundle_active_visibility_changed_listener.h"
#include "bundle_active_window_visibility_manager.h"

namespace OHOS {
namespace DeviceUsageStats {

static std::shared_ptr<BundleActiveWindowVisibilityManager> g_windowVisibilityManager = nullptr;

void BundleActiveVisibilityChangedListener::SetWindowVisibilityManager(
    std::shared_ptr<BundleActiveWindowVisibilityManager> manager)
{
    g_windowVisibilityManager = manager;
}

void BundleActiveVisibilityChangedListener::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    if (windowVisibilityInfo.empty()) {
        BUNDLE_ACTIVE_LOGE("windowVisibility Info is empty");
        return;
    }

    auto manager = g_windowVisibilityManager;
    if (manager == nullptr) {
        BUNDLE_ACTIVE_LOGE("windowVisibility manager is empty");
        return;
    }
    BUNDLE_ACTIVE_LOGD("window count: %{public}zu", windowVisibilityInfo.size());
    manager->OnWindowVisibilityChanged(windowVisibilityInfo);
}

} // namespace DeviceUsageStats
} // namespace OHOS
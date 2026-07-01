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

#ifndef BUNDLE_ACTIVE_VISIBILITY_CHANGED_LISTENER_H
#define BUNDLE_ACTIVE_VISIBILITY_CHANGED_LISTENER_H

#include <memory>
#include "window_manager.h"

namespace OHOS {
namespace DeviceUsageStats {

class BundleActiveWindowVisibilityManager;
using OHOS::Rosen::WindowVisibilityInfo;

class BundleActiveVisibilityChangedListener : public OHOS::Rosen::IVisibilityChangedListener {
public:
    BundleActiveVisibilityChangedListener() = default;
    virtual ~BundleActiveVisibilityChangedListener() = default;

    /**
    * @brief 设置窗口可见性管理器
    * @param manager 窗口可见性管理器
    */
    static void SetWindowVisibilityManager(std::shared_ptr<BundleActiveWindowVisibilityManager> manager);

    /**
    * @brief 窗口可见性变化回调
    * @param windowVisibilityInfo 窗口可见性信息列表
    */
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override;
};

} // namespace DeviceUsageStats
} // namespace OHOS

#endif // BUNDLE_ACTIVE_VISIBILITY_CHANGED_LISTENER_H
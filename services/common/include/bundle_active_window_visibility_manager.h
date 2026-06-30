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

#ifndef BUNDLE_ACTIVE_WINDOW_VISIBILITY_MANAGER_H
#define BUNDLE_ACTIVE_WINDOW_VISIBILITY_MANAGER_H

#include <map>
#include <string>
#include <memory>
#include "ffrt.h"
#include "window_visibility_info.h"

namespace OHOS {
namespace DeviceUsageStats {
using OHOS::Rosen::WindowVisibilityInfo;

class BundleActiveWindowVisibilityManager {
public:
    BundleActiveWindowVisibilityManager() = default;
    ~BundleActiveWindowVisibilityManager() = default;

    /**
    * @brief 处理窗口可见性变化事件
    * @param windowVisibilityInfo 窗口可见性信息列表
    */
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo);

    /**
    * @brief 清除指定窗口可见性信息
    * @param bundleName 包名
    * @param abilityName Ability名称
    * @param moduleName 模块名称
    * @param uid 应用uid
    */
    void ClearAbilityWindowInfo(const std::string& bundleName, const std::string& abilityName,
        const std::string& moduleName, int32_t uid);

    /**
    * @brief 检查App的所有Ability是否都不可见
    * @param bundleName 包名
    * @return true表示所有窗口都不可见，false表示至少有一个窗口可见
    */
    bool IsAllAbilitiesInvisible(const std::string& bundleName);

private:
    /**
    * @brief Ability窗口信息结构体
    */
    struct AbilityWindowInfo {
        std::string bundleName;
        std::string abilityName;
        std::string moduleName;
        int32_t uid;
        std::map<uint32_t, bool> windowVisibilityMap; // windowId -> isVisible

        AbilityWindowInfo() : uid(0) {}
        AbilityWindowInfo(const std::string& bundle, const std::string& ability, const std::string& module, int32_t uid)
            : bundleName(bundle), abilityName(ability), moduleName(module), uid(uid) {}
    };

    /**
    * @brief 窗口信息数据结构体
    */
    struct WindowInfoData {
        std::string bundleName;
        std::string abilityName;
        std::string moduleName;
        int32_t uid;
        int32_t pid;
        uint32_t windowId;
        bool isVisible;
        bool isSystem;
    };

    /**
    * @brief 生成Ability的唯一标识键
    * @param uid 应用uid
    * @param abilityName Ability名称
    * @param moduleName 模块名称
    * @return 唯一标识键
    */
    std::string GenerateAbilityKey(int32_t uid, const std::string& abilityName,
        const std::string& moduleName);
    
    /**
    * @brief 检查所有窗口是否都不可见
    * @param windowInfo 窗口信息
    * @return true 标识所有窗口都不可见，false表示至少一个窗口可见
    */
    bool AreAllWindowsInvisible(const AbilityWindowInfo& windowInfo);

    /**
    * @brief 从WindowVisibilityInfo提取窗口信息
    * @param Info 窗口可见性信息
    * @return 窗口信息数据
    */
    WindowInfoData ExtractWindowInfo(const sptr<WindowVisibilityInfo>& info);

    /**
    * @brief 处理新的Ability窗口
    * @param abilityKey Ability唯一标识键
    * @param windowData 窗口信息数据
    */
    void HandleNewAbilityWindow(const std::string& abilityKey, const WindowInfoData& windowData);

    /**
    * @brief 更新已存在的Ability窗口
    * @param windowInfo 窗口信息
    * @param windowData 窗口信息数据
    */
    void UpdateExistingAbilityWindow(AbilityWindowInfo& windowInfo, const WindowInfoData& windowData);

    /**
    * @brief 报告App前后台变化事件
    * @param bundleName 包名
    * @param abilityName Ability名称
    * @param moduleName 模块名称
    * @param uid 应用uid
    * @param toForeground true:前台，即存在可视窗口，false:后台，即不存在可视窗口
    */
    void ReportAppStateChanged(const std::string& bundleName, const std::string& abilityName,
        const std::string& moduleName, int32_t uid, bool toForeground);

    /**
    * @brief 更新App可见性
    * @param bundleName 包名
    * @param abilityName Ability名称
    * @param moduleName 模块名称
    * @param uid 应用uid
    */
    void UpdateAppVisibility(const std::string& bundleName, const std::string& abilityName,
        const std::string& moduleName, int32_t uid);
    
    ffrt::mutex abilityWindowMapMutex_;
    std::map<std::string, AbilityWindowInfo> abilityWindowMap_; // abilityKey -> AbilityWindowInfo
    ffrt::mutex appVisibilityMapMutex_;
    std::map<std::string, bool> appVisibilityMap_; // bundleName -> isVisible
    };
} // namespace DeviceUsageStats
} // namespace OHOS

#endif // BUNDLE_ACTIVE_WINDOW_VISIBILITY_MANAGER_H
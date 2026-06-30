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
#include "bundle_active_window_visibility_manager.h"
#include "bundle_active_report_controller.h"
#include "bundle_active_event.h"
#include "bundle_active_account_helper.h"
#include "time_service_client.h"

namespace OHOS {
namespace DeviceUsageStats {

std::string BundleActiveWindowVisibilityManager::GenerateAbilityKey(int32_t uid,
    const std::string& abilityName, const std::string& moduleName)
{
    return std::to_string(uid) + "|" + abilityName + "|" + moduleName;
}

bool BundleActiveWindowVisibilityManager::AreAllWindowsInvisible(const AbilityWindowInfo& windowInfo)
{
    for (const auto& iter : windowInfo.windowVisibilityMap) {
        if (iter.second) {
            return false; // 至少有一个窗口可见
        }
    }
    return true;
}

void BundleActiveWindowVisibilityManager::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    if (windowVisibilityInfo.empty()) {
        BUNDLE_ACTIVE_LOGE("windowVisibility Info is empty");
        return;
    }

    std::vector<WindowInfoData> windowDataList;
    {
        std::unique_lock<ffrt::mutex> lock(abilityWindowMapMutex_);
        for (const auto& info : windowVisibilityInfo) {
            if (info == nullptr) {
                BUNDLE_ACTIVE_LOGD("window visibility info is null");
                continue;
            }

            WindowInfoData windowData = ExtractWindowInfo(info);
            if (windowData.uid <= 0 || windowData.bundleName.empty()) {
                BUNDLE_ACTIVE_LOGE("window info bundleName is empty");
                continue;
            }
            if (windowData.isSystem || windowData.abilityName.empty()) { // 窗口为大桌面或未获取abilityName时不处理
                continue;
            }
            windowDataList.push_back(windowData);
            std::string abilityKey = GenerateAbilityKey(windowData.uid, windowData.abilityName,
                windowData.moduleName);
            auto iter = abilityWindowMap_.find(abilityKey);
            if (iter == abilityWindowMap_.end()) {
                HandleNewAbilityWindow(abilityKey, windowData);
            } else {
                UpdateExistingAbilityWindow(iter->second, windowData);
            }
        }
    }
    // 在锁外更新App可见性和上报事件
    for (const auto& windowData : windowDataList) {
        UpdateAppVisibility(windowData.bundleName, windowData.abilityName, windowData.moduleName, windowData.uid);
    }
}

BundleActiveWindowVisibilityManager::WindowInfoData BundleActiveWindowVisibilityManager::ExtractWindowInfo(
    const sptr<WindowVisibilityInfo>& info)
{
    WindowInfoData data;
    data.bundleName = info->GetBundleName();
    data.abilityName = info->GetAbilityName();
    data.moduleName = info->GetModuleName();
    data.windowId = info->GetWindowId();
    data.isVisible = info->visibilityState_ < Rosen::WindowVisibilityState::WINDOW_VISIBILITY_STATE_TOTALLY_OCCUSION;
    data.isSystem = info->IsSystem(); // 与窗口沟通确认，该处用于检查是否为大桌面，是大桌面应用返回true
    data.uid = info->uid_;
    data.pid = info->pid_;
    BUNDLE_ACTIVE_LOGD("window visibility changed, bundleName is %{public}s, abilityName is %{public}s, "
        "moduleName is %{public}s, windowid is %{public}d, isVisible is %{public}d, uid is %{public}d, "
        "pid is %{public}d, isSystem is %{public}d", data.bundleName.c_str(), data.abilityName.c_str(),
        data.moduleName.c_str(), data.windowId, data.isVisible, data.uid, data.pid, data.isSystem);
    return data;
}

void BundleActiveWindowVisibilityManager::HandleNewAbilityWindow(const std::string& abilityKey,
    const WindowInfoData& windowData)
{
    AbilityWindowInfo windowInfo(
       windowData.bundleName, windowData.abilityName, windowData.moduleName, windowData.uid);
    windowInfo.windowVisibilityMap[windowData.windowId] = windowData.isVisible;
    abilityWindowMap_[abilityKey] = windowInfo;
    BUNDLE_ACTIVE_LOGD(
        "new abilityKey: %{public}s, windowId: %{public}u, visible: %{public}d",
        abilityKey.c_str(), windowData.windowId, windowData.isVisible);
}


void BundleActiveWindowVisibilityManager::UpdateExistingAbilityWindow(AbilityWindowInfo& windowInfo,
    const WindowInfoData& windowData)
{
    if (windowData.isVisible) {
        windowInfo.windowVisibilityMap[windowData.windowId] = windowData.isVisible;
    } else {
        windowInfo.windowVisibilityMap.erase(windowData.windowId);
    }
    // 如果所有窗口都不可见，从abilityWindowMap_中移除
    std::string abilityKey = GenerateAbilityKey(windowInfo.uid, windowInfo.abilityName,
        windowInfo.moduleName);
    if (windowInfo.windowVisibilityMap.empty()) {
        abilityWindowMap_.erase(abilityKey);
        BUNDLE_ACTIVE_LOGD("erase abilityKey from map: %{public}s", abilityKey.c_str());
    }
    BUNDLE_ACTIVE_LOGD(
        "update abilityKey: %{public}s, windowId: %{public}u, visible: %{public}d",
        abilityKey.c_str(), windowData.windowId, windowData.isVisible);
}

void BundleActiveWindowVisibilityManager::ClearAbilityWindowInfo(const std::string& bundleName,
    const std::string& abilityName, const std::string& moduleName, const int32_t uid)
{
    std::string abilityKey = GenerateAbilityKey(uid, abilityName, moduleName);
    {
        std::unique_lock<ffrt::mutex> lock(abilityWindowMapMutex_);
        auto iter = abilityWindowMap_.find(abilityKey);
        if (iter != abilityWindowMap_.end()) {
            abilityWindowMap_.erase(iter);
            BUNDLE_ACTIVE_LOGD("clear ability window info, abilityKey: %{public}s", abilityKey.c_str());
        }
    }
    // 更新应用可见性
    UpdateAppVisibility(bundleName, abilityName, moduleName, uid);
}

bool BundleActiveWindowVisibilityManager::IsAllAbilitiesInvisible(const std::string& bundleName)
{
    std::unique_lock<ffrt::mutex> lock(abilityWindowMapMutex_);
    for (const auto& iter : abilityWindowMap_) {
        if (iter.second.bundleName == bundleName) {
            if (!AreAllWindowsInvisible(iter.second)) {
                return false;
            }
        }
    }
    return true;
}

void BundleActiveWindowVisibilityManager::ReportAppStateChanged(const std::string& bundleName,
    const std::string& abilityName, const std::string& moduleName, int32_t uid, bool toForeground)
{
    int32_t userId = -1;
    OHOS::ErrCode ret = BundleActiveAccountHelper::GetUserId(uid, userId);
    if (ret != ERR_OK || userId == -1) {
        BUNDLE_ACTIVE_LOGE("GetUserId failed,bundleName:%{public}s, ret:%{public}d",
            bundleName.c_str(), ret);
        return;
    }
    BundleActiveReportHandlerObject tmpHandlerObject(userId, bundleName);
    BundleActiveEvent event(bundleName, abilityName, abilityName, moduleName, uid);
    tmpHandlerObject.event_ = event;
    sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
    tmpHandlerObject.event_.timeStamp_ = timer->GetBootTimeMs();
    if (toForeground) {
        tmpHandlerObject.event_.eventId_ = BundleActiveEvent::ABILITY_FOREGROUND;
    } else {
        tmpHandlerObject.event_.eventId_ = BundleActiveEvent::ABILITY_BACKGROUND;
    }

    BUNDLE_ACTIVE_LOGI("report app state changed, %{public}s", tmpHandlerObject.ToString().c_str());

    auto reportHandler = BundleActiveReportController::GetInstance().GetBundleReportHandler();
    if (reportHandler != nullptr) {
        std::shared_ptr<BundleActiveReportHandlerObject> handlerobjToPtr =
            std::make_shared<BundleActiveReportHandlerObject>(tmpHandlerObject);
        reportHandler->SendEvent(BundleActiveReportHandler::MSG_REPORT_EVENT, handlerobjToPtr);
    }
}

void BundleActiveWindowVisibilityManager::UpdateAppVisibility(const std::string& bundleName,
    const std::string& abilityName, const std::string& moduleName, int32_t uid)
{
    bool wasVisible = false;
    bool isVisible = !IsAllAbilitiesInvisible(bundleName);

    {
        std::unique_lock<ffrt::mutex> lock(appVisibilityMapMutex_);
        auto appIter = appVisibilityMap_.find(bundleName);
        if (appIter != appVisibilityMap_.end()) {
            wasVisible = appIter->second;
        }
        if (isVisible) {
            appVisibilityMap_[bundleName] = isVisible;
        } else {
            appVisibilityMap_.erase(bundleName);
        }
    }

    if (!wasVisible && isVisible) {
        ReportAppStateChanged(bundleName, abilityName, moduleName, uid, true);
    } else if (wasVisible && !isVisible) {
        ReportAppStateChanged(bundleName, abilityName, moduleName, uid, false);
    }
}
} // namespace DeviceUsageStats
} // namespace OHOS
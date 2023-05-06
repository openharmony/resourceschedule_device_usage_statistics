/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "bundle_active_bundle_mgr_helper.h"

#include "accesstoken_kit.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"

#include "continuous_task_log.h"

namespace OHOS {
namespace DeviceUsageStats {
using OHOS::AppExecFwk::Constants::PERMISSION_GRANTED;

BundleActiveBundleMgrHelper::BundleActiveBundleMgrHelper()
{
}

BundleActiveBundleMgrHelper::~BundleActiveBundleMgrHelper()
{
}

void BundleActiveBundleMgrHelper::GetNameForUid(int32_t uid, std::string bundleName)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);
    if (!Connect()) {
        return;
    }
    if (!bundleMgr_) {
        return;
    }
    bundleMgr_->GetNameForUid(uid, bundleName);
    BUNDLE_ACTIVE_LOGD("get Bundle Name: %{public}s", bundleName.c_str());
}

bool BundleActiveBundleMgrHelper::GetApplicationInfo(const std::string &appName, const AppExecFwk::ApplicationFlag flag,
    const int userId, AppExecFwk::ApplicationInfo &appInfo)
{
    BUNDLE_ACTIVE_LOGD("start get application info");
    std::lock_guard<std::mutex> lock(connectionMutex_);

    if (!Connect()) {
        return false;
    }
    if (!bundleMgr_) {
        return false;
    }
    BUNDLE_ACTIVE_LOGD("bundleMgr is null: %{public}d ", bundleMgr_ == nullptr);
    if (bundleMgr_ != nullptr && bundleMgr_->GetApplicationInfo(appName, flag, userId, appInfo)) {
        return true;
    }
    return false;
}

bool BundleActiveBundleMgrHelper::GetBundleInfo(const std::string &bundleName, const AppExecFwk::BundleFlag flag,
    AppExecFwk::BundleInfo &bundleInfo, int32_t userId)
{
    std::lock_guard<std::mutex> lock(connectionMutex_);

    if (!Connect()) {
        return false;
    }
    if (!bundleMgr_) {
        return false;
    }
    if (bundleMgr_ != nullptr && bundleMgr_->GetBundleInfo(bundleName, flag, bundleInfo, userId)) {
        return true;
    }
    return false;
}

bool BundleActiveBundleMgrHelper::Connect()
{
    if (bundleMgr_ != nullptr) {
        return true;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        BUNDLE_ACTIVE_LOGE("get SystemAbilityManager failed");
        return false;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        BUNDLE_ACTIVE_LOGE("get Bundle Manager failed");
        return false;
    }

    bundleMgr_ = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    return bundleMgr_ ? true : false;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
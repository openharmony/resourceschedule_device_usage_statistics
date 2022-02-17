/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "bundle_active_client.h"

namespace OHOS {
namespace BundleActive {
BundleActiveClient& BundleActiveClient::GetInstance()
{
    static BundleActiveClient instance;
    return instance;
}

bool BundleActiveClient::GetBundleActiveProxy()
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        BUNDLE_ACTIVE_LOGE("Failed to get SystemAbilityManager.");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(BUNDLE_ACTIVE_SYS_ABILITY_ID);
    if (object == nullptr) {
        BUNDLE_ACTIVE_LOGE("Failed to get SystemAbility[1906] .");
        return false;
    }

    bundleActiveProxy = iface_cast<IBundleActiveService>(object);
    if (bundleActiveProxy == nullptr) {
        BUNDLE_ACTIVE_LOGE("Failed to get BundleActiveClient.");
        return false;
    }
    return true;
}

int BundleActiveClient::ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId,
    const int& userId, const int& eventId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy->ReportEvent(bundleName, abilityName, abilityId, userId, eventId);
}

int BundleActiveClient::IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId,
    const int& userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy->IsBundleIdle(bundleName, abilityName, abilityId, userId);
}

int BundleActiveClient::Query(std::string& bundleName, std::string& abilityName, const int& abilityId,
    const int& userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy->Query(bundleName, abilityName, abilityId, userId);
}
}
}

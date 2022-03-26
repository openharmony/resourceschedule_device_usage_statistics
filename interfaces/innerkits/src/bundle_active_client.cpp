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

#include "bundle_active_client.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveClient& BundleActiveClient::GetInstance()
{
    static BundleActiveClient instance;
    return instance;
}

bool BundleActiveClient::GetBundleActiveProxy()
{
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        BUNDLE_ACTIVE_LOGE("Failed to get SystemAbilityManager.");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID);
    if (!object) {
        BUNDLE_ACTIVE_LOGE("Failed to get SystemAbility[1920] .");
        return false;
    }

    bundleActiveProxy_ = iface_cast<IBundleActiveService>(object);
    if (!bundleActiveProxy_) {
        BUNDLE_ACTIVE_LOGE("Failed to get BundleActiveClient.");
        return false;
    }
    return true;
}

int BundleActiveClient::ReportFormClickedOrRemoved(const std::string& bundleName, const std::string& moduleName,
    const std::string modulePackage, const std::string& formName, const int64_t formId,
    const int32_t formDimension, const int userId, const int eventId)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveClient::ReportEvent called");
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->ReportFormClickedOrRemoved(bundleName, moduleName, modulePackage, formName, formId,
        formDimension, userId, eventId);
}

bool BundleActiveClient::IsBundleIdle(const std::string& bundleName)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->IsBundleIdle(bundleName);
}

std::vector<BundleActivePackageStats> BundleActiveClient::QueryPackageStats(const int intervalType,
    const int64_t beginTime, const int64_t endTime, int32_t& errCode)
{
    if (!GetBundleActiveProxy()) {
        return std::vector<BundleActivePackageStats>(0);
    }
    return bundleActiveProxy_->QueryPackageStats(intervalType, beginTime, endTime, errCode);
}

std::vector<BundleActiveEvent> BundleActiveClient::QueryEvents(const int64_t beginTime,
    const int64_t endTime, int32_t& errCode)
{
    if (!GetBundleActiveProxy()) {
        return std::vector<BundleActiveEvent>(0);
    }
    return bundleActiveProxy_->QueryEvents(beginTime, endTime, errCode);
}

void BundleActiveClient::SetBundleGroup(std::string bundleName, const int newGroup, const int userId)
{
    if (!GetBundleActiveProxy()) {
        return;
    }
    bundleActiveProxy_->SetBundleGroup(bundleName, newGroup, userId);
    return;
}

std::vector<BundleActivePackageStats> BundleActiveClient::QueryCurrentPackageStats(const int intervalType,
    const int64_t beginTime, const int64_t endTime)
{
    if (!GetBundleActiveProxy()) {
        return std::vector<BundleActivePackageStats>(0);
    }
    return bundleActiveProxy_->QueryCurrentPackageStats(intervalType, beginTime, endTime);
}

std::vector<BundleActiveEvent> BundleActiveClient::QueryCurrentEvents(const int64_t beginTime, const int64_t endTime)
{
    if (!GetBundleActiveProxy()) {
        return std::vector<BundleActiveEvent>(0);
    }
    return bundleActiveProxy_->QueryCurrentEvents(beginTime, endTime);
}

int BundleActiveClient::QueryPackageGroup()
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->QueryPackageGroup();
}

int BundleActiveClient::QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->QueryFormStatistics(maxNum, results);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


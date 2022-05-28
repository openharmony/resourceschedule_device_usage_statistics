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
namespace {
    const int32_t EVENTS_PARAM = 5;
    const int32_t PACKAGE_USAGE_PARAM = 6;
    const int32_t MODULE_USAGE_PARAM = 4;
}
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

int32_t BundleActiveClient::ReportEvent(BundleActiveEvent event, const int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveClient::ReportEvent called");
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->ReportEvent(event, userId);
}

bool BundleActiveClient::IsBundleIdle(const std::string& bundleName)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->IsBundleIdle(bundleName);
}

std::vector<BundleActivePackageStats> BundleActiveClient::QueryPackageStats(const int32_t intervalType,
    const int64_t beginTime, const int64_t endTime, int32_t& errCode, int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return std::vector<BundleActivePackageStats>(0);
    }
    return bundleActiveProxy_->QueryPackageStats(intervalType, beginTime, endTime, errCode, userId);
}

std::vector<BundleActiveEvent> BundleActiveClient::QueryEvents(const int64_t beginTime,
    const int64_t endTime, int32_t& errCode, int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return std::vector<BundleActiveEvent>(0);
    }
    return bundleActiveProxy_->QueryEvents(beginTime, endTime, errCode, userId);
}

void BundleActiveClient::SetBundleGroup(std::string bundleName, const int32_t newGroup, const int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return;
    }
    bundleActiveProxy_->SetBundleGroup(bundleName, newGroup, userId);
    return;
}

std::vector<BundleActivePackageStats> BundleActiveClient::QueryCurrentPackageStats(const int32_t intervalType,
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

int32_t BundleActiveClient::QueryPackageGroup()
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->QueryPackageGroup();
}

int32_t BundleActiveClient::QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
    int32_t userId)
{
    if (maxNum <= 0 || maxNum > MAXNUM_UP_LIMIT) {
        BUNDLE_ACTIVE_LOGI("maxNum is illegal, maxNum is %{public}d", maxNum);
        return -1;
    }
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->QueryFormStatistics(maxNum, results, userId);
}

int32_t BundleActiveClient::QueryEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->QueryEventStats(beginTime, endTime, eventStats, userId);
}

int32_t BundleActiveClient::QueryAppNotificationNumber(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->QueryAppNotificationNumber(beginTime, endTime, eventStats, userId);
}

int32_t BundleActiveClient::ShellDump(const std::vector<std::string> &dumpOption, std::vector<std::string> &dumpInfo)
{
    int32_t ret = -1;

    if (dumpOption[1] == "Events") {
        std::vector<BundleActiveEvent> eventResult;
        if (static_cast<int32_t>(dumpOption.size()) != EVENTS_PARAM) {
            return ret;
        }
        int64_t beginTime = std::stoll(dumpOption[2]);
        int64_t endTime = std::stoll(dumpOption[3]);
        int32_t userId = std::stoi(dumpOption[4]);
        eventResult = this->QueryEvents(beginTime, endTime, ret, userId);
        for (auto& oneEvent : eventResult) {
            dumpInfo.emplace_back(oneEvent.ToString());
        }
    } else if (dumpOption[1] == "PackageUsage") {
        std::vector<BundleActivePackageStats> packageUsageResult;
        if (static_cast<int32_t>(dumpOption.size()) != PACKAGE_USAGE_PARAM) {
            return ret;
        }
        int32_t intervalType = std::stoi(dumpOption[2]);
        int64_t beginTime = std::stoll(dumpOption[3]);
        int64_t endTime = std::stoll(dumpOption[4]);
        int32_t userId = std::stoi(dumpOption[5]);
        packageUsageResult = this->QueryPackageStats(intervalType, beginTime, endTime, ret, userId);
        for (auto& onePackageRecord : packageUsageResult) {
            dumpInfo.emplace_back(onePackageRecord.ToString());
        }
    } else if (dumpOption[1] == "ModuleUsage") {
        std::vector<BundleActiveModuleRecord> moduleResult;
        if (static_cast<int32_t>(dumpOption.size()) != MODULE_USAGE_PARAM) {
            return ret;
        }
        int32_t maxNum = std::stoi(dumpOption[2]);
        int32_t userId = std::stoi(dumpOption[3]);
        BUNDLE_ACTIVE_LOGI("M is %{public}d, u is %{public}d", maxNum, userId);
        ret = this->QueryFormStatistics(maxNum, moduleResult, userId);
        for (auto& oneModuleRecord : moduleResult) {
            dumpInfo.emplace_back(oneModuleRecord.ToString());
            for (uint32_t i = 0; i < oneModuleRecord.formRecords_.size(); i++) {
                std::string oneFormInfo = "form " + std::to_string(static_cast<int32_t>(i) + 1) + ", ";
                dumpInfo.emplace_back(oneFormInfo + oneModuleRecord.formRecords_[i].ToString());
            }
        }
    }

    return ret;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


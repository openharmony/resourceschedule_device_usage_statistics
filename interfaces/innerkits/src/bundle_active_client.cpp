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
    const std::string BUNDLE_ACTIVE_CLIENT_NAME = "bundleActiveName";
    static const int32_t DELAY_TIME = 5000;
}
BundleActiveClient& BundleActiveClient::GetInstance()
{
    static BundleActiveClient instance;
    return instance;
}

bool BundleActiveClient::GetBundleActiveProxy()
{
    if (bundleActiveProxy_ != nullptr) {
        return true;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgr) {
        BUNDLE_ACTIVE_LOGE("Failed to get SystemAbilityManager.");
        return false;
    }

    sptr<IRemoteObject> object = samgr->GetSystemAbility(DEVICE_USAGE_STATISTICS_SYS_ABILITY_ID);
    if (!object) {
        BUNDLE_ACTIVE_LOGE("Failed to get SystemAbility[1907] .");
        return false;
    }

    bundleActiveProxy_ = iface_cast<IBundleActiveService>(object);
    if (!bundleActiveProxy_) {
        BUNDLE_ACTIVE_LOGE("Failed to get BundleActiveProxy.");
        return false;
    }
    if (!recipient_) {
        recipient_ = new (std::nothrow) BundleActiveClientDeathRecipient();
    }
    if (recipient_) {
        bundleActiveProxy_->AsObject()->AddDeathRecipient(recipient_);
    }

    bundleClientRunner_ = AppExecFwk::EventRunner::Create(BUNDLE_ACTIVE_CLIENT_NAME);
    if (!bundleClientRunner_) {
        BUNDLE_ACTIVE_LOGE("BundleActiveClient runner create failed!");
        return false;
    }
    bundleClientHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(bundleClientRunner_);
    if (!bundleClientHandler_) {
        BUNDLE_ACTIVE_LOGE("BundleActiveClient handler create failed!");
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

bool BundleActiveClient::IsBundleIdle(const std::string& bundleName, int32_t& errCode, int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->IsBundleIdle(bundleName, errCode, userId);
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

int32_t BundleActiveClient::SetBundleGroup(std::string bundleName, const int32_t newGroup,
    int32_t errCode, int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    return bundleActiveProxy_->SetBundleGroup(bundleName, newGroup, errCode, userId);
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

int32_t BundleActiveClient::QueryPackageGroup(std::string& bundleName, const int32_t userId)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    int32_t result = bundleActiveProxy_->QueryPackageGroup(bundleName, userId);
    return result;
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

int32_t BundleActiveClient::RegisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    int32_t result = bundleActiveProxy_->RegisterGroupCallBack(observer);
    if (recipient_ && result == ERR_OK) {
        recipient_->SetObserver(observer);
    }
    return result;
}

int32_t BundleActiveClient::UnregisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer)
{
    if (!GetBundleActiveProxy()) {
        return -1;
    }
    int32_t result = bundleActiveProxy_->UnregisterGroupCallBack(observer);
    if (recipient_ && result == ERR_OK) {
        recipient_->RemoveObserver();
    }
    return result;
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

void BundleActiveClient::BundleActiveClientDeathRecipient::SetObserver(const sptr<IBundleActiveGroupCallback> &observer)
{
    if (observer) {
        observer_ = observer;
    }
}

void BundleActiveClient::BundleActiveClientDeathRecipient::RemoveObserver()
{
    if (observer_) {
        observer_ = nullptr;
    }
}

void BundleActiveClient::BundleActiveClientDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &object)
{
    if (object == nullptr) {
        BUNDLE_ACTIVE_LOGE("remote object is null.");
        return;
    }
    BundleActiveClient::GetInstance().bundleActiveProxy_ = nullptr;
    BundleActiveClient::GetInstance().bundleClientHandler_->PostTask([this, &object]() {
            this->OnServiceDiedInner(object);
        },
        DELAY_TIME);
}

void BundleActiveClient::BundleActiveClientDeathRecipient::OnServiceDiedInner(const wptr<IRemoteObject> &object)
{
    while (!BundleActiveClient::GetInstance().GetBundleActiveProxy()) { }
    if (observer_) {
        BundleActiveClient::GetInstance().RegisterGroupCallBack(observer_);
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


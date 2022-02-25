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

#ifndef BUNDLE_ACTIVE_PROXY_H
#define BUNDLE_ACTIVE_PROXY_H

#include "ibundle_active_service.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveProxy : public IRemoteProxy<IBundleActiveService> {
public:
    int ReportEvent(std::string& bundleName, std::string& abilityName, std::string abilityId,
        const std::string& continuousTask, const int& userId, const int& eventId) override;
    bool IsBundleIdle(const std::string& bundleName) override;
    std::vector<BundleActivePackageStats> QueryPackageStats(const int& intervalType, const int64_t& beginTime,
        const int64_t& endTime) override;
    std::vector<BundleActiveEvent>  QueryEvents(const int64_t& beginTime, const int64_t& endTime) override;
    void SetBundleGroup(const std::string& bundleName, int newGroup, int userId) override;
    std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int& intervalType, const int64_t& beginTime,
        const int64_t& endTime) override;
    std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t& beginTime, const int64_t& endTime) override;
    int QueryPackageGroup() override;
    explicit BundleActiveProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IBundleActiveService>(impl) {}
    virtual ~BundleActiveProxy() {}

private:
    static inline BrokerDelegator<BundleActiveProxy> delegator_;
};
}
}
#endif

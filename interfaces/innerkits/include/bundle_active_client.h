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

#ifndef BUNDLE_ACTIVE_CLIENT_H
#define BUNDLE_ACTIVE_CLIENT_H

#include "ibundle_active_service.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveClient {
public:
    int ReportEvent(std::string& bundleName, std::string& abilityName, std::string abilityId,
        const std::string& continuousTask, const int userId, const int eventId);
    bool IsBundleIdle(const std::string& bundleName);
    std::vector<BundleActivePackageStats> QueryPackageStats(const int intervalType, const int64_t beginTime,
        const int64_t endTime, int32_t& errCode);
    std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime, int32_t& errCode);
    void SetBundleGroup(std::string bundleName, const int newGroup, const int userId);
    std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int intervalType, const int64_t beginTime,
        const int64_t endTime);
    std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t beginTime, const int64_t endTime);
    int QueryPackageGroup();
    static BundleActiveClient& GetInstance();
    BundleActiveClient() {}
    ~BundleActiveClient() {};

private:
    bool GetBundleActiveProxy();
    sptr<IBundleActiveService> bundleActiveProxy_;
};
}
}
#endif
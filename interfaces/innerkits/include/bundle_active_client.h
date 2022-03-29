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
#include "bundle_active_package_stats.h"
#include "bundle_active_module_record.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveClient {
public:
    /*
    * function: ReportEvent, used to report event.
    * parameters: event, userId
    * return: errorcode.
    */
    int ReportEvent(BundleActiveEvent event, const int userId);
    /*
    * function: IsBundleIdle, used to check whether specific bundle is idle.
    * parameters: bundleName
    * return: if bundle is idle, return true. if bundle is not idle, return false.
    */
    bool IsBundleIdle(const std::string& bundleName);
    /*
    * function: QueryPackageStats, query all bundle usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryPackageStats(const int intervalType, const int64_t beginTime,
        const int64_t endTime, int32_t& errCode);
    /*
    * function: QueryEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: vector of events.
    */
    std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime, int32_t& errCode);
    /*
    * function: SetBundleGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId
    */
    void SetBundleGroup(std::string bundleName, const int newGroup, const int userId);
    /*
    * function: QueryCurrentPackageStats, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of calling bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int intervalType, const int64_t beginTime,
        const int64_t endTime);
    /*
    * function: QueryCurrentEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime, errCode
    * return: vector of calling bundle events.
    */
    std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t beginTime, const int64_t endTime);
    /*
    * function: QueryPackageGroup, query bundle priority group calling bundle.
    * return: the priority group of calling bundle.
    */
    int QueryPackageGroup();
    /*
    * function: QueryFormStatistics, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum
    * return: errorcode.
    */
    int QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results);
    /*
    * function: GetInstance, get instance of client.
    * return: object of BundleActiveClient.
    */
    static BundleActiveClient& GetInstance();
    /*
    * function: BundleActiveClient, default constructor.
    */
    BundleActiveClient() {}
    /*
    * function: ~BundleActiveClient, default destructor.
    */
    ~BundleActiveClient() {}

private:
    bool GetBundleActiveProxy();
    sptr<IBundleActiveService> bundleActiveProxy_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CLIENT_H


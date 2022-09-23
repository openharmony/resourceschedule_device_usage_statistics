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

#include <mutex>

#include "ibundle_active_service.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_module_record.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveClient {
public:
    // max number of query modules result.
    const int32_t MAXNUM_UP_LIMIT = 1000;
    /*
    * function: ReportEvent, used to report event.
    * parameters: event, userId
    * return: errorcode.
    */
    int32_t ReportEvent(BundleActiveEvent event, const int32_t userId);
    /*
    * function: IsBundleIdle, used to check whether specific bundle is idle.
    * parameters: bundleName
    * return: if bundle is idle, return true. if bundle is not idle, return false.
    */
    bool IsBundleIdle(const std::string& bundleName, int32_t& errCode, int32_t userId = -1);
    /*
    * function: QueryPackageStats, query all bundle usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryPackageStats(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime, int32_t& errCode, int32_t userId = -1);
    /*
    * function: QueryEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: vector of events.
    */
    std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime, int32_t userId = -1);
    /*
    * function: SetBundleGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId
    * return : void
    */
    int32_t SetBundleGroup(std::string bundleName, const int32_t newGroup, int32_t userId = -1);
    /*
    * function: QueryCurrentPackageStats, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime
    * return: vector of calling bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime);
    /*
    * function: QueryCurrentEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime
    * return: vector of calling bundle events.
    */
    std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t beginTime, const int64_t endTime);
    /*
    * function: QueryPackageGroup, query bundle priority group calling bundle.
    * parameters: bundleName,userId
    * return: the priority group of calling bundle.
    */
    int32_t QueryPackageGroup(std::string& bundleName, const int32_t userId = -1);
    /*
    * function: QueryFormStatistics, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum, results, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    int32_t QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results, int32_t userId = -1);
    /*
    * function: observe bundle group change event
    * parameters: observer
    * return: errorcode.
    */
    int32_t RegisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer);
    /*
    * function: unobserve bundle group change event
    * parameters: observer
    * return: errorcode.
    */
    int32_t UnregisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer);

    /*
    * function: QueryEventStats, query all from event stats in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    int32_t QueryEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId = -1);

    /*
    * function: QueryAppNotificationNumber, query all app notification number in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    int32_t QueryAppNotificationNumber(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId = -1);
    /*
    * function: GetInstance, get instance of client.
    * return: object of BundleActiveClient.
    */
    static BundleActiveClient& GetInstance();
private:
    class BundleActiveClientDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        /*
        * function: BundleActiveClientDeathRecipient, default constructor.
        */
        BundleActiveClientDeathRecipient() = default;

        /*
        * function: ~BundleActiveClientDeathRecipient, default destructor.
        */
        ~BundleActiveClientDeathRecipient() = default;

        /*
        * function: AddObserver.
        */
        void AddObserver(const sptr<IBundleActiveGroupCallback> &observer);

        /*
        * function: RemoveObserver.
        */
        void RemoveObserver();

        /*
        * function: OnRemoteDied, PostTask when service(bundleActiveProxy_) is died.
        */
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

        /*
        * function: OnServiceDiedInner, get bundleActiveProxy_ and registerGroupCallBack again.
        */
        void OnServiceDiedInner(const wptr<IRemoteObject> &object);

    private:
        sptr<IBundleActiveGroupCallback> observer_ = nullptr;
    };
private:
    bool GetBundleActiveProxy();
    BundleActiveClient() {}
    ~BundleActiveClient() {}
    sptr<IBundleActiveService> bundleActiveProxy_;
    sptr<BundleActiveClientDeathRecipient> recipient_;
    std::shared_ptr<AppExecFwk::EventRunner> bundleClientRunner_ {nullptr};
    std::shared_ptr<AppExecFwk::EventHandler> bundleClientHandler_ {nullptr};
    std::mutex mutex_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CLIENT_H


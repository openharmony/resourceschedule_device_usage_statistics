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
    * function: QueryBundleStatsInfoByInterval, query all bundle usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryBundleStatsInfoByInterval(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime, int32_t& errCode, int32_t userId = -1);

    /*
    * function: QueryBundleEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: vector of events.
    */
    std::vector<BundleActiveEvent> QueryBundleEvents(const int64_t beginTime, const int64_t endTime, int32_t& errCode,
        int32_t userId = -1);

    /*
    * function: SetAppGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId
    * return : void
    */
    int32_t SetAppGroup(std::string bundleName, const int32_t newGroup, int32_t userId = -1);

    /*
    * function: QueryBundleStatsInfos, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime
    * return: vector of calling bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryBundleStatsInfos(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime);

    /*
    * function: QueryCurrentBundleEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime
    * return: vector of calling bundle events.
    */
    std::vector<BundleActiveEvent> QueryCurrentBundleEvents(const int64_t beginTime, const int64_t endTime);

    /*
    * function: QueryAppGroup, query bundle priority group calling bundle.
    * parameters: bundleName,userId
    * return: the priority group of calling bundle.
    */
    int32_t QueryAppGroup(std::string& bundleName, const int32_t userId = -1);

    /*
    * function: QueryModuleUsageRecords, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum, results, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    int32_t QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results, int32_t userId = -1);

    /*
    * function: observe bundle group change event
    * parameters: observer
    * return: errorcode.
    */
    int32_t RegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer);

    /*
    * function: unobserve bundle group change event
    * parameters: observer
    * return: errorcode.
    */
    int32_t UnRegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer);

    /*
    * function: QueryDeviceEventStates, query all from event stats in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    int32_t QueryDeviceEventStates(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId = -1);

    /*
    * function: QueryNotificationNumber, query all app notification number in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    int32_t QueryNotificationNumber(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId = -1);
    /*
    * function: GetInstance, get single instance of client.
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
        void AddObserver(const sptr<IAppGroupCallback> &observer);

        /*
        * function: RemoveObserver.
        */
        void RemoveObserver();

        /*
        * function: OnRemoteDied, PostTask when service(bundleActiveProxy_) is died.
        */
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

        /*
        * function: OnServiceDiedInner, get bundleActiveProxy_ and RegisterAppGroupCallBack again.
        */
        void OnServiceDiedInner(const wptr<IRemoteObject> &object);

    private:
        sptr<IAppGroupCallback> observer_ = nullptr;
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


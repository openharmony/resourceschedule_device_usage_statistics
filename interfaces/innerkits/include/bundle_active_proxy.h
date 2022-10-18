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
#include "bundle_active_event_stats.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_module_record.h"
#include "bundle_state_inner_errors.h"
#include "iapp_group_callback.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveProxy : public IRemoteProxy<IBundleActiveService> {
public:
    /*
    * function: ReportEvent, used to report event.
    * parameters: event, userId
    * return: errorcode.
    */
    ErrCode ReportEvent(BundleActiveEvent& event, const int32_t userId) override;

    /*
    * function: IsBundleIdle, used to check whether specific bundle is idle.
    * parameters: bundleName
    * return: if bundle is idle, return true. if bundle is not idle, return false.
    */
    ErrCode IsBundleIdle(bool& isBundleIdle, const std::string& bundleName, int32_t userId = -1) override;

    /*
    * function: QueryBundleStatsInfoByInterval, query all usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    ErrCode QueryBundleStatsInfoByInterval(std::vector<BundleActivePackageStats>& PackageStats,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime, int32_t userId) override;

    /*
    * function: QueryBundleEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: errorcode.
    */
    ErrCode QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents, const int64_t beginTime,
        const int64_t endTime, int32_t userId) override;

    /*
    * function: SetAppGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId.
    * return: errorcode.
    */
    ErrCode SetAppGroup(const std::string& bundleName, int32_t newGroup, int32_t userId) override;

    /*
    * function: QueryBundleStatsInfos, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime
    * return: errCode.
    */
    ErrCode QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& bundleActivePackageStats,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime) override;

    /*
    * function: QueryCurrentBundleEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime
    * return: errCode.
    */
    ErrCode QueryCurrentBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents, const int64_t beginTime,
        const int64_t endTime) override;

    /*
    * function: QueryAppGroup, query bundle priority group calling bundle.
    * return: the priority group of calling bundle.
    * return: errorcode.
    */
    ErrCode QueryAppGroup(int32_t& appGroup, std::string& bundleName, const int32_t userId) override;

    /*
    * function: QueryModuleUsageRecords, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum, results, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
        int32_t userId = -1) override;

    /*
    * function: QueryDeviceEventStats, query all from event stats in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId) override;

    /*
    * function: QueryNotificationEventStats, query all app notification number in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId) override;

    /*
    * function: BundleActiveProxy, default constructor.
    * parameters: impl
    */
    explicit BundleActiveProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IBundleActiveService>(impl) {}

    /*
    * function: RegisterAppGroupCallBack, register the observer to groupObservers.
    * parameters: observer.
    * return: errorcode.
    */
    ErrCode RegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer) override;

    /*
    * function: UnRegisterAppGroupCallBack, remove the observer from groupObservers.
    * parameters: observer.
    * return: errorcode.
    */
    ErrCode UnRegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer) override;

    /*
    * function: ~BundleActiveProxy, default destructor.
    */
    virtual ~BundleActiveProxy() {}

private:
    static inline BrokerDelegator<BundleActiveProxy> delegator_;
    ErrCode IPCCommunication(int64_t beginTime, int64_t endTime, std::vector<BundleActiveEventStats>& eventStats,
        int32_t userId, int32_t communicationFlag);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_PROXY_H


/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
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

#ifndef BUNDLE_ACTIVE_ISERVICE_H
#define BUNDLE_ACTIVE_ISERVICE_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <stdint.h>

#include "iremote_broker.h"
#include "iremote_stub.h"
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "ipc_skeleton.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "ibundle_active_group_callback.h"
#include "bundle_active_group_callback_proxy.h"

#include "bundle_active_log.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActivePackageStats;
class BundleActiveEvent;
class BundleActiveEventStats;
class BundleActiveModuleRecord;

class IBundleActiveService : public IRemoteBroker {
public:
    IBundleActiveService() = default;
    ~IBundleActiveService() override = default;
    DISALLOW_COPY_AND_MOVE(IBundleActiveService);
    /*
    * function: ReportEvent, used to report event.
    * parameters: event, userId
    * return: errorcode.
    */
    virtual int32_t ReportEvent(BundleActiveEvent& event, const int32_t userId) = 0;
    /*
    * function: IsBundleIdle, used to check whether specific bundle is idle.
    * parameters: bundleName
    * return: if bundle is idle, return true. if bundle is not idle, return false.
    */
    virtual bool IsBundleIdle(const std::string& bundleName, int32_t& errCode, int32_t userId) = 0;
    /*
    * function: QueryPackageStats, query all bundle usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    virtual std::vector<BundleActivePackageStats> QueryPackageStats(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime, int32_t& errCode, int32_t userId) = 0;
    /*
    * function: QueryEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: vector of events.
    */
    virtual std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime,
        int32_t& errCode, int32_t userId) = 0;
    /*
    * function: QueryCurrentPackageStats, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime
    * return: vector of calling bundle usage statistics.
    */
    virtual std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int32_t intervalType,
        const int64_t beginTime, const int64_t endTime) = 0;
    /*
    * function: QueryCurrentEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime
    * return: vector of calling bundle events.
    */
    virtual std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t beginTime, const int64_t endTime) = 0;
    /*
    * function: QueryPackageGroup, query bundle priority group calling bundle.
    * return: the priority group of calling bundle.
    */
    virtual int32_t QueryPackageGroup(std::string& bundleName, int32_t userId) = 0;
    /*
    * function: SetBundleGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId
    */
    virtual int32_t SetBundleGroup(const std::string& bundleName, int32_t newGroup,
        int32_t errCode, int32_t userId) = 0;
    /*
    * function: QueryFormStatistics, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum, results, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    virtual int32_t QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
        int32_t userId) = 0;

    virtual int32_t RegisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer) = 0;
    virtual int32_t UnregisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer) = 0;

    /*
    * function: QueryEventStats, query all from event stats in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    virtual int32_t QueryEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId) = 0;

    /*
    * function: QueryAppNotificationNumber, query all app notification number in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    virtual int32_t QueryAppNotificationNumber(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId) = 0;
public:
    enum {
        REPORT_EVENT = 1,
        IS_BUNDLE_IDLE = 2,
        QUERY_USAGE_STATS = 3,
        QUERY_EVENTS = 4,
        QUERY_CURRENT_USAGE_STATS = 5,
        QUERY_CURRENT_EVENTS = 6,
        QUERY_BUNDLE_GROUP = 7,
        SET_BUNDLE_GROUP = 8,
        QUERY_FORM_STATS = 9,
        REGISTER_GROUP_CALLBACK = 10,
        UNREGISTER_GROUP_CALLBACK = 11,
        QUERY_EVENT_STATS = 12,
        QUERY_APP_NOTIFICATION_NUMBER = 13
    };
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"Resourceschedule.IBundleActiveService");
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_ISERVICE_H


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

#include "bundle_active_log.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActivePackageStats;
class BundleActiveEvent;
class IBundleActiveService : public IRemoteBroker {
public:
    virtual int ReportEvent(std::string& bundleName, std::string& abilityName, std::string abilityId,
        const std::string& continuousTask, const int& userId, const int& eventId) = 0;
    virtual bool IsBundleIdle(const std::string& bundleName) = 0;
    virtual std::vector<BundleActivePackageStats> QueryPackageStats(const int& intervalType, const int64_t& beginTime,
        const int64_t& endTime) = 0;
    virtual std::vector<BundleActiveEvent> QueryEvents(const int64_t& beginTime, const int64_t& endTime) = 0;
    virtual std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int& intervalType,
        const int64_t& beginTime, const int64_t& endTime) = 0;
    virtual std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t& beginTime, const int64_t& endTime) = 0;
    virtual int QueryPackageGroup() = 0;
    virtual void SetBundleGroup(const std::string& bundleName, int newGroup, int userId) = 0;

public:
    enum {
        REPORT_EVENT = 1,
        IS_BUNDLE_IDLE = 2,
        QUERY_USAGE_STATS = 3,
        QUERY_EVENTS = 4,
        QUERY_CURRENT_USAGE_STATS = 5,
        QUERY_CURRENT_EVENTS = 6,
        QUERY_BUNDLE_GROUP = 7,
        SET_BUNDLE_GROUP = 8
    };
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"Resourceschedule.IBundleActiveService");
};
}
}
#endif
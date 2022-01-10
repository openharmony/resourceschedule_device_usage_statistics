/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "bundle_active_log.h"
#include "iremote_broker.h"
#include "iremote_stub.h"
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "system_ability.h"
#include "system_ability_definition.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace BundleActive {
    
class IBundleActiveService : public IRemoteBroker {
public:
    virtual int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) = 0;
    virtual int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) = 0;
    virtual int Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) = 0;

public:
    enum {
        REPORT_EVENT = 1,
        IS_BUNDLE_IDLE = 2,
        QUERY = 3,
    };
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"Resourceschedule.IBundleActiveService");
};
}
}

#endif
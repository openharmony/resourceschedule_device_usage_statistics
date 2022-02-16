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

#include "bundle_active_service.h"

namespace OHOS {
namespace BundleActive {
REGISTER_SYSTEM_ABILITY_BY_ID(BundleActiveService, BUNDLE_ACTIVE_SYS_ABILITY_ID, true);

void BundleActiveService::OnStart()
{
    int ret = Publish(this);
    if (!ret) {
        BUNDLE_ACTIVE_LOGE("[Server] OnStart, Register SystemAbility[1906] FAIL.");
        return;
    }
    BUNDLE_ACTIVE_LOGI("[Server] OnStart, Register SystemAbility[1906] SUCCESS.");
}

void BundleActiveService::OnStop()
{
    BUNDLE_ACTIVE_LOGI("[Server] OnStop");
}

int BundleActiveService::ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId)
{
    BUNDLE_ACTIVE_LOGI("Report event called");
    return 0;
}

int BundleActiveService::IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId)
{
    BUNDLE_ACTIVE_LOGI("Is bundle active called");
    return true;
}

int BundleActiveService::Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId)
{
    BUNDLE_ACTIVE_LOGI("Query called");
    return 0;
}
}
}
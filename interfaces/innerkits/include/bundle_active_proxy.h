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

#ifndef BUNDLE_ACTIVE_PROXY_H
#define BUNDLE_ACTIVE_PROXY_H

#include "bundle_active_iservice.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveProxy : public IRemoteProxy<IBundleActiveService> {
public:
    int ReportEvent(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId, const int& eventId) override;
    int IsBundleIdle(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) override;
    int Query(std::string& bundleName, std::string& abilityName, const int& abilityId, const int& userId) override;

public:
    explicit BundleActiveProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<IBundleActiveService>(impl) {}
    virtual ~BundleActiveProxy() {}
    
private:
    static inline BrokerDelegator<BundleActiveProxy> delegator_;
};

}
}

#endif

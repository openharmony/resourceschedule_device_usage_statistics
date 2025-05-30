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

#ifndef BUNDLE_ACTIVE_COMMON_EVENT_SUBSCRIBER_H
#define BUNDLE_ACTIVE_COMMON_EVENT_SUBSCRIBER_H

#include <memory>

#include "common_event_manager.h"
#include "common_event_support.h"
#include "common_event_data.h"
#include "time_service_client.h"

#include "ibundle_active_service.h"
#include "ffrt.h"

namespace OHOS {
namespace DeviceUsageStats {
using CommonEventSubscriber = OHOS::EventFwk::CommonEventSubscriber;
using CommonEventData = OHOS::EventFwk::CommonEventData;
using CommonEventSubscribeInfo = OHOS::EventFwk::CommonEventSubscribeInfo;
using CommonEventSupport = OHOS::EventFwk::CommonEventSupport;
using CommonEventManager = OHOS::EventFwk::CommonEventManager;
using MatchingSkills = OHOS::EventFwk::MatchingSkills;
class BundleActiveGroupController;

class BundleActiveCommonEventSubscriber : public CommonEventSubscriber {
public:
    BundleActiveCommonEventSubscriber(
        const CommonEventSubscribeInfo &subscriberInfo) : CommonEventSubscriber(subscriberInfo) {}
    ~BundleActiveCommonEventSubscriber() = default;
    void OnReceiveEvent(const CommonEventData &data) override;

private:
    void HandleScreenEvent();
    void HandleUserRemoveEvent(const CommonEventData &data);
    void HandleUserSwitchEvent(const CommonEventData &data);
    void HandlePackageRemoveEvent(const CommonEventData &data);
    void HandlePackageAddEvent(const CommonEventData &data);
    void HandleLockEvent(const CommonEventData &data);
    ffrt::mutex mutex_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_COMMON_EVENT_SUBSCRIBER_H


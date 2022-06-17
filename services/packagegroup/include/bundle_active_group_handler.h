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

#ifndef BUNDLE_ACTIVE_GROUP_HANDLER_H
#define BUNDLE_ACTIVE_GROUP_HANDLER_H

#include "event_handler.h"
#include "event_runner.h"

#include "ibundle_active_service.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_group_common.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveGroupHandlerObject {
public:
    std::string bundleName_;
    int32_t userId_;
    BundleActiveGroupHandlerObject();
    BundleActiveGroupHandlerObject(const BundleActiveGroupHandlerObject& orig);
    ~BundleActiveGroupHandlerObject() {}
};

class BundleActiveGroupHandler : public AppExecFwk::EventHandler {
public:
    explicit BundleActiveGroupHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner, const bool debug);
    ~BundleActiveGroupHandler() = default;
        /**
     * Process the event. Developers should override this method.
     *
     * @param event The event should be processed.
     */
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    void Init(const std::shared_ptr<BundleActiveGroupController>& bundleActiveController);
    static const int32_t MSG_CHECK_BUNDLE_STATE = 0;
    static const int32_t MSG_ONE_TIME_CHECK_BUNDLE_STATE = 1;
    static const int32_t MSG_CHECK_IDLE_STATE = 2;
    int64_t checkIdleInterval_;

private:
    std::shared_ptr<BundleActiveGroupController> bundleActiveGroupController_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_GROUP_HANDLER_H


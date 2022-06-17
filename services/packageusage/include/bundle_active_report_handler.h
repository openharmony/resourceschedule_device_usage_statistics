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

#ifndef BUNDLE_ACTIVE_REPORT_HANDLER_H
#define BUNDLE_ACTIVE_REPORT_HANDLER_H

#include "event_handler.h"
#include "event_runner.h"

#include "ibundle_active_service.h"
#include "bundle_active_core.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveReportHandler : public AppExecFwk::EventHandler {
public:
    explicit BundleActiveReportHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner);
    ~BundleActiveReportHandler() = default;
        /**
     * Process the event. Developers should override this method.
     *
     * @param event The event should be processed.
     */
    void ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event) override;
    void Init(const std::shared_ptr<BundleActiveCore>& bundleActiveCore);
    static const int32_t MSG_REPORT_EVENT = 0;
    static const int32_t MSG_FLUSH_TO_DISK = 1;
    static const int32_t MSG_REMOVE_USER = 2;
    static const int32_t MSG_BUNDLE_UNINSTALLED = 3;
    static const int32_t MSG_SWITCH_USER = 4;

private:
    std::shared_ptr<BundleActiveCore> bundleActiveCore_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_REPORT_HANDLER_H


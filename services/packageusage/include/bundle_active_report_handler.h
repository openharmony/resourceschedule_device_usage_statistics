/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "ffrt.h"
#include <map>
#include <queue>

#include "ibundle_active_service.h"
#include "bundle_active_core.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveReportHandler : public std::enable_shared_from_this<BundleActiveReportHandler> {
public:
    BundleActiveReportHandler() = default;
    ~BundleActiveReportHandler() = default;
        /**
     * Process the event. Developers should override this method.
     *
     * @param eventId The event id
     * @param handlerobj The eventobj
     */
    void ProcessEvent(const int32_t& eventId, const std::shared_ptr<BundleActiveReportHandlerObject>& handlerobj);
    void SendEvent(const int32_t& eventId,
        const std::shared_ptr<BundleActiveReportHandlerObject>& handlerobj, const int64_t& delayTime = 0);
    void RemoveEvent(const int32_t& eventId);
    bool HasEvent(const int32_t& eventId);
    void Init(const std::shared_ptr<BundleActiveCore>& bundleActiveCore);
    static const int32_t MSG_REPORT_EVENT;
    static const int32_t MSG_FLUSH_TO_DISK;
    static const int32_t MSG_REMOVE_USER;
    static const int32_t MSG_BUNDLE_UNINSTALLED;
    static const int32_t MSG_SWITCH_USER;
    static const int32_t MSG_BUNDLE_INSTALLED;

private:
    void ProcessReportEvent(BundleActiveReportHandlerObject& tmpHandlerobj);
    void ProcessFlushToDiskEvent(const BundleActiveReportHandlerObject& tmpHandlerobj);
    void ProcessRmoveUserEvent(const BundleActiveReportHandlerObject& tmpHandlerobj);
    void ProcessUserSwitchEvent(const BundleActiveReportHandlerObject& tmpHandlerobj);
    void ProcessBundleUninstallEvent(const BundleActiveReportHandlerObject& tmpHandlerobj);
    void ProcessBundleInstallEvent(const BundleActiveReportHandlerObject& tmpHandlerobj);
    bool isInited_ = false;
    ffrt::mutex taskHandlerMutex_;
    std::shared_ptr<ffrt::queue> ffrtQueue_;
    std::map<int32_t, std::queue<ffrt::task_handle>> taskHandlerMap_;
    std::shared_ptr<BundleActiveCore> bundleActiveCore_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_REPORT_HANDLER_H


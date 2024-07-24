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

#ifndef BUNDLE_ACTIVE_GROUP_HANDLER_H
#define BUNDLE_ACTIVE_GROUP_HANDLER_H

#include "event_handler.h"
#include "event_runner.h"
#include "ffrt.h"
#include <map>
#include "ibundle_active_service.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_group_common.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveGroupHandlerObject {
public:
    std::string bundleName_;
    int32_t userId_;
    int32_t uid_;
    BundleActiveGroupHandlerObject();
    BundleActiveGroupHandlerObject(const BundleActiveGroupHandlerObject& orig);
    ~BundleActiveGroupHandlerObject() {}
};

class BundleActiveGroupHandler : public std::enable_shared_from_this<BundleActiveGroupHandler> {
public:
    explicit BundleActiveGroupHandler(const bool debug);
    ~BundleActiveGroupHandler() = default;
        /**
     * Process the event. Developers should override this method.
     *
     * @param event The event should be processed.
     */
    void ProcessEvent(int32_t eventId, std::shared_ptr<BundleActiveGroupHandlerObject> handlerobj);
    void SendEvent(const int32_t& eventId, const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj,
        const int32_t& delayTime = 0);
    void SendCheckBundleMsg(const int32_t& eventId, const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj,
        const int32_t& delayTime = 0);
    std::string GetMsgKey(const int32_t& eventId, const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj,
        const int32_t& delayTime);
    void RemoveEvent(const int32_t& eventId);
    void RemoveCheckBundleMsg(const std::string& msgKey);
    void PostSyncTask(const std::function<void()>& fuc);
    void PostTask(const std::function<void()>& fuc);
    void Init(const std::shared_ptr<BundleActiveGroupController>& bundleActiveController);
    static const int32_t MSG_CHECK_DEFAULT_BUNDLE_STATE = 0;
    static const int32_t MSG_ONE_TIME_CHECK_BUNDLE_STATE = 1;
    static const int32_t MSG_CHECK_IDLE_STATE = 2;
    static const int32_t MSG_CHECK_NOTIFICATION_SEEN_BUNDLE_STATE = 3;
    static const int32_t MSG_CHECK_SYSTEM_INTERACTIVE_BUNDLE_STATE = 4;
    int64_t checkIdleInterval_;

private:
    bool isInited_ = false;
    std::shared_ptr<ffrt::queue> ffrtQueue_;
    std::map<int32_t, ffrt::task_handle> taskHandlerMap_;
    std::map<std::string, ffrt::task_handle> checkBundleTaskMap_;
    std::shared_ptr<BundleActiveGroupController> bundleActiveGroupController_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_GROUP_HANDLER_H


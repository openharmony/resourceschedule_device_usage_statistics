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

#ifndef BUNDLE_ACTIVE_CONTINUOUS_TASK_OBSERVER_H
#define BUNDLE_ACTIVE_CONTINUOUS_TASK_OBSERVER_H

#ifdef BGTASKMGR_ENABLE
#include <memory>
#ifdef OS_ACCOUNT_PART_ENABLED
#include "os_account_manager.h"
#endif // OS_ACCOUNT_PART_ENABLED
#include "bundle_mgr_interface.h"
#include "iremote_object.h"

#include "bundle_active_report_handler.h"
#include "background_task_mgr_helper.h"
#include "background_task_subscriber.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveContinuousTaskObserver : public OHOS::BackgroundTaskMgr::BackgroundTaskSubscriber {
public:
    using IBundleMgr = OHOS::AppExecFwk::IBundleMgr;
    void OnContinuousTaskStart(const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>
        &continuousTaskCallbackInfo) override;
    void OnContinuousTaskStop(const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>
        &continuousTaskCallbackInfo) override;
    void OnRemoteDied(const wptr<IRemoteObject> &object) override;
    void Init(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    std::atomic<bool> isRemoteDied_ {false};
private:
    sptr<IBundleMgr> bundleMgr_;
    std::shared_ptr<BundleActiveReportHandler> reportHandler_;
    bool GetBundleMgr();
    void ReportContinuousTaskEvent(const std::shared_ptr<OHOS::BackgroundTaskMgr::ContinuousTaskCallbackInfo>&
        continuousTaskCallbackInfo, const bool isStart);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif
#endif  // BUNDLE_ACTIVE_CONTINUOUS_TASK_OBSERVER_H


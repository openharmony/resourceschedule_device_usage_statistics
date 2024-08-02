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

#include "bundle_active_log.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveReportHandler::BundleActiveReportHandler
    (const std::shared_ptr<AppExecFwk::EventRunner> &runner) : AppExecFwk::EventHandler(runner)
{
}

void BundleActiveReportHandler::Init(const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    bundleActiveCore_ = bundleActiveCore;
}

void BundleActiveReportHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        BUNDLE_ACTIVE_LOGE("event is null, exit ProcessEvent");
        return;
    }
    auto ptrToHandlerobj = event->GetSharedObject<BundleActiveReportHandlerObject>();
    switch (event->GetInnerEventId()) {
        case MSG_REPORT_EVENT: {
            BUNDLE_ACTIVE_LOGD("MSG_REPORT_EVENT CALLED");
            BundleActiveReportHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            bundleActiveCore_->ReportEvent(tmpHandlerobj.event_, tmpHandlerobj.userId_);
            break;
        }
        case MSG_FLUSH_TO_DISK: {
            BUNDLE_ACTIVE_LOGI("FLUSH TO DISK HANDLE");
            BundleActiveReportHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            if (tmpHandlerobj.userId_ != bundleActiveCore_->currentUsedUser_) {
                BUNDLE_ACTIVE_LOGE("flush user is %{public}d, not last user %{public}d, return",
                    tmpHandlerobj.userId_, bundleActiveCore_->currentUsedUser_);
                RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK, tmpHandlerobj.userId_);
                return;
            }
            bundleActiveCore_->RestoreToDatabase(tmpHandlerobj.userId_);
            break;
        }
        case MSG_REMOVE_USER: {
            BundleActiveReportHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            bundleActiveCore_->OnUserRemoved(tmpHandlerobj.userId_);
            break;
        }
        case MSG_BUNDLE_UNINSTALLED: {
            BUNDLE_ACTIVE_LOGI("MSG_BUNDLE_UNINSTALLED CALLED");
            BundleActiveReportHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            bundleActiveCore_->OnBundleUninstalled(tmpHandlerobj.userId_, tmpHandlerobj.bundleName_,
                tmpHandlerobj.uid_, tmpHandlerobj.appIndex_);
            break;
        }
        case MSG_SWITCH_USER: {
            BUNDLE_ACTIVE_LOGI("MSG_SWITCH_USER CALLED");
            BundleActiveReportHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            bundleActiveCore_->OnUserSwitched(tmpHandlerobj.userId_);
            break;
        }
        default: {
            break;
        }
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


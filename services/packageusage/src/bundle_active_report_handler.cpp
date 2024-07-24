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

#include "bundle_active_log.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
const std::string DEVICE_USAGE_REPORT_HANDLE_QUEUE = "DeviceUsageReportHandleQueue";

void BundleActiveReportHandler::Init(const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    bundleActiveCore_ = bundleActiveCore;
    ffrtQueue_ = std::make_shared<ffrt:queue>(DEVICE_USAGE_REPORT_HANDLE_QUEUE.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_default));
    if (ffrtQueue_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("BundleActiveReportHandler, ffrtQueue create failed");
        return;
    }
    isInited_ = true;
}

void BundleActiveReportHandler::SendEvent(int32_t eventId,
    std::shared_ptr<BundleActiveReportHandlerObject> handlerobj, const int32_t& delayTime)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    auto reportHandler = shared_from_this();
    taskHandlerMap_[eventId] = ffrtQueue_->submit_h([groupHandler, eventId, handlerobj]() {
        reportHandler->ProcessEvent(eventId, handlerobj);
        reportHandler->checkBundleTaskMap_.erase(eventId);
    }, ffrt::task_attr().delay(delayTime);
}

void BundleActiveReportHandler::RemoveEvent(const int32_t& eventId)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    if (taskHandlerMap_.find(eventId) == taskHandlerMap_.end()) {
        return;
    }
    ffrtQueue_->cancel(taskHandlerMap_[eventId]);
    taskHandlerMap_.erase(eventId);
}

void BundleActiveReportHandler::ProcessEvent(int32_t eventId,
    std::shared_ptr<BundleActiveReportHandlerObject> handlerobj)
{
    BundleActiveReportHandlerObject tmpHandlerobj = *ptrToHandlerobj;
    if (tmpHandlerobj == nullptr) {
        BUNDLE_ACTIVE_LOGE("tmpHandlerobj is null, exit ProcessEvent");
        return;
    }
    switch (eventId) {
        case MSG_REPORT_EVENT: {
            BUNDLE_ACTIVE_LOGD("MSG_REPORT_EVENT CALLED");
            bundleActiveCore_->ReportEvent(tmpHandlerobj.event_, tmpHandlerobj.userId_);
            break;
        }
        case MSG_FLUSH_TO_DISK: {
            BUNDLE_ACTIVE_LOGI("FLUSH TO DISK HANDLE");
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
            bundleActiveCore_->OnUserRemoved(tmpHandlerobj.userId_);
            break;
        }
        case MSG_BUNDLE_UNINSTALLED: {
            BUNDLE_ACTIVE_LOGI("MSG_BUNDLE_UNINSTALLED CALLED");
            bundleActiveCore_->OnBundleUninstalled(tmpHandlerobj.userId_, tmpHandlerobj.bundleName_,
                tmpHandlerobj.uid_, tmpHandlerobj.appIndex_);
            break;
        }
        case MSG_SWITCH_USER: {
            BUNDLE_ACTIVE_LOGI("MSG_SWITCH_USER CALLED");
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


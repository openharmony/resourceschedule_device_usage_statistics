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
#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
const std::string DEVICE_USAGE_REPORT_HANDLE_QUEUE = "DeviceUsageReportHandleQueue";
const int32_t BundleActiveReportHandler::MSG_REPORT_EVENT = 0;
const int32_t BundleActiveReportHandler::MSG_FLUSH_TO_DISK = 1;
const int32_t BundleActiveReportHandler::MSG_REMOVE_USER = 2;
const int32_t BundleActiveReportHandler::MSG_BUNDLE_UNINSTALLED = 3;
const int32_t BundleActiveReportHandler::MSG_SWITCH_USER = 4;
const int32_t BundleActiveReportHandler::MSG_BUNDLE_INSTALLED = 5;

void BundleActiveReportHandler::Init(const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    bundleActiveCore_ = bundleActiveCore;
    ffrtQueue_ = std::make_shared<ffrt::queue>(DEVICE_USAGE_REPORT_HANDLE_QUEUE.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_default));
    if (ffrtQueue_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("BundleActiveReportHandler, ffrtQueue create failed");
        return;
    }
    isInited_ = true;
}

void BundleActiveReportHandler::DeInit()
{
    isInited_ = false;
    for (auto& iter : taskHandlerMap_) {
        auto& queue = iter.second;
        while (!queue.empty()) {
            ffrtQueue_->cancel(queue.front());
            queue.pop();
        }
    }
    taskHandlerMap_.clear();
}

void BundleActiveReportHandler::SendEvent(const int32_t& eventId,
    const std::shared_ptr<BundleActiveReportHandlerObject>& handlerobj, const int64_t& delayTime)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    auto reportHandler = shared_from_this();
    int64_t ffrtDelayTime = BundleActiveUtil::GetFFRTDelayTime(delayTime);
    std::lock_guard<ffrt::mutex> lock(taskHandlerMutex_);
    if (taskHandlerMap_.find(eventId) == taskHandlerMap_.end()) {
        taskHandlerMap_[eventId] = std::queue<ffrt::task_handle>();
    }
    ffrt::task_handle taskHandle = ffrtQueue_->submit_h([reportHandler, eventId, handlerobj]() {
        reportHandler->ProcessEvent(eventId, handlerobj);
        std::lock_guard<ffrt::mutex> lock(reportHandler->taskHandlerMutex_);
        if (reportHandler->taskHandlerMap_.find(eventId) == reportHandler->taskHandlerMap_.end()) {
            return;
        }
        if (!reportHandler->taskHandlerMap_[eventId].empty()) {
            reportHandler->taskHandlerMap_[eventId].pop();
        }
    }, ffrt::task_attr().delay(ffrtDelayTime));
    taskHandlerMap_[eventId].push(std::move(taskHandle));
}

void BundleActiveReportHandler::RemoveEvent(const int32_t& eventId)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    std::lock_guard<ffrt::mutex> lock(taskHandlerMutex_);
    if (taskHandlerMap_.find(eventId) == taskHandlerMap_.end()) {
        return;
    }
    while (!taskHandlerMap_[eventId].empty()) {
        ffrtQueue_->cancel(taskHandlerMap_[eventId].front());
        taskHandlerMap_[eventId].pop();
    }
    taskHandlerMap_.erase(eventId);
}

bool BundleActiveReportHandler::HasEvent(const int32_t& eventId)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return false;
    }
    std::lock_guard<ffrt::mutex> lock(taskHandlerMutex_);
    if (taskHandlerMap_.find(eventId) != taskHandlerMap_.end()) {
        return true;
    }
    return false;
}

void BundleActiveReportHandler::ProcessEvent(const int32_t& eventId,
    const std::shared_ptr<BundleActiveReportHandlerObject>& handlerobj)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    if (handlerobj == nullptr) {
        BUNDLE_ACTIVE_LOGE("handlerobj is null, exit ProcessEvent");
        return;
    }
    BundleActiveReportHandlerObject tmpHandlerobj = *handlerobj;
    switch (eventId) {
        case MSG_REPORT_EVENT: {
            ProcessReportEvent(tmpHandlerobj);
            break;
        }
        case MSG_FLUSH_TO_DISK: {
            ProcessFlushToDiskEvent(tmpHandlerobj);
            break;
        }
        case MSG_REMOVE_USER: {
            ProcessRmoveUserEvent(tmpHandlerobj);
            break;
        }
        case MSG_SWITCH_USER: {
            ProcessUserSwitchEvent(tmpHandlerobj);
            break;
        }
        case MSG_BUNDLE_UNINSTALLED: {
            ProcessBundleUninstallEvent(tmpHandlerobj);
            break;
        }
        case MSG_BUNDLE_INSTALLED: {
            ProcessBundleInstallEvent(tmpHandlerobj);
            break;
        }
        default: {
            break;
        }
    }
}

void BundleActiveReportHandler::ProcessReportEvent(BundleActiveReportHandlerObject& tmpHandlerobj)
{
    BUNDLE_ACTIVE_LOGD("MSG_REPORT_EVENT CALLED");
    if (BundleActiveEvent::IsAppStateEvent(tmpHandlerobj.event_.eventId_) &&
        bundleActiveCore_->isUninstalledApp(tmpHandlerobj.event_.uid_)) {
            BUNDLE_ACTIVE_LOGE("not report uninstall app event");
            return;
        }
    bundleActiveCore_->ReportEvent(tmpHandlerobj.event_, tmpHandlerobj.userId_);
}

void BundleActiveReportHandler::ProcessFlushToDiskEvent(const BundleActiveReportHandlerObject& tmpHandlerobj)
{
    BUNDLE_ACTIVE_LOGI("FLUSH TO DISK HANDLE");
    if (tmpHandlerobj.userId_ != bundleActiveCore_->currentUsedUser_) {
        BUNDLE_ACTIVE_LOGE("flush user is %{public}d, not last user %{public}d, return",
            tmpHandlerobj.userId_, bundleActiveCore_->currentUsedUser_);
        RemoveEvent(BundleActiveReportHandler::MSG_FLUSH_TO_DISK);
        return;
    }
    bundleActiveCore_->RestoreToDatabase(tmpHandlerobj.userId_);
}

void BundleActiveReportHandler::ProcessRmoveUserEvent(const BundleActiveReportHandlerObject& tmpHandlerobj)
{
    BUNDLE_ACTIVE_LOGI("Remove user");
    bundleActiveCore_->OnUserRemoved(tmpHandlerobj.userId_);
}

void BundleActiveReportHandler::ProcessUserSwitchEvent(const BundleActiveReportHandlerObject& tmpHandlerobj)
{
    BUNDLE_ACTIVE_LOGI("MSG_SWITCH_USER CALLED");
    bundleActiveCore_->OnUserSwitched(tmpHandlerobj.userId_);
}

void BundleActiveReportHandler::ProcessBundleUninstallEvent(const BundleActiveReportHandlerObject& tmpHandlerobj)
{
    BUNDLE_ACTIVE_LOGI("MSG_BUNDLE_UNINSTALLED CALLED");
    bundleActiveCore_->OnBundleUninstalled(tmpHandlerobj.userId_, tmpHandlerobj.bundleName_,
        tmpHandlerobj.uid_, tmpHandlerobj.appIndex_);
}

void BundleActiveReportHandler::ProcessBundleInstallEvent(const BundleActiveReportHandlerObject& tmpHandlerobj)
{
    BUNDLE_ACTIVE_LOGI("MSG_BUNDLE_INSTALLED CALLED");
    bundleActiveCore_->OnBundleInstalled(tmpHandlerobj.userId_, tmpHandlerobj.bundleName_,
        tmpHandlerobj.uid_, tmpHandlerobj.appIndex_);
}

}  // namespace DeviceUsageStats
}  // namespace OHOS


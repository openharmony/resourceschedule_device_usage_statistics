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

#include "time_service_client.h"
#include "bundle_active_account_helper.h"
#include "bundle_active_group_handler.h"
#include "bundle_active_log.h"
#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
const std::string DEVICE_GROUP_HANDLE_QUEUE = "DeviceUsageGroupHandleQueue";
const int32_t BundleActiveGroupHandler::MSG_CHECK_DEFAULT_BUNDLE_STATE = 0;
const int32_t BundleActiveGroupHandler::MSG_ONE_TIME_CHECK_BUNDLE_STATE = 1;
const int32_t BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE = 2;
const int32_t BundleActiveGroupHandler::MSG_CHECK_NOTIFICATION_SEEN_BUNDLE_STATE = 3;
const int32_t BundleActiveGroupHandler::MSG_CHECK_SYSTEM_INTERACTIVE_BUNDLE_STATE = 4;
#ifndef OS_ACCOUNT_PART_ENABLED
namespace {
constexpr int32_t DEFAULT_OS_ACCOUNT_ID = 0; // 0 is the default id when there is no os_account part
} // namespace
#endif // OS_ACCOUNT_PART_ENABLED
BundleActiveGroupHandlerObject::BundleActiveGroupHandlerObject(const BundleActiveGroupHandlerObject& orig)
{
    bundleName_ = orig.bundleName_;
    userId_ = orig.userId_;
    uid_ = orig.uid_;
}

BundleActiveGroupHandler::BundleActiveGroupHandler(const bool debug)
{
    if (debug) {
        checkIdleInterval_ = ONE_MINUTE;
    } else {
        checkIdleInterval_ = THREE_HOUR;
    }
}

void BundleActiveGroupHandler::DeInit()
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
    for (const auto& iter: checkBundleTaskMap_) {
        ffrtQueue_->cancel(iter.second);
    }
    checkBundleTaskMap_.clear();
}

void BundleActiveGroupHandler::Init()
{
    BUNDLE_ACTIVE_LOGI("Init called");
    ffrtQueue_ = std::make_shared<ffrt::queue>(DEVICE_GROUP_HANDLE_QUEUE.c_str(),
        ffrt::queue_attr().qos(ffrt::qos_default));
    if (ffrtQueue_ == nullptr) {
        BUNDLE_ACTIVE_LOGE("Init failed ffrtQueue is null");
        return;
    }
    isInited_ = true;
}

void BundleActiveGroupHandler::SendCheckBundleMsg(const int32_t& eventId,
    const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj, const int64_t& delayTime)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    std::string msgKey = GetMsgKey(eventId, handlerobj, delayTime);
    if (msgKey == "") {
        return;
    }
    int64_t ffrtDelayTime = BundleActiveUtil::GetFFRTDelayTime(delayTime);
    std::lock_guard<ffrt::mutex> lock(checkBundleTaskMutex_);
    if (checkBundleTaskMap_.find(msgKey) != checkBundleTaskMap_.end()) {
        RemoveCheckBundleMsg(msgKey);
    }
    checkBundleTaskMap_[msgKey] = ffrtQueue_->submit_h([eventId, handlerobj, msgKey]() {
        auto groupHandler = BundleActiveGroupController::GetInstance().GetBundleGroupHandler();
        if (groupHandler == nullptr) {
            return;
        }
        groupHandler->ProcessEvent(eventId, handlerobj);
        std::lock_guard<ffrt::mutex> lock(groupHandler->checkBundleTaskMutex_);
        if (groupHandler->checkBundleTaskMap_.find(msgKey) == groupHandler->checkBundleTaskMap_.end()) {
            return;
        }
        groupHandler->checkBundleTaskMap_.erase(msgKey);
    }, ffrt::task_attr().delay(ffrtDelayTime));
}

void BundleActiveGroupHandler::RemoveCheckBundleMsg(const std::string& msgKey)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    if (checkBundleTaskMap_.find(msgKey) == checkBundleTaskMap_.end()) {
        return;
    }
    ffrtQueue_->cancel(checkBundleTaskMap_[msgKey]);
    checkBundleTaskMap_.erase(msgKey);
}

std::string BundleActiveGroupHandler::GetMsgKey(const int32_t& eventId,
    const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj, const int64_t& delayTime)
{
    if (handlerobj == nullptr) {
        BUNDLE_ACTIVE_LOGE("handlerobj is null, GetMsgKey failed");
        return "";
    }
    BundleActiveGroupHandlerObject tmpHandlerobj = *handlerobj;
    return std::to_string(eventId) + "_" + std::to_string(tmpHandlerobj.userId_) + "_" +
        std::to_string(tmpHandlerobj.uid_) + "_" + tmpHandlerobj.bundleName_ + "_" + std::to_string(delayTime);
}

void BundleActiveGroupHandler::SendEvent(const int32_t& eventId,
    const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj, const int64_t& delayTime)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    int64_t ffrtDelayTime = BundleActiveUtil::GetFFRTDelayTime(delayTime);
    std::lock_guard<ffrt::mutex> lock(taskHandlerMutex_);
    if (taskHandlerMap_.find(eventId) == taskHandlerMap_.end()) {
        taskHandlerMap_[eventId] = std::queue<ffrt::task_handle>();
    }
    ffrt::task_handle taskHandle = ffrtQueue_->submit_h([eventId, handlerobj]() {
        auto groupHandler = BundleActiveGroupController::GetInstance().GetBundleGroupHandler();
        if (groupHandler == nullptr) {
            return;
        }
        if (!groupHandler->isInited_) {
            BUNDLE_ACTIVE_LOGE("init failed");
            return;
        }
        groupHandler->ProcessEvent(eventId, handlerobj);
        std::lock_guard<ffrt::mutex> lock(groupHandler->taskHandlerMutex_);
        if (groupHandler->taskHandlerMap_.find(eventId) == groupHandler->taskHandlerMap_.end()) {
            return;
        }
        if (!groupHandler->taskHandlerMap_[eventId].empty()) {
            groupHandler->taskHandlerMap_[eventId].pop();
        }
    }, ffrt::task_attr().delay(ffrtDelayTime));
    taskHandlerMap_[eventId].push(std::move(taskHandle));
}

void BundleActiveGroupHandler::RemoveEvent(const int32_t& eventId)
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

void BundleActiveGroupHandler::PostSyncTask(const std::function<void()>& fuc)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    auto taskHandle = ffrtQueue_->submit_h(fuc);
    ffrtQueue_->wait(taskHandle);
}

void BundleActiveGroupHandler::PostTask(const std::function<void()>& fuc)
{
    if (!isInited_) {
        BUNDLE_ACTIVE_LOGE("init failed");
        return;
    }
    ffrtQueue_->submit(fuc);
}

void BundleActiveGroupHandler::ProcessEvent(const int32_t& eventId,
    const std::shared_ptr<BundleActiveGroupHandlerObject>& handlerobj)
{
    if (!isInited_) {
        return;
    }
    switch (eventId) {
        case MSG_CHECK_DEFAULT_BUNDLE_STATE:
        case MSG_CHECK_NOTIFICATION_SEEN_BUNDLE_STATE:
        case MSG_CHECK_SYSTEM_INTERACTIVE_BUNDLE_STATE: {
            if (handlerobj == nullptr) {
                return;
            }
            BundleActiveGroupHandlerObject tmpHandlerobj = *handlerobj;
            sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
            int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
            BundleActiveGroupController::GetInstance().CheckAndUpdateGroup(
                tmpHandlerobj.bundleName_, tmpHandlerobj.userId_, tmpHandlerobj.uid_, bootBasedTimeStamp);
                BundleActiveGroupController::GetInstance().RestoreToDatabase(tmpHandlerobj.userId_);
            break;
        }
        case MSG_ONE_TIME_CHECK_BUNDLE_STATE: {
            std::vector<int32_t> activatedOsAccountIds;
            BundleActiveAccountHelper::GetActiveUserId(activatedOsAccountIds);
            for (uint32_t i = 0; i < activatedOsAccountIds.size(); i++) {
                BundleActiveGroupController::GetInstance().CheckEachBundleState(activatedOsAccountIds[i]);
                BundleActiveGroupController::GetInstance().RestoreToDatabase(activatedOsAccountIds[i]);
            }
            RemoveEvent(MSG_ONE_TIME_CHECK_BUNDLE_STATE);
            break;
        }
        case MSG_CHECK_IDLE_STATE: {
            if (handlerobj == nullptr) {
                return;
            }
            BundleActiveGroupHandlerObject tmpHandlerobj = *handlerobj;
            if (BundleActiveGroupController::GetInstance().CheckEachBundleState(tmpHandlerobj.userId_) &&
            BundleActiveGroupController::GetInstance().GetBundleGroupEnable()) {
                BundleActiveGroupHandlerObject GroupHandlerObj;
                GroupHandlerObj.userId_ = tmpHandlerobj.userId_;
                std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
                    std::make_shared<BundleActiveGroupHandlerObject>(GroupHandlerObj);
                    BundleActiveGroupController::GetInstance().RestoreToDatabase(GroupHandlerObj.userId_);
                SendEvent(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE, handlerobjToPtr, checkIdleInterval_);
            }
            break;
        }
        default: {
            break;
        }
    }
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


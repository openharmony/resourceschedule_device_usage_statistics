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

#include "time_service_client.h"
#include "os_account_manager.h"

#include "bundle_active_group_handler.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveGroupHandlerObject::BundleActiveGroupHandlerObject(const BundleActiveGroupHandlerObject& orig)
{
    bundleName_ = orig.bundleName_;
    userId_ = orig.userId_;
}

BundleActiveGroupHandler::BundleActiveGroupHandler
    (const std::shared_ptr<AppExecFwk::EventRunner> &runner) : AppExecFwk::EventHandler(runner)
{
}

void BundleActiveGroupHandler::Init(const std::shared_ptr<BundleActiveGroupController>& bundleActiveController)
{
    BUNDLE_ACTIVE_LOGI("Init called");
    if (bundleActiveController == nullptr) {
        BUNDLE_ACTIVE_LOGE("Init failed bundleActiveController is null");
    }
    bundleActiveGroupController_ = bundleActiveController;
}

void BundleActiveGroupHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    switch (event->GetInnerEventId()) {
        case MSG_CHECK_BUNDLE_STATE: {
            auto ptrToHandlerobj = event->GetSharedObject<BundleActiveGroupHandlerObject>();
            BundleActiveGroupHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            sptr<MiscServices::TimeServiceClient> timer = MiscServices::TimeServiceClient::GetInstance();
            int64_t bootBasedTimeStamp = timer->GetBootTimeMs();
            bundleActiveGroupController_->CheckAndUpdateGroup(
                tmpHandlerobj.bundleName_, tmpHandlerobj.userId_, bootBasedTimeStamp);
            bundleActiveGroupController_->RestoreToDatabase(tmpHandlerobj.userId_);
            break;
        }
        case MSG_ONE_TIME_CHECK_BUNDLE_STATE: {
            std::vector<int> activatedOsAccountIds;
            if (AccountSA::OsAccountManager::QueryActiveOsAccountIds(activatedOsAccountIds) != ERR_OK) {
                BUNDLE_ACTIVE_LOGI("query activated account failed");
                return;
            }
            if (activatedOsAccountIds.size() == 0) {
                BUNDLE_ACTIVE_LOGI("GetAllActiveUser size is 0");
                return;
            }
            for (uint32_t i = 0; i < activatedOsAccountIds.size(); i++) {
                bundleActiveGroupController_->CheckEachBundleState(activatedOsAccountIds[i]);
                bundleActiveGroupController_->RestoreToDatabase(activatedOsAccountIds[i]);
            }
            RemoveEvent(MSG_ONE_TIME_CHECK_BUNDLE_STATE);
            break;
        }
        case MSG_CHECK_IDLE_STATE: {
            auto ptrToHandlerobj = event->GetSharedObject<BundleActiveGroupHandlerObject>();
            BundleActiveGroupHandlerObject tmpHandlerobj = *ptrToHandlerobj;
            if (bundleActiveGroupController_->CheckEachBundleState(tmpHandlerobj.userId_) &&
                bundleActiveGroupController_->bundleGroupEnable_) {
                BundleActiveGroupHandlerObject GroupHandlerObj;
                GroupHandlerObj.userId_ = tmpHandlerobj.userId_;
                std::shared_ptr<BundleActiveGroupHandlerObject> handlerobjToPtr =
                    std::make_shared<BundleActiveGroupHandlerObject>(GroupHandlerObj);
                auto handlerEvent = AppExecFwk::InnerEvent::Get(BundleActiveGroupHandler::MSG_CHECK_IDLE_STATE,
                    handlerobjToPtr);
                bundleActiveGroupController_->RestoreToDatabase(GroupHandlerObj.userId_);
                SendEvent(handlerEvent, CHECK_IDLE_INTERVAL);
            }
            break;
        }
        default: {
            break;
        }
    }
}
}
}
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "bundle_active_report_controller.h"

#include "bundle_active_log.h"
#include "bundle_active_user_history.h"
#include "bundle_active_group_handler.h"
#include "ibundle_active_service.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_util.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveReportController& BundleActiveReportController::GetInstance()
{
    static BundleActiveReportController instance;
    return instance;
}

void BundleActiveReportController::Init(const std::shared_ptr<BundleActiveCore>& bundleActiveCore)
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (isInit_) {
        return;
    }
    activeReportHandler_ = std::make_shared<BundleActiveReportHandler>();
    if (activeReportHandler_ == nullptr) {
        return;
    }
    activeReportHandler_->Init(bundleActiveCore);
    isInit_ = true;
}

void BundleActiveReportController::DeInit()
{
    if (!isInit_) {
        return;
    }
    std::lock_guard<ffrt::mutex> lock(mutex_);
    isInit_ = false;
    activeReportHandler_->DeInit();
}

std::shared_ptr<BundleActiveReportHandler> BundleActiveReportController::GetBundleReportHandler()
{
    std::lock_guard<ffrt::mutex> lock(mutex_);
    if (!isInit_) {
        return nullptr;
    }
    return activeReportHandler_;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
 
 
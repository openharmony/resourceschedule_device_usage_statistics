/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_ACTIVE_TEST_UTIL_H
#define BUNDLE_ACTIVE_TEST_UTIL_H

#include <thread>
#include <gtest/gtest.h>

#include "bundle_active_core.h"
#include "bundle_active_report_controller.h"
#include "bundle_active_group_controller.h"

namespace OHOS {
namespace DeviceUsageStats {
namespace BundleActiveTestUtil {
std::shared_ptr<BundleActiveCore> TestInit()
{
    std::shared_ptr<BundleActiveCore> bundleActiveCore = std::make_shared<BundleActiveCore>();
    bundleActiveCore->Init();
    bundleActiveCore->InitBundleGroupController();
    BundleActiveGroupController::GetInstance().Init(false);
    return bundleActiveCore;
};

void TestDeInit()
{
    BundleActiveReportController::GetInstance().DeInit();
    BundleActiveGroupController::GetInstance().DeInit();
    auto activeGroupHandler = BundleActiveGroupController::GetInstance().activeGroupHandler_;
    if (activeGroupHandler && activeGroupHandler->ffrtQueue_) {
        auto handle = activeGroupHandler->ffrtQueue_->submit_h([]() {});
        activeGroupHandler->ffrtQueue_->wait(handle);
        activeGroupHandler->ffrtQueue_.reset();
    }
    auto activeReportHandler = BundleActiveReportController::GetInstance().activeReportHandler_;
    if (activeReportHandler && activeReportHandler->ffrtQueue_) {
        auto handle = activeReportHandler->ffrtQueue_->submit_h([]() {});
        activeReportHandler->ffrtQueue_->wait(handle);
        activeReportHandler->ffrtQueue_.reset();
    }
    int64_t sleepTimeEnd = 1;
    std::this_thread::sleep_for(std::chrono::seconds(sleepTimeEnd));
}
};


}  // namespace DeviceUsageStats
}
#endif  // BUNDLE_ACTIVE_TEST_UTIL_H
 
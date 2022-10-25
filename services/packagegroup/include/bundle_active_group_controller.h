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

#ifndef BUNDLE_ACTIVE_GROUP_CONTROLLER_H
#define BUNDLE_ACTIVE_GROUP_CONTROLLER_H

#include <memory>
#include <mutex>

#include "power_mgr_client.h"
#include "application_info.h"

#include "ibundle_active_service.h"
#include "bundle_active_event.h"
#include "bundle_active_user_history.h"
#include "bundle_state_inner_errors.h"
#include "bundle_mgr_interface.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace DeviceUsageStatsGroupConst;

class BundleActiveGroupHandler;
class BundleActiveGroupController {
public:
    using PowerMgrClient = OHOS::PowerMgr::PowerMgrClient;
    using IBundleMgr = OHOS::AppExecFwk::IBundleMgr;
    using ApplicationInfo = OHOS::AppExecFwk::ApplicationInfo;
    using BundleInfo = OHOS::AppExecFwk::BundleInfo;
    using BundleFlag = OHOS::AppExecFwk::BundleFlag;
    using ApplicationFlag = OHOS::AppExecFwk::ApplicationFlag;
    OHOS::AppExecFwk::ApplicationFlag flag = OHOS::AppExecFwk::ApplicationFlag::GET_BASIC_APPLICATION_INFO;
    bool bundleGroupEnable_ = true;
    const int32_t LEVEL_GROUP[4] = {
        ACTIVE_GROUP_ALIVE,
        ACTIVE_GROUP_DAILY,
        ACTIVE_GROUP_FIXED,
        ACTIVE_GROUP_RARE
    };
    std::vector<int64_t> screenTimeLevel_ = {0, 0, 0, 0};
    std::vector<int64_t> bootTimeLevel_ = {0, 0, 0, 0};
    BundleActiveGroupController(const bool debug);
    ~BundleActiveGroupController() {}
    std::shared_ptr<BundleActiveUserHistory> bundleUserHistory_;
    void SetHandlerAndCreateUserHistory(const std::shared_ptr<BundleActiveGroupHandler>& groupHandler,
        const int64_t bootFromTimeStamp, const std::shared_ptr<BundleActiveCore>& bundleActiveCore);
    void ReportEvent(const BundleActiveEvent& event, const int64_t bootBasedTimeStamp, const int32_t userId);
    void CheckAndUpdateGroup(const std::string& bundleName, const int32_t userId, const int64_t bootBasedTimeStamp);
    bool CheckEachBundleState(const int32_t userId);
    void CheckIdleStatsOneTime();
    void PeriodCheckBundleState(const int32_t userId);
    void OnUserRemoved(const int32_t userId);
    void OnBundleUninstalled(const int32_t userId, const std::string bundleName);
    void OnScreenChanged(const bool& isScreenOn, const int64_t bootFromTimeStamp);
    int32_t SetAppGroup(const std::string& bundleName, const int32_t userId, int32_t newGroup, uint32_t reason,
        const int64_t bootBasedTimeStamp, const bool isFlush);
    void RestoreToDatabase(const int32_t userId);
    void RestoreDurationToDatabase();
    bool IsBundleInstalled(const std::string& bundleName, const int32_t userId);
    bool IsScreenOn();
    int32_t IsBundleIdle(const std::string& bundleName, const int32_t userId);
    ErrCode QueryAppGroup(int32_t& appGroup, const std::string& bundleName, const int32_t userId);
    void ShutDown(const int64_t bootBasedTimeStamp, const int32_t userId);
    void OnUserSwitched(const int32_t userId, const int32_t currentUsedUser);

private:
    std::mutex mutex_;
    bool GetBundleMgrProxy();
    std::weak_ptr<BundleActiveGroupHandler> activeGroupHandler_;
    int64_t timeoutForDirectlyUse_;
    int64_t timeoutForNotifySeen_;
    int64_t timeoutForSystemInteraction_;
    int64_t timeoutCalculated_ = 0;
    std::map<int32_t, uint32_t> eventIdMatchReason_;
    sptr<IBundleMgr> sptrBundleMgr_;
    bool calculationTimeOut(const std::shared_ptr<BundleActivePackageHistory>& oneBundleHistory,
        const int64_t bootBasedTimeStamp);
    int32_t GetNewGroup(const std::string& bundleName, const int32_t userId, const int64_t bootBasedTimeStamp);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_GROUP_CONTROLLER_H


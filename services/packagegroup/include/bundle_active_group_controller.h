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
    bool debug_ = true;
    const int LEVEL_GROUP[4] = {
        ACTIVE_GROUP_ALIVE,
        ACTIVE_GROUP_DAILY,
        ACTIVE_GROUP_FIXED,
        ACTIVE_GROUP_RARE
    };
    const int64_t SCREEN_TIME_LEVEL[4] = {
        0,
        0,
        debug_ ? TWO_MINUTE : ONE_HOUR,
        debug_ ? FOUR_MINUTE : TWO_HOUR
    };
    const int64_t BOOT_TIME_LEVEL[4] = {
        0,
        debug_ ? TWO_MINUTE : TWELVE_HOUR,
        debug_ ? FOUR_MINUTE : TWENTY_FOUR_HOUR,
        debug_ ? SIXTEEN_MINUTE : FOURTY_EIGHT_HOUR
    };
    BundleActiveGroupController() {};
    ~BundleActiveGroupController() {};
    std::shared_ptr<BundleActiveUserHistory> bundleUserHistory_;
    void SetHandlerAndCreateUserHistory(const std::shared_ptr<BundleActiveGroupHandler>& groupHandler,
        const int64_t bootFromTimeStamp);
    void ReportEvent(const BundleActiveEvent& event, const int64_t bootBasedTimeStamp, const int userId);
    void CheckAndUpdateGroup(const std::string& bundleName, const int userId, const int64_t bootBasedTimeStamp);
    bool CheckEachBundleState(const int userId);
    void CheckIdleStatsOneTime();
    void PeriodCheckBundleState(const int userId);
    void OnUserRemoved(const int userId);
    void OnBundleUninstalled(const int userId, const std::string bundleName);
    void OnScreenChanged(const bool& isScreenOn, const int64_t bootFromTimeStamp);
    void SetBundleGroup(const std::string& bundleName, const int userId, int newGroup, uint32_t reason,
        const int64_t bootBasedTimeStamp, const bool& resetTimeout);
    void RestoreToDatabase(const int userId);
    void RestoreDurationToDatabase();
    bool IsBundleInstalled(const std::string& bundleName, const int userId);
    bool IsScreenOn();
    int IsBundleIdle(const std::string& bundleName, const int userId);
    int QueryPackageGroup(const int userId, const std::string& bundleName);
    void ShutDown(const int64_t bootBasedTimeStamp);
    void OnUserSwitched(const int userId);

private:
    std::mutex mutex_;
    bool GetBundleMgrProxy();
    std::weak_ptr<BundleActiveGroupHandler> activeGroupHandler_;
    uint32_t EventToGroupReason(const int eventId);
    int64_t timeoutForDirectlyUse_ = debug_ ? THREE_MINUTE : ONE_HOUR;
    int64_t timeoutForNotifySeen_ = debug_ ? ONE_MINUTE : TWELVE_HOUR;
    int64_t timeoutForSystemInteraction_ = debug_ ? ONE_MINUTE : TEN_MINUTE;
    int64_t timeoutCalculated_;
    sptr<IBundleMgr> sptrBundleMgr_;
    bool calculationTimeOut(const std::shared_ptr<BundleActivePackageHistory>& oneBundleHistory,
        const int64_t bootBasedTimeStamp);
    int GetNewGroup(const std::string& bundleName, const int userId, const int64_t bootBasedTimeStamp);
};
}
}

#endif
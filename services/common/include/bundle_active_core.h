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

#ifndef BUNDLE_ACTIVE_CORE_H
#define BUNDLE_ACTIVE_CORE_H

#include <mutex>

#include "power_mgr_client.h"
#include "os_account_manager.h"

#include "ibundle_active_service.h"
#include "bundle_active_stats_update_listener.h"
#include "bundle_active_user_service.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_group_handler.h"
#include "bundle_active_common_event_subscriber.h"
#include "bundle_active_constant.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveReportHandlerObject {
public:
    BundleActiveEvent event_;
    int userId_;
    std::string bundleName_;
    BundleActiveReportHandlerObject() {};
    BundleActiveReportHandlerObject(const BundleActiveReportHandlerObject& orig);
    ~BundleActiveReportHandlerObject() {};
};

class BundleActiveReportHandler;

class BundleActiveCore : public BundleActiveStatsUpdateListener {
public:
    BundleActiveCore();
    virtual ~BundleActiveCore();
    int ReportEvent(BundleActiveEvent& event, const int& userId);
    int ReportEventToAllUserId(BundleActiveEvent& event);
    void OnStatsChanged() override;
    void OnStatsReload() override;
    void OnSystemUpdate(int userId) override;
    void OnBundleUninstalled(const int& userId, const std::string& bundleName);
    void Init();
    void InitBundleGroupController();
    void SetHandler(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    void RestoreToDatabase();
    void ShutDown();
    std::vector<BundleActivePackageStats> QueryPackageStats(const int& userId, const int& intervalType,
        const int64_t& beginTime, const int64_t& endTime, std::string bundleName);
    std::vector<BundleActiveEvent> QueryEvents(const int& userId, const int64_t& beginTime,
        const int64_t& endTime, std::string bundleName);
    int IsBundleIdle(const std::string& bundleName, const int& userId);
    int QueryPackageGroup(const int& userId, const std::string bundleName);
    int64_t CheckTimeChangeAndGetWallTime();
    void ConvertToSystemTimeLocked(BundleActiveEvent& event);
    std::shared_ptr<BundleActiveUserService> GetUserDataAndInitializeIfNeeded(const int& userId,
        const int64_t& timeStamp);
    void OnUserRemoved(const int& userId);
    void RestoreToDatabaseLocked();
    void SetBundleGroup(const std::string& bundleName, const int& newGroup, const int& userId);
    void GetAllActiveUser(std::vector<OHOS::AccountSA::OsAccountInfo> &osAccountInfos);
    void UnRegisterSubscriber();

private:
    static const int64_t FLUSH_INTERVAL = THIRTY_MINUTES;
    static const int64_t TIME_CHANGE_THRESHOLD_MILLIS = TWO_SECONDS;
    const int DEFAULT_USER_ID = -1;
    std::map<int, std::string> visibleActivities_;
    // use weak_ptr to avoid circulate reference of core and handler.
    std::weak_ptr<BundleActiveReportHandler> handler_;
    std::shared_ptr<BundleActiveGroupController> bundleGroupController_;
    std::shared_ptr<BundleActiveGroupHandler> bundleGroupHandler_;
    int64_t systemTimeShot_;
    int64_t realTimeShot_;
    std::mutex mutex_;
    std::map<int, std::shared_ptr<BundleActiveUserService>> userStatServices_;
    void RegisterSubscriber();
    std::shared_ptr<BundleActiveCommonEventSubscriber> commonEventSubscriber_;
};
}
}
#endif

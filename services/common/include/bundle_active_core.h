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
    BundleActiveReportHandlerObject();
    BundleActiveReportHandlerObject(const BundleActiveReportHandlerObject& orig);
    ~BundleActiveReportHandlerObject() {};
};

class BundleActiveReportHandler;

class BundleActiveCore : public BundleActiveStatsUpdateListener {
public:
    BundleActiveCore();
    virtual ~BundleActiveCore();
    /*
    * function: ReportEvent, used to report ability fourground/background/destroy event.
    * parameters: event, userId
    */
    int ReportEvent(BundleActiveEvent& event, const int userId);
    /*
    * function: ReportEventToAllUserId, report flush to disk, end_of_day event to service.
    * parameters: event
    */
    int ReportEventToAllUserId(BundleActiveEvent& event);
    /*
    * function: OnStatsChanged, report flush to disk, end_of_day event to service.
    */
    void OnStatsChanged(const int userId) override;
    /*
    * function: OnStatsChanged, when device reboot after more than one day, BundleActiveUserService
    * will use it to flush group info.
    */
    void OnStatsReload() override;
    /*
    * function: OnSystemUpdate, now is emtpy, later will called when system is updated.
    */
    void OnSystemUpdate(int userId) override;
    /*
    * function: OnBundleUninstalled when received a PACKATE_REMOVED commen event,
    * BundleActiveCommonEventSubscriber call it to remove data.
    * parameter: userId, Bundlename.
    */
    void OnBundleUninstalled(const int userId, const std::string& bundleName);
    /*
    * function: Init, BundleAciveService call it to init systemTimeShot_, realTimeShot_,
    * create bundleGroupController_ object.
    */
    void Init();
    /*
    * function: InitBundleGroupController, BundleAciveService call it to init bundleGroupController_ object,
    * set its handler and subscribe needed common event.
    * create bundleGroupController_ object.
    */
    void InitBundleGroupController();
    /*
    * function: SetHandler, BundleActiveService call it to set event report handler
    */
    void SetHandler(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    // flush database for one user data
    void RestoreToDatabase(const int userId);
    // flush database for one user data
    void RestoreToDatabaseLocked(const int userId);
    // called when device shutdown, update the in-memory stat and flush the database.
    void ShutDown();
    // query the package stat for calling user.
    std::vector<BundleActivePackageStats> QueryPackageStats(const int userId, const int intervalType,
        const int64_t beginTime, const int64_t endTime, std::string bundleName);
    // query the event stat for calling user.
    std::vector<BundleActiveEvent> QueryEvents(const int userId, const int64_t beginTime,
        const int64_t endTime, std::string bundleName);
    // check the app idle state for calling user.
    int IsBundleIdle(const std::string& bundleName, const int userId);
    // query the app group for calling app.
    int QueryPackageGroup(const int userId, const std::string bundleName);
    // get the wall time and check if the wall time is changed.
    int64_t CheckTimeChangeAndGetWallTime(int userId = 0);
    // convert event timestamp from boot based time to wall time.
    void ConvertToSystemTimeLocked(BundleActiveEvent& event);
    // get or create BundleActiveUserService object for specifice user.
    std::shared_ptr<BundleActiveUserService> GetUserDataAndInitializeIfNeeded(const int userId,
        const int64_t timeStamp);
    // when received a USER_REMOVED commen event, call it to remove data.
    void OnUserRemoved(const int userId);
    // force set app group.
    void SetBundleGroup(const std::string& bundleName, const int newGroup, const int userId);
    // get all user in device
    void GetAllActiveUser(std::vector<OHOS::AccountSA::OsAccountInfo> &osAccountInfos);
    // when service stop, call it to unregister commen event and shutdown call back.
    void UnRegisterSubscriber();
    int64_t GetSystemTimeMs();

private:
    static const int64_t FLUSH_INTERVAL = TWO_MINUTE;
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
    void RestoreAllData();
};
}
}
#endif

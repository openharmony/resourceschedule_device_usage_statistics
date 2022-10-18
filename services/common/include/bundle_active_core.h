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
#include "accesstoken_kit.h"
#ifdef OS_ACCOUNT_PART_ENABLED
#include "os_account_manager.h"
#endif // OS_ACCOUNT_PART_ENABLED
#include "ibundle_active_service.h"
#include "remote_death_recipient.h"
#include "iapp_group_callback.h"
#include "bundle_active_debug_mode.h"
#include "bundle_active_stats_update_listener.h"
#include "bundle_state_inner_errors.h"
#include "bundle_active_user_service.h"
#include "bundle_active_group_controller.h"
#include "bundle_active_group_handler.h"
#include "bundle_active_common_event_subscriber.h"
#include "bundle_active_constant.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace OHOS::Security;

class BundleActiveReportHandlerObject {
public:
    BundleActiveEvent event_;
    int32_t userId_;
    std::string bundleName_;
    BundleActiveReportHandlerObject();
    BundleActiveReportHandlerObject(const int32_t userId, const std::string bundleName);
    BundleActiveReportHandlerObject(const BundleActiveReportHandlerObject& orig);
    ~BundleActiveReportHandlerObject() {}
};

class BundleActiveReportHandler;

class BundleActiveCore : public BundleActiveStatsUpdateListener,
    public std::enable_shared_from_this<BundleActiveCore>  {
public:
    BundleActiveCore();
    virtual ~BundleActiveCore();

    /*
    * function: ReportEvent, used to report ability fourground/background/destroy event.
    * parameters: event, userId
    */
    int32_t ReportEvent(BundleActiveEvent& event, int32_t userId);

    /*
    * function: ReportEventToAllUserId, report flush to disk, end_of_day event to service.
    * parameters: event
    */
    int32_t ReportEventToAllUserId(BundleActiveEvent& event);

    /*
    * function: OnStatsChanged, report flush to disk, end_of_day event to service.
    * parameters: userId
    */
    void OnStatsChanged(const int32_t userId) override;

    /*
    * function: OnStatsChanged, when device reboot after more than one day, BundleActiveUserService
    * will use it to flush group info.
    */
    void OnStatsReload() override;

    /*
    * function: OnSystemUpdate, now is emtpy, later will called when system is updated.
    * parameters: userId
    */
    void OnSystemUpdate(int32_t userId) override;

    /*
    * function: OnBundleUninstalled when received a PACKATE_REMOVED commen event,
    * BundleActiveCommonEventSubscriber call it to remove data.
    * parameters: userId, bundleName
    */
    void OnBundleUninstalled(const int32_t userId, const std::string& bundleName);

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
    * parameters: reportHandler
    */
    void SetHandler(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);

    /*
    * function: RestoreToDatabase, restore bundle usage data and form data to database
    * parameters: userId
    */
    void RestoreToDatabase(const int32_t userId);

    /*
    * function: RestoreToDatabaseLocked, flush database for one user data
    * parameters: userId
    */
    void RestoreToDatabaseLocked(const int32_t userId);

    /*
    * function: ShutDown, called when device shutdown, update the in-memory stat and flush the database.
    */
    void ShutDown();
    /*
    * function: PreservePowerStateInfo, called when device change power state, preserve power state info.
    */
    void PreservePowerStateInfo(const int32_t eventId);

    /*
    * function: queryBundleStatsInfos, query the package stat for calling user.
    * parameters: userId, intervalType, beginTime, endTime, bundleName
    * return: vector of BundleActivePackageStats
    */
    ErrCode QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& packageStats, const int32_t userId,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime, std::string bundleName);

    // query the event stat for calling user.
    ErrCode QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvent, const int32_t userId,
        const int64_t beginTime, const int64_t endTime, std::string bundleName);

    // check the app idle state for calling user.
    int32_t IsBundleIdle(const std::string& bundleName, const int32_t userId);

    // query the app group for calling app.
    ErrCode QueryAppGroup(int32_t& appGroup, const std::string& bundleName, const int32_t userId);

    ErrCode QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results, int32_t userId);

    /*
    * function: QueryDeviceEventStats, query all from event stats in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId);

    /*
    * function: QueryNotificationEventStats, query all app notification number in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId);

    // get the wall time and check if the wall time is changed.
    int64_t CheckTimeChangeAndGetWallTime(int32_t userId = 0);

    // convert event timestamp from boot based time to wall time.
    void ConvertToSystemTimeLocked(BundleActiveEvent& event);

    // get or create BundleActiveUserService object for specifice user.
    std::shared_ptr<BundleActiveUserService> GetUserDataAndInitializeIfNeeded(const int32_t userId,
        const int64_t timeStamp, const bool debug);

    // when received a USER_REMOVED commen event, call it to remove data.
    void OnUserRemoved(const int32_t userId);

    // when user switched, restore old userdata.
    void OnUserSwitched(const int32_t userId);

    /*
    * function: SetAppGroup, change bundleGroup to the newGroup.
    * parameters: bundleName, newGroup, userId, isFlush,
    * return: errorcode.
    */
    ErrCode SetAppGroup(
        const std::string& bundleName, const int32_t newGroup, const int32_t userId, const bool isFlush);

    // get all user in device.
    void GetAllActiveUser(std::vector<int32_t>& activatedOsAccountIds);

    // when service stop, call it to unregister commen event and shutdown call back.
    void UnRegisterSubscriber();

    // get system time in MS.
    int64_t GetSystemTimeMs();

    /*
    * function: RegisterAppGroupCallBack, register the observer to groupObservers.
    * parameters: observer
    * return: errCode.
    */
    ErrCode RegisterAppGroupCallBack(const AccessToken::AccessTokenID& tokenId,
        const sptr<IAppGroupCallback> &observer);

    /*
    * function: UnRegisterAppGroupCallBack, remove the observer from groupObservers.
    * parameters: observer
    * return: errCode.
    */
    ErrCode UnRegisterAppGroupCallBack(const AccessToken::AccessTokenID& tokenId,
        const sptr<IAppGroupCallback> &observer);

    int32_t currentUsedUser_;
    void OnAppGroupChanged(const AppGroupCallbackInfo& callbackInfo);

private:
    void AddObserverDeathRecipient(const sptr<IAppGroupCallback> &observer);
    void RemoveObserverDeathRecipient(const sptr<IAppGroupCallback> &observer);
    void OnObserverDied(const wptr<IRemoteObject> &remote);
    void OnObserverDiedInner(const wptr<IRemoteObject> &remote);
    int64_t flushInterval_;
    static const int64_t TIME_CHANGE_THRESHOLD_MILLIS = TWO_SECONDS;
    const int32_t DEFAULT_USER_ID = -1;
    std::map<int32_t, std::string> visibleActivities_;
    // use weak_ptr to avoid circulate reference of core and handler.
    std::weak_ptr<BundleActiveReportHandler> handler_;
    std::shared_ptr<BundleActiveGroupController> bundleGroupController_;
    std::shared_ptr<BundleActiveGroupHandler> bundleGroupHandler_;
    int64_t systemTimeShot_;
    int64_t realTimeShot_;
    std::mutex mutex_;
    std::mutex callbackMutex_;
    std::mutex deathRecipientMutex_;
    std::map<int32_t, std::shared_ptr<BundleActiveUserService>> userStatServices_;
    void RegisterSubscriber();
    std::shared_ptr<BundleActiveCommonEventSubscriber> commonEventSubscriber_;
    void RestoreAllData();
    std::map<AccessToken::AccessTokenID, sptr<IAppGroupCallback>> groupChangeObservers_;
    std::map<sptr<IRemoteObject>, sptr<RemoteDeathRecipient>> recipientMap_;
    void ObtainSystemEventName(BundleActiveEvent& event);
    bool debugCore_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CORE_H


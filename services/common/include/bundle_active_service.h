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

#ifndef BUNDLE_ACTIVE_SERVICE_H
#define BUNDLE_ACTIVE_SERVICE_H

#include "bundle_mgr_interface.h"
#include "singleton.h"

#include "ibundle_active_service.h"
#include "bundle_active_stub.h"
#include "bundle_active_core.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_shutdown_callback_service.h"
#include "bundle_active_power_state_callback_service.h"
#include "bundle_active_app_state_observer.h"
#include "bundle_active_continuous_task_observer.h"
#include "bundle_active_account_helper.h"
#include "file_ex.h"
#include "string_ex.h"
#include "system_ability.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveService : public SystemAbility, public BundleActiveStub,
    public std::enable_shared_from_this<BundleActiveService> {
    DISALLOW_COPY_AND_MOVE(BundleActiveService);
    DECLARE_SYSTEM_ABILITY(BundleActiveService);
    DECLARE_DELAYED_SINGLETON(BundleActiveService);
public:
    using IBundleMgr = OHOS::AppExecFwk::IBundleMgr;
    using BundleInfo = OHOS::AppExecFwk::BundleInfo;
    using BundleFlag = OHOS::AppExecFwk::BundleFlag;
    using HapModuleInfo = OHOS::AppExecFwk::HapModuleInfo;
    using AbilityInfo = OHOS::AppExecFwk::AbilityInfo;
    using ApplicationInfo = OHOS::AppExecFwk::ApplicationInfo;
    using ApplicationFlag = OHOS::AppExecFwk::ApplicationFlag;
    using AbilityType = OHOS::AppExecFwk::AbilityType;

    /*
    * function: ReportEvent, used to report event.
    * parameters: event, userId
    * return: errorcode.
    */
    ErrCode ReportEvent(BundleActiveEvent& event, const int32_t userId) override;

    /*
    * function: IsBundleIdle, used to check whether specific bundle is idle.
    * parameters: bundleName
    * return: if bundle is idle, return true. if bundle is not idle, return false.
    */
    ErrCode IsBundleIdle(bool& isBundleIdle, const std::string& bundleName, int32_t userId) override;

    /*
    * function: QueryBundleStatsInfoByInterval, query all usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    ErrCode QueryBundleStatsInfoByInterval(std::vector<BundleActivePackageStats>& PackageStats,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime, int32_t userId) override;

    /*
    * function: QueryBundleEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: vector of events.
    */
    ErrCode QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents, const int64_t beginTime,
        const int64_t endTime, int32_t userId) override;

    /*
    * function: SetAppGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId
    */
    ErrCode SetAppGroup(const std::string& bundleName, int32_t newGroup, int32_t userId) override;
    /*
    * function: QueryBundleStatsInfos, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime
    * return: vector of calling bundle usage statistics.
    */
    ErrCode QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& bundleActivePackageStats,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime) override;

    /*
    * function: QueryCurrentBundleEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime
    * return: vector of calling bundle events.
    */
    ErrCode QueryCurrentBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents,
        const int64_t beginTime, const int64_t endTime) override;

    /*
    * function: QueryAppGroup, query bundle priority group calling bundle.
    * return: the priority group of calling bundle.
    */
    ErrCode QueryAppGroup(int32_t& appGroup, std::string& bundleName, const int32_t userId) override;

    /*
    * function: QueryModuleUsageRecords, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum, results, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
        int32_t userId = -1) override;

    /*
    * function: QueryDeviceEventStates, query all from event stats in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryDeviceEventStates(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId) override;

    /*
    * function: QueryNotificationNumber, query all app notification number in specific time span for calling user.
    * parameters: beginTime, endTime, eventStats, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId.
    * return: errorcode.
    */
    ErrCode QueryNotificationNumber(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId) override;

    /*
    * function: BundleActiveService, default constructor.
    * parameters: systemAbilityId, runOnCreate
    */
    BundleActiveService(const int32_t systemAbilityId, bool runOnCreate);

    /*
    * function: RegisterAppGroupCallBack, register the observer to groupObservers.
    * parameters: observer
    * return: errorcode.
    */
    int32_t RegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer) override;

    /*
    * function: UnRegisterAppGroupCallBack, remove the observer from groupObservers.
    * parameters: observer
    * return: errorcode.
    */
    int32_t UnRegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

protected:
    /**
     * @brief The OnStart callback.
     */
    void OnStart() override;
    /**
     * @brief The OnStop callback.
     */
    void OnStop() override;

private:
    std::shared_ptr<BundleActiveCore> bundleActiveCore_;
    std::shared_ptr<BundleActiveReportHandler> reportHandler_;
    std::shared_ptr<BundleActiveAppStateObserver> appStateObserver_;
#ifdef BGTASKMGR_ENABLE
    std::shared_ptr<BundleActiveContinuousTaskObserver> continuousTaskObserver_;
#endif
    sptr<IBundleMgr> sptrBundleMgr_;
    sptr<BundleActiveShutdownCallbackService> shutdownCallback_;
    sptr<BundleActivePowerStateCallbackService> powerStateCallback_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    bool ready_ {false};
    int32_t ConvertIntervalType(const int32_t intervalType);
    void InitNecessaryState();
    void InitService();
    ErrCode GetBundleMgrProxy();
    ErrCode CheckBundleIsSystemAppAndHasPermission(const int32_t uid,
        OHOS::Security::AccessToken::AccessTokenID tokenId);
    ErrCode CheckSystemAppOrNativePermission(const int32_t uid, OHOS::Security::AccessToken::AccessTokenID tokenId);
    void InitAppStateSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    void InitContinuousSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    bool SubscribeAppState();
    bool SubscribeContinuousTask();
    OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance();
    void QueryModuleRecordInfos(BundleActiveModuleRecord& moduleRecord);
    void SerModuleProperties(const HapModuleInfo& hapModuleInfo,
    const ApplicationInfo& appInfo, const AbilityInfo& abilityInfo, BundleActiveModuleRecord& moduleRecord);
    void DumpUsage(std::string &result);
    int32_t ShellDump(const std::vector<std::string> &dumpOption, std::vector<std::string> &dumpInfo);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_SERVICE_H


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
#include "bundle_active_app_state_observer.h"
#include "bundle_active_continuous_task_observer.h"
#include "bundle_active_account_helper.h"

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
    int32_t ReportEvent(BundleActiveEvent& event, const int32_t userId) override;
    /*
    * function: IsBundleIdle, used to check whether specific bundle is idle.
    * parameters: bundleName
    * return: if bundle is idle, return true. if bundle is not idle, return false.
    */
    bool IsBundleIdle(const std::string& bundleName) override;
    /*
    * function: QueryPackageStats, query all bundle usage statistics in specific time span for calling user.
    * parameters: intervalType, beginTime, endTime, errCode
    * return: vector of bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryPackageStats(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime, int32_t& errCode, int32_t userId = -1) override;
    /*
    * function: QueryEvents, query all events in specific time span for calling user.
    * parameters: beginTime, endTime, errCode
    * return: vector of events.
    */
    std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime,
        int32_t& errCode, int32_t userId = -1) override;
    /*
    * function: SetBundleGroup, set specific bundle of specific user to a priority group.
    * parameters: bundleName, newGroup, userId
    */
    bool SetBundleGroup(const std::string& bundleName, int32_t newGroup, int32_t& errCode, int32_t userId) override;
    /*
    * function: QueryCurrentPackageStats, query bundle usage statistics in specific time span for calling bundle.
    * parameters: intervalType, beginTime, endTime
    * return: vector of calling bundle usage statistics.
    */
    std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int32_t intervalType, const int64_t beginTime,
        const int64_t endTime) override;
    /*
    * function: QueryCurrentEvents, query bundle usage statistics in specific time span for calling bundle.
    * parameters: beginTime, endTime
    * return: vector of calling bundle events.
    */
    std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t beginTime, const int64_t endTime) override;
    /*
    * function: QueryPackageGroup, query bundle priority group calling bundle.
    * return: the priority group of calling bundle.
    */
    int32_t QueryPackageGroup(const std::string& bundleName, int32_t userId) override;
    /*
    * function: QueryFormStatistics, query all from usage statistics in specific time span for calling user.
    * parameters: maxNum, results, userId, default userId is -1 for JS API,
    * if other SAs call this API, they should explicit define userId
    * return: errorcode.
    */
    int32_t QueryFormStatistics(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results,
        int32_t userId = -1) override;
    /*
    * function: BundleActiveService, default constructor.
    * parameters: systemAbilityId, runOnCreate
    */
    BundleActiveService(const int32_t systemAbilityId, bool runOnCreate);
    /*
    * function: RegisterGroupCallBack, register the observer to groupObservers.
    * parameters: observer
    * return: result of RegisterGroupCallBack, true or false.
    */
    bool RegisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer) override;
    /*
    * function: UnregisterGroupCallBack, remove the observer from groupObservers.
    * parameters: observer
    * return: result of UnregisterGroupCallBack, true or false.
    */
    bool UnregisterGroupCallBack(const sptr<IBundleActiveGroupCallback> &observer) override;

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
    std::shared_ptr<BundleActiveContinuousTaskObserver> continuousTaskObserver_;
    sptr<IBundleMgr> sptrBundleMgr_;
    sptr<BundleActiveShutdownCallbackService> shutdownCallback_;
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<AppExecFwk::EventHandler> handler_;
    bool ready_ {false};
    int32_t ConvertIntervalType(const int32_t intervalType);
    void InitNecessaryState();
    void InitService();
    bool GetBundleMgrProxy();
    bool CheckBundleIsSystemAppAndHasPermission(const int32_t uid, OHOS::Security::AccessToken::AccessTokenID tokenId,
        int32_t& errCode);
    void InitAppStateSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    void InitContinuousSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    bool SubscribeAppState();
    bool SubscribeContinuousTask();
    OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance();
    void QueryModuleRecordInfos(BundleActiveModuleRecord& moduleRecord);
    void SerModuleProperties(const HapModuleInfo& hapModuleInfo,
    const ApplicationInfo& appInfo, const AbilityInfo& abilityInfo, BundleActiveModuleRecord& moduleRecord);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_SERVICE_H


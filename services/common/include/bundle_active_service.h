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
#include "os_account_manager.h"

#include "ibundle_active_service.h"
#include "bundle_active_stub.h"
#include "bundle_active_core.h"
#include "bundle_active_report_handler.h"
#include "bundle_active_shutdown_callback_service.h"
#include "bundle_active_app_state_observer.h"
#include "bundle_active_continuous_task_observer.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveService : public SystemAbility, public BundleActiveStub {
    DECLARE_SYSTEM_ABILITY(BundleActiveService);
public:
    using IBundleMgr = OHOS::AppExecFwk::IBundleMgr;
    using ApplicationInfo = OHOS::AppExecFwk::ApplicationInfo;
    using BundleInfo = OHOS::AppExecFwk::BundleInfo;
    using BundleFlag = OHOS::AppExecFwk::BundleFlag;
    int ReportEvent(std::string& bundleName, std::string& abilityName, std::string abilityId,
        const std::string& continuousTask, const int userId, const int eventId) override;
    bool IsBundleIdle(const std::string& bundleName) override;
    std::vector<BundleActivePackageStats> QueryPackageStats(const int intervalType, const int64_t beginTime,
        const int64_t endTime) override;
    std::vector<BundleActiveEvent> QueryEvents(const int64_t beginTime, const int64_t endTime) override;
    void SetBundleGroup(const std::string& bundleName, int newGroup, int userId) override;
    std::vector<BundleActivePackageStats> QueryCurrentPackageStats(const int intervalType, const int64_t beginTime,
        const int64_t endTime) override;
    std::vector<BundleActiveEvent> QueryCurrentEvents(const int64_t beginTime, const int64_t endTime) override;
    int QueryPackageGroup() override;
    BundleActiveService(int32_t systemAbilityId, int runOnCreate)
        : SystemAbility(systemAbilityId, runOnCreate) {}
    ~BundleActiveService() {}

protected:
    void OnStart() override;
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
    int ConvertIntervalType(const int intervalType);
    void InitNecessaryState();
    bool GetBundleMgrProxy();
    bool CheckBundleIsSystemAppAndHasPermission(const int uid, const int userId);
    void InitAppStateSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    void InitContinuousSubscriber(const std::shared_ptr<BundleActiveReportHandler>& reportHandler);
    bool SubscribeAppState();
    bool SubscribeContinuousTask();
    OHOS::sptr<OHOS::AppExecFwk::IAppMgr> GetAppManagerInstance();
};
}
}
#endif
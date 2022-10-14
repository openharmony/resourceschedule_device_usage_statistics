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

#ifndef BUNDLE_ACTIVE_CLIENT_H
#define BUNDLE_ACTIVE_CLIENT_H

#include <mutex>

#include "ibundle_active_service.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_module_record.h"
#include "bundle_state_inner_errors.h"
#include "event_handler.h"
#include "event_runner.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveClient {
public:
    // max number of query modules result.
    const int32_t MAXNUM_UP_LIMIT = 1000;
    
    /**
     * @brief ReportEvent, used to report event.
     *
     * @param event .
     * @param userId .
     * @return errCode.
     */
    ErrCode ReportEvent(BundleActiveEvent event, const int32_t userId);

    /**
     * @brief IsBundleIdle, used to check whether specific bundle is idle.
     *
     * @param isBundleIdle The result of IsBundleIdle.
     * @param bundleName .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode IsBundleIdle(bool& isBundleIdle, const std::string& bundleName, int32_t userId = -1);

    /**
     * @brief QueryBundleStatsInfoByInterval, query all bundle usage statistics in specific time span for calling user.
     *
     * @param bundleActivePackageStats .
     * @param intervalType .
     * @param beginTime .
     * @param endTime .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode QueryBundleStatsInfoByInterval(std::vector<BundleActivePackageStats>& bundleActivePackageStats,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime, int32_t userId = -1);

    /**
     * @brief QueryBundleEvents, query all events in specific time span for calling user.
     *
     * @param bundleActiveEvents .
     * @param beginTime .
     * @param endTime .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents, const int64_t beginTime,
        const int64_t endTime, int32_t userId = -1);

    /**
     * @brief SetAppGroup, set specific bundle of specific user to a priority group.
     *
     * @param bundleName .
     * @param newGroup .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode SetAppGroup(std::string bundleName, const int32_t newGroup, int32_t userId = -1);

    /**
     * @brief QueryBundleStatsInfos, query bundle usage statistics in specific time span for calling bundle.
     *
     * @param bundleActivePackageStats bundleActivePackageStats, The result of QueryBundleStatsInfos.
     * @param intervalType .
     * @param beginTime .
     * @param endTime .
     * @return errCode.
     */
    ErrCode QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& bundleActivePackageStats,
        const int32_t intervalType, const int64_t beginTime, const int64_t endTime);

    /**
     * @brief QueryCurrentBundleEvents, query bundle usage statistics in specific time span for calling bundle.
     *
     * @param bundleActiveEvents the std::vector<BundleActiveEvent>, as the result of QueryCurrentBundleEvents.
     * @param beginTime .
     * @param endTime .
     * @return errCode.
     */
    ErrCode QueryCurrentBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvents,
        const int64_t beginTime, const int64_t endTime);

    /**
     * @brief QueryAppGroup, query appGroup by bundleName and userId.
     *
     * @param appGroup as the result of QueryAppGroup.
     * @param bundleName .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode QueryAppGroup(int32_t& appGroup, std::string& bundleName, const int32_t userId = -1);

    /**
     * @brief QueryModuleUsageRecords, query all from usage statistics in specific time span for calling user.
     *
     * @param maxNum .
     * @param results .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode QueryModuleUsageRecords(int32_t maxNum,
        std::vector<BundleActiveModuleRecord>& results, int32_t userId = -1);

    /**
     * @brief RegisterAppGroupCallBack, observe bundle group change event.
     *
     * @param observer .
     * @return errCode.
     */
    ErrCode RegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer);

    /**
     * @brief UnRegisterAppGroupCallBack, unobserve bundle group change event.
     *
     * @param observer .
     * @return errCode.
     */
    ErrCode UnRegisterAppGroupCallBack(const sptr<IAppGroupCallback> &observer);

    /**
     * @brief QueryDeviceEventStats, query all from event stats in specific time span for calling user.
     *
     * @param beginTime .
     * @param endTime .
     * @param eventStats .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId = -1);

    /**
     * @brief QueryNotificationEventStats, query all app notification number in specific time span for calling user.
     *
     * @param beginTime .
     * @param endTime .
     * @param eventStats .
     * @param userId default userId is -1 for JS API, if other SAs call this API, they should explicit define userId.
     * @return errCode.
     */
    ErrCode QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
        std::vector<BundleActiveEventStats>& eventStats, int32_t userId = -1);
    /*
    * function: GetInstance, get single instance of client.
    * return: object of BundleActiveClient.
    */
    static BundleActiveClient& GetInstance();
private:
    class BundleActiveClientDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        /*
        * function: BundleActiveClientDeathRecipient, default constructor.
        */
        BundleActiveClientDeathRecipient() = default;

        /*
        * function: ~BundleActiveClientDeathRecipient, default destructor.
        */
        ~BundleActiveClientDeathRecipient() = default;

        /*
        * function: AddObserver.
        */
        void AddObserver(const sptr<IAppGroupCallback> &observer);

        /*
        * function: RemoveObserver.
        */
        void RemoveObserver();

        /*
        * function: OnRemoteDied, PostTask when service(bundleActiveProxy_) is died.
        */
        void OnRemoteDied(const wptr<IRemoteObject> &object) override;

        /*
        * function: OnServiceDiedInner, get bundleActiveProxy_ and RegisterAppGroupCallBack again.
        */
        void OnServiceDiedInner();

    private:
        sptr<IAppGroupCallback> observer_ = nullptr;
    };
private:
    ErrCode GetBundleActiveProxy();
    BundleActiveClient() {}
    ~BundleActiveClient() {}
    sptr<IBundleActiveService> bundleActiveProxy_;
    sptr<BundleActiveClientDeathRecipient> recipient_;
    std::shared_ptr<AppExecFwk::EventRunner> bundleClientRunner_ {nullptr};
    std::shared_ptr<AppExecFwk::EventHandler> bundleClientHandler_ {nullptr};
    std::mutex mutex_;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CLIENT_H


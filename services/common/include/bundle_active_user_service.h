/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_ACTIVE_USER_SERVICE_H
#define BUNDLE_ACTIVE_USER_SERVICE_H

#include "bundle_active_iservice.h"
#include "bundle_active_event.h"
#include "bundle_active_usage_stats.h"
#include "bundle_active_interval_stats.h"
#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveUserService {
public:
    BundleActiveUserService(int userId);//,/*database定义待补充*/ BundleActiveService listener/*刷数据库监听器接口实现类*/);
    virtual void onStatsUpdated() = 0;
    virtual void onstatsReloaded() = 0;
    virtual void onNewUpdate(int userId) = 0;
private:
    //BundleActiveUsageDatabase m_dataBase;
    int m_incrementBundleLaunch;
    int m_userId;
    std::vector<BundleActiveIntervalStats> m_currentStats;
    bool m_statsChanged;
    std::string m_lastBackgroundBundle;
    inline static const std::vector<long long> INTERVAL_LENGTH = {BundleActiveIntervalStats::DAY_IN_MILLIS, BundleActiveIntervalStats::WEEK_IN_MILLIS, 
                                                      BundleActiveIntervalStats::MONTH_IN_MILLIS, BundleActiveIntervalStats::YEAR_IN_MILLIS};
    void NotifyStatsChanged();
    void ReportEvent(BundleActiveEvent event);
};
}
}
#endif
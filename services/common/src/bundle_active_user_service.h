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
#include "bundle_active_package_stats.h"
#include "bundle_active_period_stats.h"
#include "bundle_active_event_stats.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveUserService {
public:
    BundleActiveUserService(int userId);//,/*database定义待补充*/ BundleActiveService listener/*刷数据库监听器接口实现类*/);
private:
    //BundleActiveUsageDatabase m_dataBase;
    int incrementBundleLaunch_;
    int userId_;
    std::vector<BundleActivePeriodStats> currentStats_;
    bool statsChanged_;
    std::string lastBackgroundBundle_;
    inline static const std::vector<int64_t> PERIOD_LENGTH = {BundleActivePeriodStats::DAY_IN_MILLIS, BundleActivePeriodStats::WEEK_IN_MILLIS, 
                                                      BundleActivePeriodStats::MONTH_IN_MILLIS, BundleActivePeriodStats::YEAR_IN_MILLIS};
    void NotifyStatsChanged();
    void ReportEvent(BundleActiveEvent event);
};
}
}
#endif
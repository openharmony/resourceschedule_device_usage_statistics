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
    //BundleActiveService m_listener;
    inline static const std::vector<long long> INTERVAL_LENGTH = {BundleActiveIntervalStats::DAY_IN_MILLIS, BundleActiveIntervalStats::WEEK_IN_MILLIS, 
                                                      BundleActiveIntervalStats::MONTH_IN_MILLIS, BundleActiveIntervalStats::YEAR_IN_MILLIS};
    void NotifyStatsChanged();
    void ReportEvent(BundleActiveEvent event);


};

}
}
#endif
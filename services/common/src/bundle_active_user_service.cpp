#include "bundle_active_user_service.h"

namespace OHOS {
namespace BundleActive {

BundleActiveUserService::BundleActiveUserService(int userId){//,/*database定义待补充*/ BundleActiveService listener/*刷数据库监听器接口实现类*/) {
    m_currentStats.reserve(BundleActiveIntervalStats::INTERVAL_COUNT);
    //m_listener = listener;
    m_userId = userId;
}
void BundleActiveUserService::NotifyStatsChanged() {
    if (!m_statsChanged) {
        m_statsChanged = true;
        //m_listener.onStatsUpdated();
    }
}

void BundleActiveUserService::ReportEvent(BundleActiveEvent event) {
    BundleActiveIntervalStats currentDailyStats = m_currentStats[BundleActiveIntervalStats::INTERVAL_DAILY];
    if (event.m_eventId == BundleActiveEvent::ABILITY_FOREGROUND) {
        if (!event.m_bundleName.empty() && event.m_bundleName != m_lastBackgroundBundle) {
            m_incrementBundleLaunch = true;
        }
    } else if (event.m_eventId == BundleActiveEvent::ABILITY_BACKGROUND) {
        if (!event.m_bundleName.empty()) {
            m_lastBackgroundBundle = event.m_bundleName;
        }
    }
    for (int i = 0; i < m_currentStats.size(); i++) {
        switch (event.m_eventId)
        {
            case BundleActiveEvent::SCREEN_INTERACTIVE:
                m_currentStats[i].UpdateScreenInteractive(event.m_timeStamp);
                break;
            case BundleActiveEvent::SCREEN_NON_INTERACTIVE:
                m_currentStats[i].UpdateScreenNonInteractive(event.m_timeStamp);
                break;
            case BundleActiveEvent::KEYGUARD_SHOWN:
                m_currentStats[i].UpdateKeyguardShown(event.m_timeStamp);
                break;
            case BundleActiveEvent::KEYGUARD_HIDDEN:
                m_currentStats[i].UpdateKeyguardHidden(event.m_timeStamp);
                break;
            default:
                m_currentStats[i].Update(event.m_bundleName, event.m_serviceName, event.m_timeStamp, event.m_eventId, event.m_abilityId);
                if (m_incrementBundleLaunch) {
                    m_currentStats[i].m_bundleStats[event.m_bundleName].IncrementBundleLaunchedCount();
                }
                break;
        }
    }
}

}
}
#ifndef BUNDLE_ACTIVE_EVENT_H
#define BUNDLE_ACTIVE_EVENT_H

#include "bundle_active_iservice.h"

namespace OHOS {
namespace BundleActive {

class BundleActiveEvent {
public:
    //external events
    //static const int ABILITY_START = 1;//onStart() called, ability is not in front, but still visible
    static const int ABILITY_FOREGROUND = 2;//onForeground() called, ability is in front.
    static const int ABILITY_BACKGROUND = 3;//onBackground() called, ability is in background.
    static const int ABILITY_STOP = 4;//onStop() called, ability is destroyed.
    static const int FRONT_SERVICE_STARTTED = 5;
    //static const int FRONT_SERVICE_CONTINUED = 11;
    static const int FRONT_SERVICE_STOPPED = 6;
    static const int SYSTEM_INTERACTIVE = 7;
    static const int USER_INTERACTIVE = 8;
    
    //internal events 
    static const int END_OF_THE_DAY = 9; 
    //static const int CONTINUE_PREVIOUS_DAY = 21;
    static const int DEVICE_SHUTDOWN = 10;
    static const int DEVICE_STARTUP = 11;
    static const int FLUSH_TO_DISK = 12;
    static const int SCREEN_INTERACTIVE = 13;
    static const int SCREEN_NON_INTERACTIVE = 14;
    static const int KEYGUARD_SHOWN = 15;
    static const int KEYGUARD_HIDDEN = 16;
    inline static const std::string DEVICE_EVENT_PACKAGE_NAME = "openharmony";
    std::string m_bundleName;
    std::string m_serviceName;
    std::string m_abilityName;
    int m_abilityId;
    long m_timeStamp;
    int m_eventId;
    bool m_isIdle;

public:
    BundleActiveEvent() {};
    BundleActiveEvent(const BundleActiveEvent& orig);
    BundleActiveEvent(int eventId, long timeStamp);
    std::string GetBundleName();
    std::string GetAbilityName();
    int GetAbilityId();
    long GetTimeStamp();
    int GetEventId();
    bool GetIsIdle();
};

}
}
#endif
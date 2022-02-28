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

#ifndef BUNDLE_ACTIVE_EVENT_H
#define BUNDLE_ACTIVE_EVENT_H

#include "ibundle_active_service.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveEvent : public Parcelable {
public:
    // external events
    static const int ABILITY_FOREGROUND = 2; // onForeground() called, ability is in front.
    static const int ABILITY_BACKGROUND = 3; // onBackground() called, ability is in background.
    static const int ABILITY_STOP = 4; // onStop() called, ability is destroyed.
    static const int LONG_TIME_TASK_STARTTED = 5;
    static const int LONG_TIME_TASK_ENDED = 6;
    static const int SYSTEM_INTERACTIVE = 7;
    static const int USER_INTERACTIVE = 8;
    // internal events
    static const int END_OF_THE_DAY = 9;
    static const int SHUTDOWN = 10;
    static const int STARTUP = 11;
    static const int FLUSH = 12;
    static const int SCREEN_INTERACTIVE = 13;
    static const int SCREEN_NON_INTERACTIVE = 14;
    static const int KEYGUARD_SHOWN = 15;
    static const int KEYGUARD_HIDDEN = 16;
    static const int NOTIFICATION_SEEN = 17;
    inline static const std::string DEVICE_EVENT_PACKAGE_NAME = "openharmony";
    std::string bundleName_;
    std::string continuousTaskAbilityName_;
    std::string abilityName_;
    std::string abilityId_;
    int64_t timeStamp_;
    int eventId_;
    bool isidle_;

public:
    BundleActiveEvent() {};
    BundleActiveEvent(const BundleActiveEvent& orig);
    BundleActiveEvent(int eventId, int64_t timeStamp);
    std::string GetBundleName();
    std::string GetAbilityName();
    std::string GetAbilityId();
    int64_t GetTimeStamp();
    int GetEventId();
    bool GetIsIdle();
    virtual bool Marshalling(Parcel &parcel) const override;
    std::shared_ptr<BundleActiveEvent> Unmarshalling(Parcel &parcel);
};
}
}
#endif
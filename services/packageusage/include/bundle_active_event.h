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
    static const int DEFAULT_EVENT_ID = 0;
    static const int END_OF_THE_DAY = 9;
    static const int SHUTDOWN = 10;
    static const int STARTUP = 11;
    static const int FLUSH = 12;
    static const int SCREEN_INTERACTIVE = 13;
    static const int SCREEN_NON_INTERACTIVE = 14;
    static const int KEYGUARD_SHOWN = 15;
    static const int KEYGUARD_HIDDEN = 16;
    static const int NOTIFICATION_SEEN = 17;
    static const int FORM_IS_CLICKED = 18;
    static const int FORM_IS_REMOVED = 19;
    inline static const std::string DEVICE_EVENT_PACKAGE_NAME = "openharmony";
    std::string bundleName_;
    std::string continuousTaskAbilityName_;
    std::string abilityName_;
    std::string abilityId_;
    std::string moduleName_;
    std::string formName_;
    int32_t formDimension_;
    int64_t formId_;
    int64_t timeStamp_;
    int eventId_;

public:
    /*
    * function: BundleActiveEvent, default constructor.
    */
    BundleActiveEvent();
    /*
    * function: BundleActiveEvent, copy constructor.
    * parameters: orig
    */
    BundleActiveEvent(const BundleActiveEvent& orig);
    /*
    * function: BundleActiveEvent, constructor using event Id, time stamp.
    * parameters: eventId, timeStamp
    */
    BundleActiveEvent(int eventId, int64_t timeStamp);
    /*
    * function: BundleActiveEvent, constructor of continuous task event.
    * parameters: bundleName, continuousTaskAbilityName
    */
    BundleActiveEvent(const std::string bundleName, const std::string continuousTaskAbilityName);
    /*
    * function: BundleActiveEvent, constructor of app ability event.
    * parameters: bundleName, abilityName, abilityId
    */
    BundleActiveEvent(const std::string bundleName, const std::string abilityName, const std::string abilityId,
        const std::string moduleName);
    /*
    * function: BundleActiveEvent, constructor of form event.
    * parameters: bundleName, moduleName, formName, formDimension, formId, eventId
    */
    BundleActiveEvent(const std::string bundleName, const std::string moduleName,
        const std::string formName, const int32_t formDimension, const int64_t formId, const int eventId);
    void PrintEvent(const bool debug) const;
    /*
    * function: operator=, override operator =.
    * parameters: orig
    * return: a BundleActiveEvent object same as orig.
    */
    BundleActiveEvent& operator=(const BundleActiveEvent& orig);
    /*
    * function: Marshalling, mashalling event object to parcel.
    * parameters: parcel
    * return: result of mashalling, true means successful, flase means failed.
    */
    virtual bool Marshalling(Parcel &parcel) const override;
    /*
    * function: UnMarshalling, Unmashalling event object from parcel.
    * parameters: parcel
    * return: point to a BundleActiveEvent.
    */
    std::shared_ptr<BundleActiveEvent> UnMarshalling(Parcel &parcel);

    std::string ToString();
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_EVENT_H


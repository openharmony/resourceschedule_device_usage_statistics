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

#ifndef BUNDLE_ACTIVE_USAGE_STATS_H
#define BUNDLE_ACTIVE_USAGE_STATS_H

#include "bundle_active_iservice.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {
//bundles statistics, including activities and frontservices.
class BundleActiveUsageStats {
public:
        std::string m_bundleName;
        long m_beginTimeStamp; //start time of counting
        long m_endTimeStamp; //stop time of counting
        long m_lastTimeUsed; //the timestamp of last launch
        long m_totalTimeInFront; // the total time of using the bundle
        long m_lastTimeFrontServiceUsed;
        long m_totalTimeFrontServiceUsed;
        int m_launchedCount;
        int m_bundleLaunchedCount;
        int m_lastEvent;
        std::map<int, int> m_abilities; // key is abilityId, value is the last event of this ability. Restore all abilities' last event of bundle.
        std::map<std::string, int> m_frontServices; // restore the frontservices' names using by bundle.
        BundleActiveUsageStats() {};
        BundleActiveUsageStats(const BundleActiveUsageStats& orig);
        std::string GetBundleName();
        long GetBeginTimeStamp();
        long GetEntTimeStamp();
        long GetLastTimeUsed();
        long GetTotalTimeInFront();
        long GetLastTimeFrontServiceUsed();
        long GetTotalTimeFrontServiceUsed();
        int GetLaunchedCount();
        int GetBundleLaunchedCount();
        void Update(std::string frontServiceName, long timeStamp, int eventId, int abilityId);
        void IncrementTimeUsed(long timeStamp);
        void IncrementServiceTimeUsed(long timeStamp);
        void IncrementBundleLaunchedCount();

private:
        bool HasFrontAbility();
        bool AnyFrontServiceStarted();
        void UpdateActivity(long timeStamp, int eventId, int abilityId);
        void UpdateFrontService(std::string frontServiceName, long timeStamp, int eventId);


};
}
}
#endif

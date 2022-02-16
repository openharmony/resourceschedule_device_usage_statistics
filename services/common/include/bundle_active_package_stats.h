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

#ifndef bundle_active_package_stats_H
#define bundle_active_package_stats_H

#include "bundle_active_iservice.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {
class BundleActivePackageStats {
public:
        std::string bundleName_;
        int64_t beginTimeStamp_;
        int64_t endTimeStamp_;
        int64_t lastTimeUsed_;
        int64_t totalTimeInFront_;
        int64_t lastTimeLongTimeTaskUsed_;
        int64_t totalTimeLongTimeTaskUsed_;
        int launchedCount_;
        int bundleLaunchedCount_;
        int lastEvent_;
        std::map<int, int> abilities_;
        std::map<std::string, int> frontServices_;
        BundleActivePackageStats() {};
        BundleActivePackageStats(const BundleActivePackageStats& orig);
        std::string GetBundleName();
        int64_t GetBeginTimeStamp();
        int64_t GetEntTimeStamp();
        int64_t GetLastTimeUsed();
        int64_t GetTotalTimeInFront();
        int64_t GetLastTimeFrontServiceUsed();
        int64_t GetTotalTimeFrontServiceUsed();
        int GetLaunchedCount();
        int GetBundleLaunchedCount();
        void Update(std::string longTimeTaskName, const int64_t timeStamp, const int eventId, const int abilityId);
        void IncrementTimeUsed(const int64_t timeStamp);
        void IncrementLongTimeTaskTimeUsed(const int64_t timeStamp);
        void IncrementBundleLaunchedCount();

private:
        bool HasFrontAbility();
        bool AnyFrontServiceStarted();
        void UpdateAbility(const int64_t timeStamp, const int eventId, const int abilityId);
        void UpdateLongTimeTask(std::string longTimeTaskName, 
                                const int64_t timeStamp, const int eventId);


};
}
}
#endif

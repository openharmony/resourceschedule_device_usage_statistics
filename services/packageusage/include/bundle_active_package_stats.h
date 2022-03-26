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

#ifndef BUNDLE_ACTIVE_PACKAGE_STATS_H
#define BUNDLE_ACTIVE_PACKAGE_STATS_H

#include "ibundle_active_service.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActivePackageStats : public Parcelable {
public:
        std::string bundleName_;
        int64_t beginTimeStamp_; // start time of counting
        int64_t endTimeStamp_; // stop time of counting
        int64_t lastTimeUsed_; // the timestamp of last launch
        int64_t totalInFrontTime_; // the total time of using the bundle
        int64_t lastContiniousTaskUsed_;
        int64_t totalContiniousTaskUsedTime_;
        int startCount_;
        int bundleStartedCount_;
        int lastEvent_;
        // key is abilityId, value is the last event of this ability. Restore all abilities' last event of bundle.
        std::map<std::string, int> abilities_;
        // key is name of continuous task, value is last event of this last continuous task.
        std::map<std::string, int> longTimeTasks_;
        BundleActivePackageStats();
        ~BundleActivePackageStats() {}
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
        void Update(const std::string& longTimeTaskName, const int64_t timeStamp, const int eventId,
                const std::string& abilityId);
        void IncrementTimeUsed(const int64_t timeStamp);
        void IncrementServiceTimeUsed(const int64_t timeStamp);
        void IncrementBundleLaunchedCount();
        virtual bool Marshalling(Parcel &parcel) const override;
        std::shared_ptr<BundleActivePackageStats> UnMarshalling(Parcel &parcel);
        void printdata();

private:
        bool HasFrontAbility();
        bool AnyLongTimeTaskStarted();
        void UpdateAbility(const int64_t timeStamp, const int eventId, const std::string& abilityId);
        void UpdateLongTimeTask(const std::string& longTimeTaskName, const int64_t timeStamp, const int eventId);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_PACKAGE_STATS_H


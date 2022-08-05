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

#include <cstdint>
#include <iosfwd>
#include <map>
#include <memory>
#include "parcel.h"

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
    int32_t startCount_;
    int32_t bundleStartedCount_;
    int32_t lastEvent_;
    // key is abilityId, value is the last event of this ability. Restore all abilities' last event of bundle.
    std::map<std::string, int32_t> abilities_;
    // key is name of continuous task, value is last event of this last continuous task.
    std::map<std::string, int32_t> longTimeTasks_;
    /*
    * function: BundleActivePackageStats, default constructor.
    */
    BundleActivePackageStats();
    /*
    * function: ~BundleActivePackageStats, default destructor.
    */
    ~BundleActivePackageStats() {}
    /*
    * function: BundleActivePackageStats, copy constructor.
    * parameters: orig
    */
    BundleActivePackageStats(const BundleActivePackageStats& orig);
    /*
    * function: Update, update one bundle statistics.
    * parameters: longTimeTaskName timeStamp eventId abilityId
    */
    void Update(const std::string& longTimeTaskName, const int64_t timeStamp, const int32_t eventId,
        const std::string& abilityId);
    /*
    * function: IncrementTimeUsed, increase bundle's use time.
    * parameters: timeStamp
    */
    void IncrementTimeUsed(const int64_t timeStamp);
    /*
    * function: IncrementServiceTimeUsed, increase bundle's continuous task use time.
    * parameters: timeStamp
    */
    void IncrementServiceTimeUsed(const int64_t timeStamp);
    /*
    * function: IncrementBundleLaunchedCount, increase bundle's launched times by 1.
    */
    void IncrementBundleLaunchedCount();
    /*
    * function: Marshalling, mashalling BundleActivePackageStats object to parcel.
    * parameters: parcel
    * return: result of mashalling, true means successful, flase means failed.
    */
    virtual bool Marshalling(Parcel &parcel) const override;
    /*
    * function: UnMarshalling, Unmashalling BundleActivePackageStats object from parcel.
    * parameters: parcel
    * return: point to a BundleActivePackageStats.
    */
    std::shared_ptr<BundleActivePackageStats> UnMarshalling(Parcel &parcel);
    /*
    * function: ToString, change module record object to string.
    * return: string of bundle name, last used time, total front time, last continuous task used time,
    * total continuous task time.
    */
    std::string ToString();

private:
    bool HasFrontAbility();
    bool AnyLongTimeTaskStarted();
    void UpdateAbility(const int64_t timeStamp, const int32_t eventId, const std::string& abilityId);
    void UpdateLongTimeTask(const std::string& longTimeTaskName, const int64_t timeStamp, const int32_t eventId);
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_PACKAGE_STATS_H


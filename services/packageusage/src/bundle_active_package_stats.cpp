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

#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActivePackageStats::BundleActivePackageStats()
{
    bundleName_.clear();
    beginTimeStamp_ = 0; // start time of counting
    endTimeStamp_ = 0; // stop time of counting
    lastTimeUsed_ = -1; // the timestamp of last launch
    totalInFrontTime_ = 0; // the total time of bundle in front.
    lastContiniousTaskUsed_ = -1; // the timestamp of bundle calling a continuous task.
    totalContiniousTaskUsedTime_ = 0; // the total time of bundle use continuous tasks.
    startCount_ = 0;
    bundleStartedCount_ = 0;
    lastEvent_ = 0;
}

BundleActivePackageStats::BundleActivePackageStats (const BundleActivePackageStats& orig)
{
    bundleName_ = orig.bundleName_;
    beginTimeStamp_ = orig.beginTimeStamp_;
    endTimeStamp_ = orig.endTimeStamp_;
    lastTimeUsed_ = orig.lastTimeUsed_;
    lastContiniousTaskUsed_ = orig.lastContiniousTaskUsed_;
    totalContiniousTaskUsedTime_ = orig.totalContiniousTaskUsedTime_;
    totalInFrontTime_ = orig.totalInFrontTime_;
    startCount_ = orig.startCount_;
    bundleStartedCount_ = orig.bundleStartedCount_;
    abilities_ = orig.abilities_;
    longTimeTasks_ = orig.longTimeTasks_;
    lastEvent_ = orig.lastEvent_;
}

bool BundleActivePackageStats::HasFrontAbility()
{
    for (auto ability : abilities_) {
        if (ability.second == BundleActiveEvent::ABILITY_FOREGROUND) {
            return true;
        }
    }
    return false;
}

bool BundleActivePackageStats::AnyLongTimeTaskStarted()
{
    return !longTimeTasks_.empty();
}

void BundleActivePackageStats::IncrementTimeUsed(const int64_t timeStamp)
{
    if (timeStamp > lastTimeUsed_) {
        totalInFrontTime_ += timeStamp - lastTimeUsed_;
        lastTimeUsed_ = timeStamp;
    }
}

void BundleActivePackageStats::IncrementServiceTimeUsed(const int64_t timeStamp)
{
    if (timeStamp > lastContiniousTaskUsed_) {
        totalContiniousTaskUsedTime_ += timeStamp - lastContiniousTaskUsed_;
        lastContiniousTaskUsed_ = timeStamp;
    }
}

void BundleActivePackageStats::IncrementBundleLaunchedCount()
{
    bundleStartedCount_ += 1;
}

void BundleActivePackageStats::UpdateAbility(const int64_t timeStamp, const int32_t eventId,
    const std::string& abilityId)
{
    if (eventId != BundleActiveEvent::ABILITY_FOREGROUND && eventId != BundleActiveEvent::ABILITY_BACKGROUND &&
        eventId != BundleActiveEvent::ABILITY_STOP) {
            return;
        }
    if (abilities_.empty() && eventId == BundleActiveEvent::ABILITY_FOREGROUND) {
        beginTimeStamp_ = timeStamp;
    }
    std::map<std::string, int>::iterator it = abilities_.find(abilityId);
    if (it != abilities_.end()) {
        int32_t lastEventId = it->second;
        // When we receive a new event, first update the time stats according to the last event in map.
        switch (lastEventId) {
            case BundleActiveEvent::ABILITY_FOREGROUND:
                IncrementTimeUsed(timeStamp);
                break;
            default:
                break;
        }
    }

    switch (eventId) {
        case BundleActiveEvent::ABILITY_FOREGROUND:
            if (!HasFrontAbility()) {
                lastTimeUsed_ = timeStamp;
            }
            abilities_[abilityId] = eventId;
            break;
        case BundleActiveEvent::ABILITY_BACKGROUND:
            abilities_[abilityId] = eventId;
            break;
        case BundleActiveEvent::ABILITY_STOP:
            abilities_.erase(abilityId);
            break;
        default:
            break;
    }
}

void BundleActivePackageStats::UpdateLongTimeTask(const std::string& longTimeTaskName, const int64_t timeStamp,
    const int32_t eventId)
{
    if (eventId != BundleActiveEvent::LONG_TIME_TASK_STARTTED && eventId != BundleActiveEvent::LONG_TIME_TASK_ENDED) {
        return;
    }

    // When we receive a new event, first update the time stats according to the last service event in map.
    std::map<std::string, int>::iterator it = longTimeTasks_.find(longTimeTaskName);
    if (it != longTimeTasks_.end()) {
        int32_t lastEventId = it->second;
        switch (lastEventId) {
            case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
                IncrementServiceTimeUsed(timeStamp);
                break;
            default:
                break;
        }
    }

    switch (eventId) {
        case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
            if (!AnyLongTimeTaskStarted()) {
                lastContiniousTaskUsed_ = timeStamp;
            }
            longTimeTasks_[longTimeTaskName] = eventId;
            break;
        case BundleActiveEvent::LONG_TIME_TASK_ENDED:
            longTimeTasks_.erase(longTimeTaskName);
            break;
        default:
            break;
    }
}

void BundleActivePackageStats::Update(const std::string& longTimeTaskName, const int64_t timeStamp,
    const int32_t eventId, const std::string& abilityId)
{
    switch (eventId) {
        case BundleActiveEvent::ABILITY_FOREGROUND:
        case BundleActiveEvent::ABILITY_BACKGROUND:
        case BundleActiveEvent::ABILITY_STOP:
            UpdateAbility(timeStamp, eventId, abilityId);
            break;
        case BundleActiveEvent::END_OF_THE_DAY:
            if (HasFrontAbility()) {
                IncrementTimeUsed(timeStamp);
            }
            if (AnyLongTimeTaskStarted()) {
                IncrementServiceTimeUsed(timeStamp);
            }
            break;
        case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
        case BundleActiveEvent::LONG_TIME_TASK_ENDED:
            UpdateLongTimeTask(longTimeTaskName, timeStamp, eventId);
            break;
        case BundleActiveEvent::SHUTDOWN:
        case BundleActiveEvent::FLUSH:
            if (HasFrontAbility()) {
                IncrementTimeUsed(timeStamp);
            }
            if (AnyLongTimeTaskStarted()) {
                IncrementServiceTimeUsed(timeStamp);
            }
            break;
        default:
            break;
    }
    endTimeStamp_ = timeStamp;
    if (eventId == BundleActiveEvent::ABILITY_FOREGROUND) {
        startCount_ += 1;
    }
}

bool BundleActivePackageStats::Marshalling(Parcel &parcel) const
{
    if (parcel.WriteString(bundleName_) &&
        parcel.WriteInt64(beginTimeStamp_) &&
        parcel.WriteInt64(lastTimeUsed_) &&
        parcel.WriteInt64(totalInFrontTime_) &&
        parcel.WriteInt64(lastContiniousTaskUsed_) &&
        parcel.WriteInt64(totalContiniousTaskUsedTime_)) {
        return true;
    }
    return false;
}

std::shared_ptr<BundleActivePackageStats> BundleActivePackageStats::UnMarshalling(Parcel &parcel)
{
    std::shared_ptr<BundleActivePackageStats> result = std::make_shared<BundleActivePackageStats>();
    result->bundleName_ = parcel.ReadString();
    result->beginTimeStamp_ = parcel.ReadInt64();
    result->lastTimeUsed_ = parcel.ReadInt64();
    result->totalInFrontTime_ = parcel.ReadInt64();
    result->lastContiniousTaskUsed_ = parcel.ReadInt64();
    result->totalContiniousTaskUsedTime_ = parcel.ReadInt64();
    return result;
}

std::string BundleActivePackageStats::ToString()
{
    return "bundle name is " + this->bundleName_ +
            ", last used time stamp is " + std::to_string(this->lastTimeUsed_) +
            ", total time in front is " + std::to_string(this->totalInFrontTime_) +
            ", last continuous task used time is " + std::to_string(this->lastContiniousTaskUsed_) +
            ", total continuous task time is " +
            std::to_string(this->totalContiniousTaskUsedTime_) + "\n";
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


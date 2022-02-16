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

#include "bundle_active_package_stats.h"

namespace OHOS {
namespace BundleActive {
BundleActivePackageStats::BundleActivePackageStats (const BundleActivePackageStats& orig)
{
    bundleName_ = orig.bundleName_;
    beginTimeStamp_ = orig.beginTimeStamp_;
    endTimeStamp_ = orig.endTimeStamp_;
    lastTimeUsed_ = orig.lastTimeUsed_;
    lastTimeLongTimeTaskUsed_ = orig.lastTimeLongTimeTaskUsed_;
    totalTimeLongTimeTaskUsed_ = orig.totalTimeLongTimeTaskUsed_;
    totalTimeInFront_ = orig.totalTimeInFront_;
    launchedCount_ = orig.launchedCount_;
    bundleLaunchedCount_ = orig.bundleLaunchedCount_;
    abilities_ = orig.abilities_;
    frontServices_ = orig.frontServices_;
    lastEvent_ = orig.lastEvent_;
}

std::string BundleActivePackageStats::GetBundleName()
{
    return bundleName_;
}

int64_t BundleActivePackageStats::GetBeginTimeStamp()
{
    return beginTimeStamp_;
}

int64_t BundleActivePackageStats::GetEntTimeStamp()
{
    return endTimeStamp_;
}

int64_t BundleActivePackageStats::GetLastTimeUsed()
{
    return lastTimeUsed_;
}

int64_t BundleActivePackageStats::GetTotalTimeInFront()
{
    return totalTimeInFront_;
}

int64_t BundleActivePackageStats::GetLastTimeFrontServiceUsed()
{
    return lastTimeLongTimeTaskUsed_;
}

int64_t BundleActivePackageStats::GetTotalTimeFrontServiceUsed()
{
    return totalTimeLongTimeTaskUsed_;
}

int BundleActivePackageStats::GetLaunchedCount()
{
    return launchedCount_;
}

int BundleActivePackageStats::GetBundleLaunchedCount()
{
    return bundleLaunchedCount_;
}

bool BundleActivePackageStats::HasFrontAbility() {
    for (auto ability : abilities_) {
        if (ability.second == BundleActiveEvent::ABILITY_FOREGROUND)
        {
            return true;
        }
    }
    return false;
}

bool BundleActivePackageStats::AnyFrontServiceStarted()
{
    return !frontServices_.empty();
}

void BundleActivePackageStats::IncrementTimeUsed(const int64_t timeStamp)
{
    if (timeStamp > lastTimeUsed_) {
        totalTimeInFront_ += timeStamp - lastTimeUsed_;
        lastTimeUsed_ = timeStamp;
    }
}

void BundleActivePackageStats::IncrementLongTimeTaskTimeUsed(const int64_t timeStamp)
{
    if (timeStamp > lastTimeLongTimeTaskUsed_) {
        totalTimeLongTimeTaskUsed_ += timeStamp - lastTimeLongTimeTaskUsed_;
        lastTimeLongTimeTaskUsed_ = timeStamp;
    }
}

void BundleActivePackageStats::IncrementBundleLaunchedCount()
{
    bundleLaunchedCount_ += 1;
}

void BundleActivePackageStats::UpdateAbility(const int64_t timeStamp, const int eventId, const int abilityId)
{
    if (eventId != BundleActiveEvent::ABILITY_FOREGROUND && eventId != BundleActiveEvent::ABILITY_BACKGROUND && 
        eventId != BundleActiveEvent::ABILITY_STOP) {
            return;
        }
    std::map<int, int>::iterator it;
    it = abilities_.find(abilityId);
    if (it != abilities_.end()) {
        int lastEventId = it->second;
        //When we recieve a new event, first update the time stats according to the last event in map.
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
        default:
            break;
    }
}

void BundleActivePackageStats::UpdateLongTimeTask(std::string longTimeTaskName, 
                                                  const int64_t timeStamp, const int eventId)
 {
    if (eventId != BundleActiveEvent::LONG_TIME_TASK_STARTTED && eventId != BundleActiveEvent::LONG_TIME_TASK_STOPPED) {
        return;
    }
    
    //When we recieve a new event, first update the time stats according to the last service event in map.
    std::map<std::string, int>::iterator it;
    it = frontServices_.find(longTimeTaskName);
    if (it != frontServices_.end()) {
        int lastEventId = it->second;
        switch (lastEventId) {
            case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
                IncrementLongTimeTaskTimeUsed(timeStamp);
            default:
                break;
        }
    }

    switch (eventId) {
        case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
            if(!AnyFrontServiceStarted()) {
                lastTimeLongTimeTaskUsed_ = timeStamp;
            }
            frontServices_[longTimeTaskName] = eventId;
            break;
        case BundleActiveEvent::LONG_TIME_TASK_STOPPED:
            frontServices_.erase(longTimeTaskName);
        default:
            break;
    }
}

void BundleActivePackageStats::Update(std::string longTimeTaskName, const int64_t timeStamp, const int eventId, const int abilityId)
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
        case BundleActiveEvent::LONG_TIME_TASK_STARTTED:
        case BundleActiveEvent::LONG_TIME_TASK_STOPPED:
            UpdateLongTimeTask(longTimeTaskName, timeStamp, eventId);
            break;
        case BundleActiveEvent::DEVICE_SHUTDOWN:
        case BundleActiveEvent::FLUSH_TO_DISK:
            if (HasFrontAbility()) {
                IncrementTimeUsed(timeStamp);
            }
            if (AnyFrontServiceStarted()) {
                IncrementLongTimeTaskTimeUsed(timeStamp);
            }
            break;
        default:
            break;
    }
    endTimeStamp_ = timeStamp;
    if (eventId == BundleActiveEvent::ABILITY_FOREGROUND) {
        launchedCount_ += 1;
    }
}
}
}

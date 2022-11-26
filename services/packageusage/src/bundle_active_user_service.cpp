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

#include "bundle_active_user_service.h"
#include "bundle_active_core.h"

namespace OHOS {
namespace DeviceUsageStats {
void BundleActiveUserService::Init(const int64_t timeStamp)
{
    database_.InitDatabaseTableInfo(timeStamp);
    database_.InitUsageGroupDatabase(APP_GROUP_DATABASE_INDEX, true);
    BUNDLE_ACTIVE_LOGI("Init called");
    LoadActiveStats(timeStamp, false, false);
    LoadModuleAndFormStats();
    PrintInMemFormStats(debugUserService_, true);
    PrintInMemPackageStats(0, debugUserService_);
    std::shared_ptr<BundleActivePeriodStats> currentDailyStats = currentStats_[BundleActivePeriodStats::PERIOD_DAILY];
    if (currentDailyStats != nullptr) {
        BundleActiveEvent startupEvent(BundleActiveEvent::STARTUP, timeStamp - ONE_SECOND_MILLISECONDS);
        startupEvent.bundleName_ = BundleActiveEvent::DEVICE_EVENT_PACKAGE_NAME;
        currentDailyStats->AddEvent(startupEvent);
        for (auto it : currentDailyStats->events_.events_) {
            BUNDLE_ACTIVE_LOGI("Init event id is %{public}d, time stamp is %{public}lld",
                it.eventId_, (long long)it.timeStamp_);
        }
        BUNDLE_ACTIVE_LOGI("Init currentDailyStats begintime is %{public}lld, "
            "expire time is %{public}lld", (long long)currentDailyStats->beginTime_,
            (long long)dailyExpiryDate_.GetMilliseconds());
    }
}

void BundleActiveUserService::OnUserRemoved()
{
    database_.OnPackageUninstalled(userId_, "");
}

void BundleActiveUserService::DeleteUninstalledBundleStats(const std::string& bundleName)
{
    for (auto it : currentStats_) {
        if (it != nullptr) {
            if (it->bundleStats_.find(bundleName) != it->bundleStats_.end()) {
                it->bundleStats_.erase(bundleName);
            }
            for (auto eventIter = it->events_.events_.begin(); eventIter != it->events_.events_.end();) {
                if (eventIter->bundleName_ == bundleName) {
                    it->events_.events_.erase(eventIter);
                } else {
                    eventIter++;
                }
            }
        }
    }
    for (auto it : moduleRecords_) {
        if (it.first.find(bundleName) != std::string::npos) {
            moduleRecords_.erase(it.first);
            break;
        }
    }
    database_.OnPackageUninstalled(userId_, bundleName);
}

void BundleActiveUserService::RenewTableTime(int64_t oldTime, int64_t newTime)
{
    BUNDLE_ACTIVE_LOGI("RenewTableTime called");
    BUNDLE_ACTIVE_LOGI("RenewTableTime called current event size is %{public}d", currentStats_[0]->events_.Size());
    database_.RenewTableTime(newTime - oldTime);
}

void BundleActiveUserService::NotifyStatsChanged()
{
    BUNDLE_ACTIVE_LOGI("NotifyStatsChanged stat change is %{public}d, user is %{public}d", statsChanged_, userId_);
    if (!statsChanged_) {
        BUNDLE_ACTIVE_LOGI("NotifyStatsChanged() set stats changed to true");
        statsChanged_ = true;
        listener_.OnStatsChanged(userId_);
    }
}

void BundleActiveUserService::NotifyNewUpdate()
{
    listener_.OnSystemUpdate(userId_);
}

void BundleActiveUserService::ReportEvent(const BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("ReportEvent, B time is %{public}lld, E time is %{public}lld, userId is %{public}d,",
        (long long)currentStats_[0]->beginTime_, (long long)dailyExpiryDate_.GetMilliseconds(), userId_);
    event.PrintEvent(debugUserService_);
    if (event.timeStamp_ >= dailyExpiryDate_.GetMilliseconds()) {
        BUNDLE_ACTIVE_LOGI("ReportEvent later than daily expire, renew data in memory");
        RenewStatsInMemory(event.timeStamp_);
    }
    std::shared_ptr<BundleActivePeriodStats> currentDailyStats = currentStats_[BundleActivePeriodStats::PERIOD_DAILY];
    if (!currentDailyStats) {
        return;
    }
    bool incrementBundleLaunch = false;
    if (event.eventId_ != BundleActiveEvent::SYSTEM_INTERACTIVE && event.eventId_ != BundleActiveEvent::FLUSH) {
            currentDailyStats->AddEvent(event);
        }
    if (event.eventId_ == BundleActiveEvent::ABILITY_FOREGROUND) {
        if (!event.bundleName_.empty() && event.bundleName_ != lastForegroundBundle_) {
            incrementBundleLaunch = true;
            lastForegroundBundle_ = event.bundleName_;
        }
    }
    for (auto it : currentStats_) {
        switch (event.eventId_) {
            case BundleActiveEvent::SCREEN_INTERACTIVE:
                it->UpdateScreenInteractive(event.timeStamp_);
                break;
            case BundleActiveEvent::SCREEN_NON_INTERACTIVE:
                it->UpdateScreenNonInteractive(event.timeStamp_);
                break;
            case BundleActiveEvent::KEYGUARD_SHOWN:
                it->UpdateKeyguardShown(event.timeStamp_);
                break;
            case BundleActiveEvent::KEYGUARD_HIDDEN:
                it->UpdateKeyguardHidden(event.timeStamp_);
                break;
            default:
                it->Update(event.bundleName_, event.continuousTaskAbilityName_, event.timeStamp_, event.eventId_,
                    event.abilityId_);
                if (incrementBundleLaunch) {
                    it->bundleStats_[event.bundleName_]->IncrementBundleLaunchedCount();
                }
                break;
        }
    }
    if (event.eventId_ != BundleActiveEvent::FLUSH) {
        NotifyStatsChanged();
    }
}

void BundleActiveUserService::ReportForShutdown(const BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("ReportForShutdown() called");
    if (event.eventId_ != BundleActiveEvent::SHUTDOWN) {
        return;
    }
    currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->AddEvent(event);
    if (event.timeStamp_ >= dailyExpiryDate_.GetMilliseconds()) {
        BUNDLE_ACTIVE_LOGI(" BundleActiveUserService::ReportEvent later than daily expire");
        RenewStatsInMemory(event.timeStamp_);
    }
    for (auto it : currentStats_) {
        it->Update(event.bundleName_, event.continuousTaskAbilityName_, event.timeStamp_, event.eventId_,
            event.abilityId_);
    }
    BUNDLE_ACTIVE_LOGI("ReportForShutdown called notify");
    NotifyStatsChanged();
}

void BundleActiveUserService::RestoreStats(bool forced)
{
    BUNDLE_ACTIVE_LOGI("RestoreStats() called, userId is %{public}d", userId_);
    if (statsChanged_ || forced) {
        BUNDLE_ACTIVE_LOGI("RestoreStats() stat changed is true");
        for (uint32_t i = 0; i < currentStats_.size(); i++) {
            if (currentStats_[i]) {
                if (!currentStats_[i]->bundleStats_.empty()) {
                    database_.UpdateBundleUsageData(i, *(currentStats_[i]));
                }
                if (!currentStats_[i]->events_.events_.empty() && i == BundleActivePeriodStats::PERIOD_DAILY) {
                    database_.UpdateEventData(i, *(currentStats_[i]));
                }
            }
        }
        if (!moduleRecords_.empty()) {
            database_.UpdateModuleData(userId_, moduleRecords_,
                currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->beginTime_);
        }
        currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->events_.Clear();
        statsChanged_ = false;
        BUNDLE_ACTIVE_LOGI("change statsChanged_ to %{public}d user is %{public}d", statsChanged_, userId_);
    }
}

void BundleActiveUserService::LoadActiveStats(const int64_t timeStamp, const bool& force, const bool& timeChanged)
{
    BUNDLE_ACTIVE_LOGI("LoadActiveStats called");
    BundleActiveCalendar tmpCalendar(0);
    if (debugUserService_ == true) {
        tmpCalendar.ChangeToDebug();
    }
    for (uint32_t intervalType = 0; intervalType < periodLength_.size(); intervalType++) {
        tmpCalendar.SetMilliseconds(timeStamp);
        tmpCalendar.TruncateTo(static_cast<int32_t>(intervalType));
        if (!force && currentStats_[intervalType] != nullptr &&
            currentStats_[intervalType]->beginTime_ == tmpCalendar.GetMilliseconds()) {
            continue;
        }
        std::shared_ptr<BundleActivePeriodStats> stats = database_.GetCurrentUsageData(intervalType, userId_);
        currentStats_[intervalType].reset(); // 当前interval stat置空
        if (stats != nullptr) { // 找出最近的stats
            BUNDLE_ACTIVE_LOGI("LoadActiveStats inter type is %{public}d, "
                "bundle size is %{public}zu", intervalType, stats->bundleStats_.size());
            // 如果当前时间在stats的统计时间范围内，则可以从数据库加载数据
            BUNDLE_ACTIVE_LOGI("interval type is %{public}d, database stat BEGIN time is %{public}lld, "
                "timestamp is %{public}lld, expect end is %{public}lld",
                intervalType, (long long)stats->beginTime_, (long long)timeStamp,
                (long long)stats->beginTime_ + periodLength_[intervalType]);
            if (timeStamp > stats->beginTime_ && timeStamp < stats->beginTime_ + periodLength_[intervalType]) {
                currentStats_[intervalType] = stats;
            }
        }
        if (currentStats_[intervalType] != nullptr) {
            continue;
        }
        BUNDLE_ACTIVE_LOGI("LoadActiveStats [Server]create new interval stats!");
        currentStats_[intervalType] = std::make_shared<BundleActivePeriodStats>();
        currentStats_[intervalType]->userId_ = userId_;
        currentStats_[intervalType]->beginTime_ = tmpCalendar.GetMilliseconds();
        currentStats_[intervalType]->endTime_ = timeStamp;
    }
    statsChanged_ = false;
    // 延长统计时间到第二天0点
    if (timeChanged) {
        dailyExpiryDate_.SetMilliseconds(currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->beginTime_);
    } else {
        dailyExpiryDate_.SetMilliseconds(timeStamp);
    }
    dailyExpiryDate_.IncreaseDays(1);
    if (!timeChanged) {
        dailyExpiryDate_.TruncateToDay();
    }
    listener_.OnStatsReload();
    BUNDLE_ACTIVE_LOGI("LoadActiveStats current expire time is %{public}lld, "
        "begin time is %{public}lld", (long long)dailyExpiryDate_.GetMilliseconds(),
        (long long)tmpCalendar.GetMilliseconds());
}

void BundleActiveUserService::LoadModuleAndFormStats()
{
    database_.LoadModuleData(userId_, moduleRecords_);
    database_.LoadFormData(userId_, moduleRecords_);
}

void BundleActiveUserService::RenewStatsInMemory(const int64_t timeStamp)
{
    std::set<std::string> continueBundles;
    std::map<std::string, std::map<std::string, int>> continueAbilities;
    std::map<std::string, std::map<std::string, int>> continueServices;
    for (std::vector<std::shared_ptr<BundleActivePeriodStats>>::iterator it = currentStats_.begin(); // 更新使用时长
        it != currentStats_.end(); ++it) {
        if (*it == nullptr) {
            continue;
        }
        for (auto bundleUsageStatsPair : (*it)->bundleStats_) {
            if (bundleUsageStatsPair.second == nullptr) {
                continue;
            }
            BundleActivePackageStats bundleUsageStats(*(bundleUsageStatsPair.second));
            if (!bundleUsageStats.abilities_.empty()) {
                continueAbilities[bundleUsageStats.bundleName_] = bundleUsageStats.abilities_;
            }
            if (!bundleUsageStats.longTimeTasks_.empty()) {
                continueServices[bundleUsageStats.bundleName_] = bundleUsageStats.longTimeTasks_;
            }
            (*it)->Update(bundleUsageStats.bundleName_, "", dailyExpiryDate_.GetMilliseconds() - 1,
                BundleActiveEvent::END_OF_THE_DAY, "");

            continueBundles.insert(bundleUsageStats.bundleName_);
            NotifyStatsChanged();
        }
        (*it)->CommitTime(dailyExpiryDate_.GetMilliseconds() - 1);
    }
    RestoreStats(true);
    database_.RemoveOldData(timeStamp);
    LoadActiveStats(timeStamp, false, false); // 新建intervalstat或加载当前数据库数据
    for (std::string continueBundleName : continueBundles) { // 更新所有事件的时间戳到新的begintime
        int64_t beginTime = currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->beginTime_;
        for (std::vector<std::shared_ptr<BundleActivePeriodStats>>::iterator itInterval = currentStats_.begin();
            itInterval != currentStats_.end(); ++itInterval) {
            if (continueAbilities.find(continueBundleName) != continueAbilities.end()) {
                for (std::map<std::string, int>::iterator it = continueAbilities[continueBundleName].begin();
                    it != continueAbilities[continueBundleName].end(); ++it) {
                    if (it->second == BundleActiveEvent::ABILITY_BACKGROUND) {
                        continue;
                    }
                    (*itInterval)->Update(continueBundleName, "", beginTime, it->second, it->first);
                }
            }
            if (continueServices.find(continueBundleName) != continueServices.end()) {
                for (std::map<std::string, int>::iterator it = continueServices[continueBundleName].begin();
                    it != continueServices[continueBundleName].end(); ++it) {
                    (*itInterval)->Update(continueBundleName, it->first, beginTime, it->second, "");
                }
            }
        }
    }
    RestoreStats(true);
}

ErrCode BundleActiveUserService::QueryBundleStatsInfos(std::vector<BundleActivePackageStats>& PackageStats,
    int32_t intervalType, const int64_t beginTime, const int64_t endTime, const int32_t userId,
    const std::string& bundleName)
{
    if (intervalType == BundleActivePeriodStats::PERIOD_BEST) {
        intervalType = database_.GetOptimalIntervalType(beginTime, endTime);
        if (intervalType < 0) {
            intervalType = BundleActivePeriodStats::PERIOD_DAILY;
        }
    }
    if (intervalType < 0 || intervalType >= static_cast<int32_t>(currentStats_.size())) {
        return ERR_USAGE_STATS_INTERVAL_NUMBER;
    }
    auto currentStats = currentStats_[intervalType];
    if (currentStats == nullptr) {
        BUNDLE_ACTIVE_LOGE("current interval stat is null!");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    if (currentStats->endTime_ == 0) {
        if (beginTime > currentStats->beginTime_ + periodLength_[intervalType]) {
            return ERR_QUERY_TIME_OUT_OF_RANGE;
        } else {
            PackageStats = database_.QueryDatabaseUsageStats(intervalType, beginTime, endTime, userId);
            return ERR_OK;
        }
    } else if (beginTime >= currentStats->endTime_) {
        return ERR_QUERY_TIME_OUT_OF_RANGE;
    }
    int64_t truncatedEndTime = std::min(currentStats->beginTime_, endTime);
    PackageStats = database_.QueryDatabaseUsageStats(intervalType, beginTime, truncatedEndTime, userId);
    BUNDLE_ACTIVE_LOGI("Query package data in db PackageStats size is %{public}zu", PackageStats.size());
    PrintInMemPackageStats(intervalType, debugUserService_);
    // if we need a in-memory stats, combine current stats with PackageStats from database.
    if (endTime > currentStats->beginTime_) {
        BUNDLE_ACTIVE_LOGI("QueryBundleStatsInfos need in memory stats");
        for (auto it : currentStats->bundleStats_) {
            if (bundleName.empty()) {
                if ((it.second->totalInFrontTime_ != 0 || it.second->totalContiniousTaskUsedTime_ != 0) &&
                    it.second->lastTimeUsed_ >= beginTime && it.second->lastTimeUsed_ <= endTime) {
                    PackageStats.push_back(*(it.second));
                }
            } else {
                if ((it.second->totalInFrontTime_ != 0 || it.second->totalContiniousTaskUsedTime_ != 0) &&
                    it.second->bundleName_ == bundleName && it.second->lastTimeUsed_ >= beginTime &&
                    it.second->lastTimeUsed_ <= endTime) {
                    PackageStats.push_back(*(it.second));
                }
            }
        }
    }
    return ERR_OK;
}

ErrCode BundleActiveUserService::QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvent,
    const int64_t beginTime, const int64_t endTime, const int32_t userId, const std::string& bundleName)
{
    BUNDLE_ACTIVE_LOGI("QueryBundleEvents called");
    auto currentStats = currentStats_[BundleActivePeriodStats::PERIOD_DAILY];
    if (currentStats == nullptr) {
        BUNDLE_ACTIVE_LOGE("current interval stat is null!");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    if (beginTime >= currentStats->endTime_) {
        return ERR_QUERY_TIME_OUT_OF_RANGE;
    }
    BUNDLE_ACTIVE_LOGI("Query event bundle name is %{public}s", bundleName.c_str());
    bundleActiveEvent = database_.QueryDatabaseEvents(beginTime, endTime, userId, bundleName);
    PrintInMemEventStats(debugUserService_);
    if (currentStats->endTime_ == 0) {
        BUNDLE_ACTIVE_LOGI("QueryBundleEvents result in db is %{public}zu", bundleActiveEvent.size());
        return ERR_OK;
    }
    // if we need a in-memory stats, combine current stats with bundleActiveEvent from database.
    if (endTime > currentStats->beginTime_) {
        BUNDLE_ACTIVE_LOGI("QueryBundleEvents need in memory stats");
        int32_t eventBeginIdx = currentStats->events_.FindBestIndex(beginTime);
        int32_t eventSize = currentStats->events_.Size();
        for (int32_t i = eventBeginIdx; i < eventSize; i++) {
            if (currentStats->events_.events_[i].timeStamp_ <= endTime) {
                if (bundleName.empty() || currentStats->events_.events_[i].bundleName_ == bundleName) {
                    bundleActiveEvent.push_back(currentStats->events_.events_[i]);
                }
            }
        }
    }
    BUNDLE_ACTIVE_LOGI("QueryBundleEvents result in db and memory is %{public}zu", bundleActiveEvent.size());
    return ERR_OK;
}

int32_t BundleActiveUserService::QueryModuleUsageRecords(int32_t maxNum, std::vector<BundleActiveModuleRecord>& results)
{
    BUNDLE_ACTIVE_LOGI("QueryModuleUsageRecords called, MAX IS %{public}d", maxNum);
    for (auto oneModuleRecord = moduleRecords_.begin(); oneModuleRecord != moduleRecords_.end(); oneModuleRecord++) {
        if (!oneModuleRecord->second) {
            continue;
        }
        results.emplace_back(*(oneModuleRecord->second));
    }
    std::sort(results.begin(), results.end(), BundleActiveModuleRecord::cmp);
    if (static_cast<int32_t>(results.size()) > maxNum) {
        results.resize(maxNum);
    }
    for (auto& result : results) {
        std::sort(result.formRecords_.begin(), result.formRecords_.end(), BundleActiveFormRecord::cmp);
    }
    return 0;
}

int32_t BundleActiveUserService::QueryDeviceEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("QueryDeviceEventStats called");
    auto currentStats = currentStats_[BundleActivePeriodStats::PERIOD_DAILY];
    if (currentStats == nullptr) {
        BUNDLE_ACTIVE_LOGE("current interval stat is null!");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    if (beginTime >= currentStats->endTime_) {
        return ERR_TIME_OPERATION_FAILED;
    }
    std::map<std::string, BundleActiveEventStats> systemEventStats;
    database_.QueryDeviceEventStats(BundleActiveEvent::SYSTEM_LOCK, beginTime, endTime, systemEventStats, userId);
    database_.QueryDeviceEventStats(BundleActiveEvent::SYSTEM_UNLOCK, beginTime, endTime, systemEventStats, userId);
    database_.QueryDeviceEventStats(BundleActiveEvent::SYSTEM_SLEEP, beginTime, endTime, systemEventStats, userId);
    database_.QueryDeviceEventStats(BundleActiveEvent::SYSTEM_WAKEUP, beginTime, endTime, systemEventStats, userId);
    BUNDLE_ACTIVE_LOGI("Query eventStats data in db result size is %{public}zu", systemEventStats.size());
    PrintInMemEventStats(debugUserService_);
    // if we need a in-memory stats, combine current stats with result from database.
    if (currentStats->endTime_ != 0 && endTime > currentStats->beginTime_) {
        BUNDLE_ACTIVE_LOGI("QueryDeviceEventStats need in memory stats");
        GetCachedSystemEvents(currentStats, beginTime, endTime, systemEventStats);
    }
    std::map<std::string, BundleActiveEventStats>::iterator iter;
    for (iter = systemEventStats.begin(); iter != systemEventStats.end(); ++iter) {
        eventStats.push_back(iter->second);
    }
    return ERR_OK;
}

void BundleActiveUserService::GetCachedSystemEvents(std::shared_ptr<BundleActivePeriodStats> currentStats,
    int64_t beginTime, int64_t endTime, std::map<std::string, BundleActiveEventStats>& systemEventStats)
{
    int32_t eventBeginIdx = currentStats->events_.FindBestIndex(beginTime);
    int32_t eventSize = currentStats->events_.Size();
    BundleActiveEventStats singleEventStats;
    std::map<std::string, BundleActiveEventStats>::iterator iter;
    for (int32_t i = eventBeginIdx; i < eventSize; i++) {
        if ((currentStats->events_.events_[i].timeStamp_ <= endTime)
            && ((currentStats->events_.events_[i].eventId_== BundleActiveEvent::SYSTEM_LOCK)
            || (currentStats->events_.events_[i].eventId_== BundleActiveEvent::SYSTEM_UNLOCK)
            || (currentStats->events_.events_[i].eventId_== BundleActiveEvent::SYSTEM_SLEEP)
            || (currentStats->events_.events_[i].eventId_== BundleActiveEvent::SYSTEM_WAKEUP))) {
            singleEventStats.name_ = currentStats->events_.events_[i].bundleName_;
            iter = systemEventStats.find(singleEventStats.name_);
            if (iter != systemEventStats.end()) {
                iter->second.count_++;
            } else {
                singleEventStats.eventId_ = currentStats->events_.events_[i].eventId_;
                singleEventStats.count_ = 1;
                systemEventStats.insert(std::pair<std::string, BundleActiveEventStats>(
                    singleEventStats.name_, singleEventStats));
            }
        }
    }
}

int32_t BundleActiveUserService::QueryNotificationEventStats(int64_t beginTime, int64_t endTime,
    std::vector<BundleActiveEventStats>& eventStats, int32_t userId)
{
    BUNDLE_ACTIVE_LOGI("QueryNotificationEventStats called");
    auto currentStats = currentStats_[BundleActivePeriodStats::PERIOD_DAILY];
    if (currentStats == nullptr) {
        BUNDLE_ACTIVE_LOGE("current interval stat is null!");
        return ERR_MEMORY_OPERATION_FAILED;
    }
    if (beginTime >= currentStats->endTime_) {
        return ERR_TIME_OPERATION_FAILED;
    }
    std::map<std::string, BundleActiveEventStats> notificationEventStats;
    database_.QueryNotificationEventStats(BundleActiveEvent::NOTIFICATION_SEEN,
        beginTime, endTime, notificationEventStats, userId);
    BUNDLE_ACTIVE_LOGI("Query eventStats data in db result size is %{public}zu", notificationEventStats.size());
    PrintInMemEventStats(debugUserService_);
    // if we need a in-memory stats, combine current stats with result from database.
    if (currentStats->endTime_ != 0 && endTime > currentStats->beginTime_) {
        BUNDLE_ACTIVE_LOGI("QueryNotificationEventStats need in memory stats");
        GetCachedNotificationEvents(currentStats, beginTime, endTime, notificationEventStats);
    }
    std::map<std::string, BundleActiveEventStats>::iterator iter;
    for (iter = notificationEventStats.begin(); iter != notificationEventStats.end(); ++iter) {
        eventStats.push_back(iter->second);
    }
    return ERR_OK;
}

void BundleActiveUserService::GetCachedNotificationEvents(std::shared_ptr<BundleActivePeriodStats> currentStats,
    int64_t beginTime, int64_t endTime, std::map<std::string, BundleActiveEventStats>& notificationEventStats)
{
    int32_t eventBeginIdx = currentStats->events_.FindBestIndex(beginTime);
    int32_t eventSize = currentStats->events_.Size();
    std::map<std::string, BundleActiveEventStats>::iterator iter;
    BundleActiveEventStats singleEventStats;
    for (int32_t i = eventBeginIdx; i < eventSize; i++) {
        if ((currentStats->events_.events_[i].timeStamp_ <= endTime)
            && (currentStats->events_.events_[i].eventId_== BundleActiveEvent::NOTIFICATION_SEEN)) {
            singleEventStats.name_ = currentStats->events_.events_[i].bundleName_;
            iter = notificationEventStats.find(singleEventStats.name_);
            if (iter != notificationEventStats.end()) {
                iter->second.count_++;
            } else {
                singleEventStats.eventId_ = BundleActiveEvent::NOTIFICATION_SEEN;
                singleEventStats.count_ = 1;
                notificationEventStats.insert(std::pair<std::string, BundleActiveEventStats>(
                    singleEventStats.name_, singleEventStats));
            }
        }
    }
}

void BundleActiveUserService::PrintInMemPackageStats(const int32_t idx, const bool debug)
{
    if (!debug) {
        return;
    }
    BUNDLE_ACTIVE_LOGI("PrintInMemPackageStats called");
    for (auto it : currentStats_[idx]->bundleStats_) {
        BUNDLE_ACTIVE_LOGI("In mem, bundle name is %{public}s", it.first.c_str());
        int64_t lastTimeUsed = it.second->lastTimeUsed_;
        int64_t totalUsedTime = it.second->totalInFrontTime_;
        int64_t lastTimeContinuousTaskUsed = it.second->lastContiniousTaskUsed_;
        int64_t totalTimeContinuousTaskUsed = it.second->totalContiniousTaskUsedTime_;
        BUNDLE_ACTIVE_LOGI("bundle stat is, totaltime is %{public}lld, lastTimeUsed is %{public}lld"
            "total continuous task is %{public}lld, lastTimeContinuousTaskUsed is %{public}lld",
            (long long)totalUsedTime, (long long)lastTimeUsed,
            (long long)totalTimeContinuousTaskUsed, (long long)lastTimeContinuousTaskUsed);
    }
}

void BundleActiveUserService::PrintInMemEventStats(const bool debug)
{
    if (!debug) {
        return;
    }
    BUNDLE_ACTIVE_LOGI("PrintInMemEventStats called");
    int32_t idx = 0;
    int32_t size = static_cast<int32_t>(currentStats_[idx]->events_.events_.size());
    for (int32_t i = 0; i < size; i++) {
        std::string abilityId = currentStats_[idx]->events_.events_[i].abilityId_;
        std::string abilityname = currentStats_[idx]->events_.events_[i].abilityName_;
        std::string bundlename = currentStats_[idx]->events_.events_[i].bundleName_;
        int32_t eventid = currentStats_[idx]->events_.events_[i].eventId_;
        int64_t timestamp = currentStats_[idx]->events_.events_[i].timeStamp_;
        BUNDLE_ACTIVE_LOGI("In mem, event stat is, abilityid is %{public}s, abilityname is %{public}s, "
            "bundlename is %{public}s, eventid is %{public}d, timestamp is %{public}lld",
            abilityId.c_str(), abilityname.c_str(), bundlename.c_str(), eventid, (long long)timestamp);
    }
}

void BundleActiveUserService::PrintInMemFormStats(const bool debug, const bool printform)
{
    if (!debug) {
        return;
    }
    for (const auto& oneModule : moduleRecords_) {
        if (oneModule.second) {
        BUNDLE_ACTIVE_LOGI("bundle name is %{public}s, module name is %{public}s, "
            "lastusedtime is %{public}lld, launchcount is %{public}d", oneModule.second->bundleName_.c_str(),
            oneModule.second->moduleName_.c_str(),
            (long long)oneModule.second->lastModuleUsedTime_, oneModule.second->launchedCount_);
        BUNDLE_ACTIVE_LOGI("combined info is %{public}s", oneModule.first.c_str());
            if (printform) {
                for (const auto& oneForm : oneModule.second->formRecords_) {
                    BUNDLE_ACTIVE_LOGI("form name is %{public}s, form dimension is %{public}d, "
                        "form id is %{public}lld, "
                        "lasttouchtime is %{public}lld, touchcount is %{public}d", oneForm.formName_.c_str(),
                        oneForm.formDimension_, (long long)oneForm.formId_,
                        (long long)oneForm.formLastUsedTime_, oneForm.count_);
                }
            }
        }
    }
}

void BundleActiveUserService::ReportModuleEvent(const BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("ReportModuleEvent called");
    if (event.eventId_ != BundleActiveEvent::ABILITY_FOREGROUND) {
        return;
    }
    auto moduleRecord = GetOrCreateModuleRecord(event);
    moduleRecord->UpdateModuleRecord(event.timeStamp_);
    NotifyStatsChanged();
    PrintInMemFormStats(debugUserService_, false);
}

void BundleActiveUserService::ReportFormEvent(const BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("ReportFormEvent called");
    auto moduleRecord = GetOrCreateModuleRecord(event);
    if (event.eventId_ == BundleActiveEvent::FORM_IS_CLICKED && moduleRecord) {
        moduleRecord->AddOrUpdateOneFormRecord(event.formName_, event.formDimension_, event.formId_, event.timeStamp_);
        NotifyStatsChanged();
    } else if (event.eventId_ == BundleActiveEvent::FORM_IS_REMOVED && moduleRecord) {
        moduleRecord->RemoveOneFormRecord(event.formName_, event.formDimension_, event.formId_);
        database_.RemoveFormData(userId_, event.bundleName_, event.moduleName_, event.formName_, event.formDimension_,
            event.formId_);
    }
    PrintInMemFormStats(debugUserService_, true);
}

std::shared_ptr<BundleActiveModuleRecord> BundleActiveUserService::GetOrCreateModuleRecord(
    const BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGI("GetOrCreateModuleRecord called");
    std::string combinedInfo = event.bundleName_ + " " + event.moduleName_;
    auto it = moduleRecords_.find(combinedInfo);
    if (it == moduleRecords_.end()) {
        auto moduleRecordInserted = std::make_shared<BundleActiveModuleRecord>();
        moduleRecordInserted->bundleName_ = event.bundleName_;
        moduleRecordInserted->moduleName_ = event.moduleName_;
        moduleRecordInserted->userId_ = userId_;
        moduleRecords_[combinedInfo] = moduleRecordInserted;
    }
    return moduleRecords_[combinedInfo];
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
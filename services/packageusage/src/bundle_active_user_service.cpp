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
#include "bundle_active_log.h"
#include "bundle_active_util.h"
#include "bundle_active_bundle_mgr_helper.h"

namespace OHOS {
namespace DeviceUsageStats {
    const int32_t MAIN_APP_INDEX = 0;
void BundleActiveUserService::Init(const int64_t timeStamp)
{
    database_.InitDatabaseTableInfo(timeStamp);
    database_.InitUsageGroupDatabase(APP_GROUP_DATABASE_INDEX, true);
    BUNDLE_ACTIVE_LOGI("Init called");
    LoadActiveStats(timeStamp, false, false);
    LoadModuleAndFormStats();
    PrintInMemFormStats(debugUserService_, true);
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
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
    database_.OnPackageUninstalled(userId_, "", 0, 0);
}

void BundleActiveUserService::DeleteUninstalledBundleStats(const std::string& bundleName, const int32_t uid,
    const int32_t appIndex)
{
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
    for (auto it : currentStats_) {
        if (it != nullptr) {
            DeleteMemUsageStats(it, bundleName, uid, appIndex);
            DeleteMemEvent(it, bundleName, uid, appIndex);
            DeleteMemRecords(it, bundleName, uid, appIndex);
            DeleteMemPackageUidSet(it, bundleName, uid, appIndex);
        }
    }
    database_.OnPackageUninstalled(userId_, bundleName, uid, appIndex);
}

void BundleActiveUserService::DeleteMemUsageStats(const std::shared_ptr<BundleActivePeriodStats>& currentStats,
    const std::string& bundleName, const int32_t deletedUid, const int32_t appIndex)
{
    std::string bundleStatsKey = BundleActiveUtil::GetBundleUsageKey(bundleName, deletedUid);
    if (appIndex != MAIN_APP_INDEX) {
        if (currentStats->bundleStats_.find(bundleStatsKey) != currentStats->bundleStats_.end()) {
            currentStats->bundleStats_.erase(bundleStatsKey);
        }
        return;
    }
    auto uidSet = BundleActiveBundleMgrHelper::GetInstance()->GetPackageUidSet(bundleName);
    for (auto it: uidSet) {
        bundleStatsKey = BundleActiveUtil::GetBundleUsageKey(bundleName, it);
        currentStats->bundleStats_.erase(bundleStatsKey);
    }
}

void BundleActiveUserService::DeleteMemEvent(const std::shared_ptr<BundleActivePeriodStats>& currentStats,
    const std::string& bundleName, const int32_t deletedUid, const int32_t appIndex)
{
    if (appIndex != MAIN_APP_INDEX) {
        for (auto eventIter = currentStats->events_.events_.begin();
            eventIter != currentStats->events_.events_.end();) {
            if (eventIter->bundleName_ == bundleName && eventIter->uid_ == deletedUid) {
                eventIter = currentStats->events_.events_.erase(eventIter);
            } else {
                eventIter++;
            }
        }
        return;
    }
    auto uidSet = BundleActiveBundleMgrHelper::GetInstance()->GetPackageUidSet(bundleName);
    for (auto eventIter = currentStats->events_.events_.begin();
        eventIter != currentStats->events_.events_.end();) {
        if (eventIter->bundleName_ == bundleName && uidSet.find(eventIter->uid_) != uidSet.end()) {
            eventIter = currentStats->events_.events_.erase(eventIter);
        } else {
            eventIter++;
        }
    }
}

void BundleActiveUserService::DeleteMemRecords(const std::shared_ptr<BundleActivePeriodStats>& currentStats,
    const std::string& bundleName, const int32_t deletedUid, const int32_t appIndex)
{
    if (appIndex != MAIN_APP_INDEX) {
        for (auto it = moduleRecords_.begin(); it != moduleRecords_.end();) {
            std::string moduleKey = bundleName + " " + std::to_string(deletedUid);
            if (it->first.find(moduleKey) != std::string::npos) {
                it = moduleRecords_.erase(it);
            } else {
                it++;
            }
        }
        return;
    }
    for (auto it = moduleRecords_.begin(); it != moduleRecords_.end();) {
        if (it->first.find(bundleName) != std::string::npos) {
            it = moduleRecords_.erase(it);
        } else {
            it++;
        }
    }
}

void BundleActiveUserService::DeleteMemPackageUidSet(const std::shared_ptr<BundleActivePeriodStats>& currentStats,
    const std::string& bundleName, const int32_t deletedUid, const int32_t appIndex)
{
    if (appIndex != MAIN_APP_INDEX) {
        BundleActiveBundleMgrHelper::GetInstance()->DeletePackageUid(bundleName, deletedUid);
        return;
    }
    BundleActiveBundleMgrHelper::GetInstance()->DeleteMemPackage(bundleName);
}

void BundleActiveUserService::RenewTableTime(int64_t oldTime, int64_t newTime)
{
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
    BUNDLE_ACTIVE_LOGI("RenewTableTime called current event size is %{public}d", currentStats_[0]->events_.Size());
    database_.RenewTableTime(newTime - oldTime);
}

void BundleActiveUserService::NotifyStatsChanged()
{
    BUNDLE_ACTIVE_LOGD("NotifyStatsChanged stat change is %{public}d, user is %{public}d", statsChanged_, userId_);
    if (!statsChanged_) {
        BUNDLE_ACTIVE_LOGD("NotifyStatsChanged() set stats changed to true");
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
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
    BUNDLE_ACTIVE_LOGD("ReportEvent, B time is %{public}lld, E time is %{public}lld, userId is %{public}d,",
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
    UpdatePeriodStats(event, incrementBundleLaunch);
}

void BundleActiveUserService::UpdatePeriodStats(const BundleActiveEvent& event, const bool& incrementBundleLaunch)
{
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
                    event.abilityId_, event.uid_);
                if (incrementBundleLaunch) {
                    std::string bundleStatsKey = event.bundleName_ + std::to_string(event.uid_);
                    it->bundleStats_[bundleStatsKey]->IncrementBundleLaunchedCount();
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
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
    currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->AddEvent(event);
    if (event.timeStamp_ >= dailyExpiryDate_.GetMilliseconds()) {
        BUNDLE_ACTIVE_LOGI(" BundleActiveUserService::ReportEvent later than daily expire");
        RenewStatsInMemory(event.timeStamp_);
    }
    for (auto it : currentStats_) {
        it->Update(event.bundleName_, event.continuousTaskAbilityName_, event.timeStamp_, event.eventId_,
            event.abilityId_, event.uid_);
    }
    BUNDLE_ACTIVE_LOGI("ReportForShutdown called notify");
    NotifyStatsChanged();
}

void BundleActiveUserService::RestoreStats(bool forced)
{
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
    BUNDLE_ACTIVE_LOGI("RestoreStats() called, userId is %{public}d", userId_);
    if (statsChanged_ || forced) {
        BUNDLE_ACTIVE_LOGI("RestoreStats() stat changed is true");
        for (uint32_t i = 0; i < currentStats_.size(); i++) {
            if (!currentStats_[i]) {
                continue;
            }
            if (!currentStats_[i]->bundleStats_.empty()) {
                database_.UpdateBundleUsageData(i, *(currentStats_[i]));
            }
            if (!currentStats_[i]->events_.events_.empty() && i == BundleActivePeriodStats::PERIOD_DAILY) {
                database_.UpdateEventData(i, *(currentStats_[i]));
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
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
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
            currentStats_[intervalType]->beginTime_ = tmpCalendar.GetMilliseconds();
            currentStats_[intervalType]->endTime_ = timeStamp;
            continue;
        }
        BUNDLE_ACTIVE_LOGI("LoadActiveStats [Server]create new interval stats!");
        currentStats_[intervalType] = std::make_shared<BundleActivePeriodStats>();
        currentStats_[intervalType]->userId_ = userId_;
        currentStats_[intervalType]->beginTime_ = tmpCalendar.GetMilliseconds();
        currentStats_[intervalType]->endTime_ = timeStamp;
    }
    statsChanged_ = false;
    UpdateExpiryDate(timeChanged, tmpCalendar, timeStamp);
    listener_.OnStatsReload();
    BUNDLE_ACTIVE_LOGI("LoadActiveStats current expire time is %{public}lld, "
        "begin time is %{public}lld", (long long)dailyExpiryDate_.GetMilliseconds(),
        (long long)tmpCalendar.GetMilliseconds());
}

void BundleActiveUserService::UpdateExpiryDate(const bool timeChanged,
    BundleActiveCalendar& tmpCalendar, const int64_t timeStamp)
{
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
}

void BundleActiveUserService::LoadModuleAndFormStats()
{
    database_.LoadModuleData(userId_, moduleRecords_);
    database_.LoadFormData(userId_, moduleRecords_);
}

void BundleActiveUserService::FlushDataInMem(std::set<std::string> &continueBundles,
    std::map<std::string, std::map<std::string, int>> &continueAbilities,
    std::map<std::string, std::map<std::string, int>> &continueServices)
{
    for (std::vector<std::shared_ptr<BundleActivePeriodStats>>::iterator it = currentStats_.begin();
        it != currentStats_.end(); ++it) {
        if (*it == nullptr) {
            continue;
        }
        for (auto bundleUsageStatsPair : (*it)->bundleStats_) {
            if (bundleUsageStatsPair.second == nullptr) {
                continue;
            }
            BundleActivePackageStats bundleUsageStats(*(bundleUsageStatsPair.second));
            std::string bundleStatsKey = bundleUsageStats.bundleName_ + std::to_string(bundleUsageStats.uid_);
            if (!bundleUsageStats.abilities_.empty()) {
                continueAbilities[bundleStatsKey] = bundleUsageStats.abilities_;
            }
            if (!bundleUsageStats.longTimeTasks_.empty()) {
                continueServices[bundleStatsKey] = bundleUsageStats.longTimeTasks_;
            }
            (*it)->Update(bundleUsageStats.bundleName_, "", dailyExpiryDate_.GetMilliseconds() - 1,
                BundleActiveEvent::END_OF_THE_DAY, "", bundleUsageStats.uid_);

            continueBundles.insert(bundleUsageStats.bundleName_);
            NotifyStatsChanged();
        }
        (*it)->CommitTime(dailyExpiryDate_.GetMilliseconds() - 1);
    }
}

void BundleActiveUserService::UpdateContinueAbilitiesMemory(const int64_t& beginTime,
    const std::map<std::string, std::map<std::string, int>>& continueAbilities, const std::string& continueBundleName,
    const std::vector<std::shared_ptr<BundleActivePeriodStats>>::iterator& itInterval)
{
    auto uidSet = BundleActiveBundleMgrHelper::GetInstance()->GetPackageUidSet(continueBundleName);
    for (auto uid: uidSet) {
        std::string continueAbilitiesKey = BundleActiveUtil::GetBundleUsageKey(continueBundleName, uid);
        auto ability = continueAbilities.find(continueAbilitiesKey);
        if (ability == continueAbilities.end()) {
            return;
        }
        for (auto it = ability->second.begin(); it != ability->second.end(); ++it) {
            if (it->second == BundleActiveEvent::ABILITY_BACKGROUND) {
                continue;
            }
            (*itInterval)->Update(continueBundleName, "", beginTime, it->second, it->first, uid);
        }
    }
}

void BundleActiveUserService::UpdateContinueServicesMemory(const int64_t& beginTime,
    const std::map<std::string, std::map<std::string, int>>& continueServices, const std::string& continueBundleName,
    const std::vector<std::shared_ptr<BundleActivePeriodStats>>::iterator& itInterval)
{
    auto uidSet = BundleActiveBundleMgrHelper::GetInstance()->GetPackageUidSet(continueBundleName);

    for (auto uid: uidSet) {
        std::string continueServicesKey = BundleActiveUtil::GetBundleUsageKey(continueBundleName, uid);
        auto service = continueServices.find(continueServicesKey);
        if (service == continueServices.end()) {
            return;
        }
        for (auto it = service->second.begin(); it != service->second.end(); ++it) {
            (*itInterval)->Update(continueBundleName, it->first, beginTime, it->second, "", uid);
        }
    }
}

void BundleActiveUserService::RenewStatsInMemory(const int64_t timeStamp)
{
    std::set<std::string> continueBundles;
    std::map<std::string, std::map<std::string, int>> continueAbilities;
    std::map<std::string, std::map<std::string, int>> continueServices;
    // update stat in memory.
    FlushDataInMem(continueBundles, continueAbilities, continueServices);
    RestoreStats(true);
    database_.RemoveOldData(timeStamp);
    // create new stats
    LoadActiveStats(timeStamp, false, false);
    // update timestamps of events in memory
    for (std::string continueBundleName : continueBundles) {
        int64_t beginTime = currentStats_[BundleActivePeriodStats::PERIOD_DAILY]->beginTime_;
        for (std::vector<std::shared_ptr<BundleActivePeriodStats>>::iterator itInterval = currentStats_.begin();
            itInterval != currentStats_.end(); ++itInterval) {
            UpdateContinueAbilitiesMemory(beginTime, continueAbilities, continueBundleName, itInterval);
            UpdateContinueServicesMemory(beginTime, continueServices, continueBundleName, itInterval);
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
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
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
            PackageStats = database_.QueryDatabaseUsageStats(intervalType, beginTime, endTime, userId, bundleName);
            return ERR_OK;
        }
    } else if (beginTime >= currentStats->endTime_) {
        return ERR_QUERY_TIME_OUT_OF_RANGE;
    }
    int64_t truncatedEndTime = std::min(currentStats->beginTime_, endTime);
    PackageStats = database_.QueryDatabaseUsageStats(intervalType, beginTime, truncatedEndTime, userId, bundleName);
    BUNDLE_ACTIVE_LOGI("Query package data in db PackageStats size is %{public}zu", PackageStats.size());
    PrintInMemPackageStats(intervalType, debugUserService_);
    // if we need a in-memory stats, combine current stats with PackageStats from database.
    if (endTime > currentStats->beginTime_) {
        BUNDLE_ACTIVE_LOGI("QueryBundleStatsInfos need in memory stats");
        for (auto it : currentStats->bundleStats_) {
            bool isTimeLegal = (it.second->totalInFrontTime_ != 0 || it.second->totalContiniousTaskUsedTime_ != 0) &&
                it.second->lastTimeUsed_ >= beginTime && it.second->lastTimeUsed_ <= endTime;
            bool isBundleNameEqual = !bundleName.empty() && it.second->bundleName_ == bundleName;
            it.second->userId_ = userId;
            if (bundleName.empty() && isTimeLegal) {
                PackageStats.push_back(*(it.second));
            } else if (isBundleNameEqual && isTimeLegal) {
                PackageStats.push_back(*(it.second));
            }
        }
    }
    return ERR_OK;
}

ErrCode BundleActiveUserService::QueryBundleEvents(std::vector<BundleActiveEvent>& bundleActiveEvent,
    const int64_t beginTime, const int64_t endTime, const int32_t userId, const std::string& bundleName)
{
    BUNDLE_ACTIVE_LOGI("QueryBundleEvents called");
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
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
            if (currentStats->events_.events_[i].timeStamp_ > endTime) {
                continue;
            }
            if (bundleName.empty() || currentStats->events_.events_[i].bundleName_ == bundleName) {
                bundleActiveEvent.push_back(currentStats->events_.events_[i]);
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
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
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
    std::lock_guard<ffrt::recursive_mutex> lock(statsMutex_);
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
        int32_t uid = it.second->uid_;
        BUNDLE_ACTIVE_LOGI("bundle stat is, totaltime is %{public}lld, lastTimeUsed is %{public}lld"
            "total continuous task is %{public}lld, lastTimeContinuousTaskUsed is %{public}lld uid is %{public}d",
            (long long)totalUsedTime, (long long)lastTimeUsed,
            (long long)totalTimeContinuousTaskUsed, (long long)lastTimeContinuousTaskUsed, uid);
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
        int32_t uid = currentStats_[idx]->events_.events_[i].uid_;
        BUNDLE_ACTIVE_LOGI("In mem, event stat is, abilityid is %{public}s, abilityname is %{public}s, "
            "bundlename is %{public}s, eventid is %{public}d, timestamp is %{public}lld, uid is %{public}d",
            abilityId.c_str(), abilityname.c_str(), bundlename.c_str(), eventid, (long long)timestamp, uid);
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
            "lastusedtime is %{public}lld, launchcount is %{public}d, uid is %{public}d",
            oneModule.second->bundleName_.c_str(), oneModule.second->moduleName_.c_str(),
            (long long)oneModule.second->lastModuleUsedTime_, oneModule.second->launchedCount_, oneModule.second->uid_);
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
    BUNDLE_ACTIVE_LOGD("ReportModuleEvent called");
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
    BUNDLE_ACTIVE_LOGD("ReportFormEvent called");
    auto moduleRecord = GetOrCreateModuleRecord(event);
    if (event.eventId_ == BundleActiveEvent::FORM_IS_CLICKED && moduleRecord) {
        moduleRecord->AddOrUpdateOneFormRecord(event.formName_, event.formDimension_, event.formId_,
            event.timeStamp_, event.uid_);
        NotifyStatsChanged();
    } else if (event.eventId_ == BundleActiveEvent::FORM_IS_REMOVED && moduleRecord) {
        moduleRecord->RemoveOneFormRecord(event.formName_, event.formDimension_, event.formId_);
        database_.RemoveFormData(userId_, event.bundleName_, event.moduleName_, event.formName_, event.formDimension_,
            event.formId_, event.uid_);
    }
    PrintInMemFormStats(debugUserService_, true);
}

std::shared_ptr<BundleActiveModuleRecord> BundleActiveUserService::GetOrCreateModuleRecord(
    const BundleActiveEvent& event)
{
    BUNDLE_ACTIVE_LOGD("GetOrCreateModuleRecord called");
    std::string combinedInfo = event.bundleName_ + " " + std::to_string(event.uid_) + " " + event.moduleName_;
    auto it = moduleRecords_.find(combinedInfo);
    if (it == moduleRecords_.end()) {
        auto moduleRecordInserted = std::make_shared<BundleActiveModuleRecord>();
        moduleRecordInserted->bundleName_ = event.bundleName_;
        moduleRecordInserted->moduleName_ = event.moduleName_;
        moduleRecordInserted->userId_ = userId_;
        moduleRecordInserted->uid_ = event.uid_;
        moduleRecords_[combinedInfo] = moduleRecordInserted;
    }
    return moduleRecords_[combinedInfo];
}

void BundleActiveUserService::DeleteExcessiveEventTableData(int32_t deleteDays)
{
    if (deleteDays < 0) {
        return;
    }
    database_.DeleteExcessiveEventTableData(deleteDays);
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
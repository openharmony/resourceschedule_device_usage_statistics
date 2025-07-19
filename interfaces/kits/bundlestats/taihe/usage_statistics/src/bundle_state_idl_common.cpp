/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "bundle_active_log.h"
#include "bundle_active_client.h"
#include "taihe/runtime.hpp"
#include "stdexcept"
#include "bundle_state_idl_common.h"
#include "bundle_state_idl_condition.h"
using namespace ohos::resourceschedule::usageStatistics;
using namespace taihe;
namespace OHOS {
namespace DeviceUsageStats {
const int32_t INTERVAL_TYPE_DEFAULT = 0;
const int64_t INTERVAL_TYPE_YEAR = 4;
const int64_t MAX_TIME = 20000000000000;
const int64_t INTERVAL_TIME = (30 * 24 * 60 * 60 * 1000LL); // 30天

std::shared_ptr<std::map<std::string, BundleActivePackageStats>> BundleStateIDLCommon::QueryBundleStatsInfos(
    int64_t& beginTime, int64_t& endTime, int32_t& errCode)
{
    std::vector<BundleActivePackageStats> packageStats;
    errCode = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats, INTERVAL_TYPE_DEFAULT,
        beginTime, endTime);
    std::shared_ptr<std::map<std::string, BundleActivePackageStats>> mergedPackageStats =
        std::make_shared<std::map<std::string, BundleActivePackageStats>>();
    if (packageStats.empty()) {
        return nullptr;
    }
    for (auto packageStat : packageStats) {
        std::map<std::string, BundleActivePackageStats>::iterator iter =
            mergedPackageStats->find(packageStat.bundleName_);
        if (iter != mergedPackageStats->end()) {
            MergePackageStats(iter->second, packageStat);
        } else {
            mergedPackageStats->
                insert(std::pair<std::string, BundleActivePackageStats>(packageStat.bundleName_, packageStat));
        }
    }
    return mergedPackageStats;
}

void BundleStateIDLCommon::ParseBundleStatsInfo(const BundleActivePackageStats& bundleActivePackageStats,
    BundleStatsInfo& bundleStatsInfo)
{
    bundleStatsInfo.bundleName =
        taihe::optional<string>(std::in_place, taihe::string(bundleActivePackageStats.bundleName_));
    bundleStatsInfo.abilityPrevAccessTime =
        taihe::optional<int64_t>(std::in_place, bundleActivePackageStats.lastTimeUsed_);
    bundleStatsInfo.abilityInFgTotalTime =
        taihe::optional<int64_t>(std::in_place, bundleActivePackageStats.totalInFrontTime_);
    bundleStatsInfo.appIndex =
        taihe::optional<int32_t>(std::in_place, bundleActivePackageStats.appIndex_);
}

void BundleStateIDLCommon::ParseBundleEvents(const BundleActiveEvent& bundleActiveEvent,
    BundleEvents& bundleEvent)
{
    bundleEvent.bundleName =
        taihe::optional<string>(std::in_place, taihe::string(bundleActiveEvent.bundleName_));
    bundleEvent.eventId =
        taihe::optional<int32_t>(std::in_place, bundleActiveEvent.eventId_);
    bundleEvent.eventOccurredTime =
        taihe::optional<int64_t>(std::in_place, bundleActiveEvent.timeStamp_);
}

void BundleStateIDLCommon::MergePackageStats(BundleActivePackageStats& left, const BundleActivePackageStats& right)
{
    if (left.bundleName_ != right.bundleName_) {
        BUNDLE_ACTIVE_LOGE("Merge package stats failed, existing packageName : %{public}s,"
            " new packageName : %{public}s,", left.bundleName_.c_str(), right.bundleName_.c_str());
        return;
    }
    left.lastTimeUsed_ = std::max(left.lastTimeUsed_, right.lastTimeUsed_);
    left.lastContiniousTaskUsed_ = std::max(left.lastContiniousTaskUsed_, right.lastContiniousTaskUsed_);
    left.totalInFrontTime_ += right.totalInFrontTime_;
    left.totalContiniousTaskUsedTime_ += right.totalContiniousTaskUsedTime_;
    left.bundleStartedCount_ += right.bundleStartedCount_;
}

void BundleStateIDLCommon::ParseQueryInfosMap(const taihe::map<taihe::string, taihe::array<int64_t>>& appInfos,
    std::map<std::string, std::vector<int64_t>>& queryInfos)
{
    for (const auto& iter : appInfos) {
        std::vector<int64_t> appIndexVector;
        for (int64_t appIndex : iter.second) {
            appIndexVector.push_back(appIndex);
        }
        queryInfos.emplace(std::string(iter.first.c_str()), appIndexVector);
    }
}

array<HapFormInfo> BundleStateIDLCommon::ParseformRecords(const std::vector<BundleActiveFormRecord>& formRecords)
{
    std::vector<HapFormInfo> HapFormInfoVector;
    for (auto& formRecord : formRecords) {
        HapFormInfo HapFormInfo {
            .formName = formRecord.formName_,
            .formDimension = formRecord.formDimension_,
            .formId = formRecord.formId_,
            .formLastUsedTime = formRecord.formLastUsedTime_,
            .count = formRecord.count_,
        };
        HapFormInfoVector.push_back(HapFormInfo);
    }
    return array<HapFormInfo>(HapFormInfoVector);
}

std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> BundleStateIDLCommon::QueryAppStatsInfos(
    int64_t& beginTime, int64_t& endTime, int32_t& errCode)
{
    std::vector<BundleActivePackageStats> packageStats;
    if (endTime - beginTime <= INTERVAL_TIME) {
        errCode = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats,
            DeviceUsageStats::IntervalType::BY_DAILY, beginTime, endTime);
    } else {
        errCode = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats,
            DeviceUsageStats::IntervalType::BY_OPTIMIZED, beginTime, endTime);
    }
    std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> mergedPackageStats =
        std::make_shared<std::map<std::string, std::vector<BundleActivePackageStats>>>();
    if (packageStats.empty()) {
        return nullptr;
    }
    MergeAppStatsInfos(packageStats, mergedPackageStats);
    return mergedPackageStats;
}

std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> BundleStateIDLCommon::QueryLastUseTime(
    const std::map<std::string, std::vector<int64_t>>& queryInfos, int32_t& errCode)
{
    std::vector<BundleActivePackageStats> packageStats;
    errCode = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(packageStats,
        INTERVAL_TYPE_YEAR, 0, MAX_TIME);
    std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>> mergedPackageStats =
        std::make_shared<std::map<std::string, std::vector<BundleActivePackageStats>>>();
    if (packageStats.empty()) {
        return nullptr;
    }

    std::vector<std::string> tempQueryInfos;
    for (auto queryInfo : queryInfos) {
        for (std::vector<int64_t>::iterator queryInfoIter = queryInfo.second.begin();
            queryInfoIter != queryInfo.second.end(); queryInfoIter++) {
            std::string tempQueryInfo = queryInfo.first + std::to_string(*queryInfoIter);
            tempQueryInfos.push_back(tempQueryInfo);
        }
    }
    std::vector<BundleActivePackageStats> tempPackageStats;
    for (auto tempQueryInfoIter : tempQueryInfos) {
        for (auto packageStat : packageStats) {
            if (tempQueryInfoIter == packageStat.bundleName_ + std::to_string(packageStat.appIndex_)) {
                tempPackageStats.push_back(packageStat);
            }
        }
    }
    MergeAppStatsInfos(tempPackageStats, mergedPackageStats);
    return mergedPackageStats;
}

void BundleStateIDLCommon::MergeAppStatsInfos(const std::vector<BundleActivePackageStats>& packageStats,
    std::shared_ptr<std::map<std::string, std::vector<BundleActivePackageStats>>>& mergedPackageStats)
{
    for (auto packageStat : packageStats) {
        std::map<std::string, std::vector<BundleActivePackageStats>>::iterator iter =
            mergedPackageStats->find(packageStat.bundleName_);
        if (iter == mergedPackageStats->end()) {
            std::vector<BundleActivePackageStats> temp;
            temp.push_back(packageStat);
            mergedPackageStats->insert(std::pair<std::string, std::vector<BundleActivePackageStats>>(
                packageStat.bundleName_, temp));
            continue;
        }
        bool sign = false;
        for (auto& packageMerge : iter->second) {
            if (packageMerge.appIndex_ == packageStat.appIndex_) {
                MergePackageStats(packageMerge, packageStat);
                sign = true;
            }
        }
        if (sign == false) {
            iter->second.push_back(packageStat);
        }
    }
}
}
}
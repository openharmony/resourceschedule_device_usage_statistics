/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include <ctime>
#include <limits>
#include "bundle_active_util.h"
#include "bundle_active_log.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/statfs.h>

namespace OHOS {
namespace DeviceUsageStats {
const int32_t MILLISECOND_TO_MICROSECOND = 1000;
const int32_t MILLISECOND_TO_SECOND = 1000;
const int32_t SECOND_TO_MILLISECOND = 1000;
const int64_t ONE_DAY_TIME = 24 * 60 * 60 *1000LL;
const int64_t WEEK_OFFSET = 6LL;
const int64_t DAYS_OF_WEEK = 7LL;
const int64_t HOUR_OF_MIDNIGHT = 0;
const int64_t MIN_OF_MIDNIGHT = 0;
const int64_t SECOND_TO_MIDNIGHT = 0;
const int64_t STATR_DAY_OF_MON = 1;
const int64_t STATR_MON_OF_YEAR = 0;
const int64_t ERROR_TIME = 0;
std::string BundleActiveUtil::GetBundleUsageKey(const std::string &bundleName, const int32_t uid)
{
    return bundleName + std::to_string(uid);
}

int64_t BundleActiveUtil::GetFFRTDelayTime(const int64_t& delayTime)
{
    return delayTime * MILLISECOND_TO_MICROSECOND;
}

int64_t BundleActiveUtil::GetIntervalTypeStartTime(const int64_t& timeStamp, const int32_t& intervalType)
{
    if (timeStamp <= 0) {
        return ERROR_TIME;
    }
    time_t time = timeStamp / MILLISECOND_TO_SECOND;
    std::tm* tm_time = std::localtime(&time);
    if (tm_time == nullptr) {
        return ERROR_TIME;
    }
    tm_time->tm_hour = HOUR_OF_MIDNIGHT;
    tm_time->tm_min = MIN_OF_MIDNIGHT;
    tm_time->tm_sec = SECOND_TO_MIDNIGHT;
    if (intervalType == PERIOD_WEEKLY) {
        int64_t weekday = tm_time->tm_wday;
        time_t startOfDay = mktime(tm_time) * SECOND_TO_MILLISECOND;
        time_t weekDayTime = (weekday + WEEK_OFFSET) % DAYS_OF_WEEK * ONE_DAY_TIME;
        return startOfDay - weekDayTime;
    }
    switch (intervalType) {
        case PERIOD_DAILY:
            break;
        case PERIOD_MONTHLY:
            tm_time->tm_mday = STATR_DAY_OF_MON;
            break;
        case PERIOD_YEARLY:
            tm_time->tm_mon = STATR_MON_OF_YEAR;
            tm_time->tm_mday = STATR_DAY_OF_MON;
            break;
        default:
            break;
    }
    return mktime(tm_time) * SECOND_TO_MILLISECOND;
}

int32_t BundleActiveUtil::StringToInt32(const std::string& str)
{
    char* pEnd = nullptr;
    errno = 0;
    int64_t res = std::strtol(str.c_str(), &pEnd, 10);
    if (errno == ERANGE || pEnd == str.c_str() || *pEnd != '\0' ||
        (res < std::numeric_limits<int32_t>::min()) ||
        res > std::numeric_limits<int32_t>::max()) {
        return 0;
    }
    return static_cast<int32_t>(res);
}

int64_t BundleActiveUtil::StringToInt64(const std::string& str)
{
    char* pEnd = nullptr;
    errno = 0;
    int64_t res = std::strtol(str.c_str(), &pEnd, 10);
    if (errno == ERANGE || pEnd == str.c_str() || *pEnd != '\0') {
        return 0;
    }
    return res;
}

int64_t BundleActiveUtil::GetSystemTimeMs()
{
    time_t now;
    (void)time(&now);  // unit is seconds.
    if (static_cast<int64_t>(now) < 0) {
        BUNDLE_ACTIVE_LOGE("Get now time error");
        return 0;
    }
    auto tarEndTimePoint = std::chrono::system_clock::from_time_t(now);
    auto tarDuration = std::chrono::duration_cast<std::chrono::milliseconds>(tarEndTimePoint.time_since_epoch());
    int64_t tarDate = tarDuration.count();
    if (tarDate < 0) {
        BUNDLE_ACTIVE_LOGE("tarDuration is less than 0.");
        return -1;
    }
    return static_cast<int64_t>(tarDate);
}

int64_t BundleActiveUtil::GetSteadyTime()
{
    std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch());
    return static_cast<int64_t>(secs.count());
}

int64_t BundleActiveUtil::GetNowMicroTime()
{
    std::chrono::microseconds microSecs;
    // get system time, it can change.
    microSecs = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch());
    return static_cast<int64_t>(microSecs.count());
}

uint64_t BundleActiveUtil::GetFolderOrFileSize(const std::string& path)
{
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return 0;
    }
    if (!S_ISDIR(st.st_mode)) {
        return static_cast<uint64_t>(st.st_size);
    }
    uint64_t totalSize = 0;
    DIR *dir = opendir(path.c_str());
    // input invaild, open failed
    if (dir == nullptr) {
        return 0;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        // current dir is path of '.' or '..'
        std::string entryPath = path + "/" + entry->d_name;
        if (entry->d_name[0] == '.') {
            continue;
        }

        struct stat entryStat;
        if (stat(entryPath.c_str(), &entryStat) != 0) {
            continue;
        }
        if (S_ISDIR(entryStat.st_mode)) {
            uint64_t subDirSize = GetFolderOrFileSize(entryPath);
            totalSize += subDirSize;
        } else {
            totalSize += static_cast<uint64_t>(entryStat.st_size);
        }
    }
    closedir(dir);
    return totalSize;
}

std::string BundleActiveUtil::GetPartitionName(const std::string& path)
{
    std::string partition;
    std::size_t first = path.find_first_of("/");
    if (first == std::string::npos) {
        partition = "/" + path;
        return partition;
    }
    std::size_t second = path.find_first_of("/", first + 1);
    if (second == std::string::npos) {
        if (path.at(0) != '/') {
            partition = "/" + path.substr(0, first);
        } else {
            partition = path;
        }
        return partition;
    }
    partition = path.substr(0, second - first);
    return partition;
}

double BundleActiveUtil::GetDeviceValidSize(const std::string& path)
{
    std::string partitionName = GetPartitionName(path);
    struct statfs stat;
    if (statfs(partitionName.c_str(), &stat) != 0) {
        return 0;
    }
    constexpr double units = 1024.0;
    return (static_cast<double>(stat.f_bfree) / units) * (static_cast<double>(stat.f_bsize) / units);
}

double BundleActiveUtil::GetPercentOfAvailableUserSpace(const std::string& path)
{
    std::string partitionName = GetPartitionName(path);
    struct statfs stat;
    if (statfs(partitionName.c_str(), &stat) != 0) {
        return 0;
    }
    uint64_t totalBytes = static_cast<uint64_t>(stat.f_blocks) * stat.f_bsize;
    uint64_t availableBytes = static_cast<uint64_t>(stat.f_bfree) * stat.f_bsize;
    if (totalBytes == 0 || availableBytes == 0) {
        return 0;
    }
    return static_cast<double>(availableBytes) / static_cast<double>(totalBytes);
}
} // namespace DeviceUsageStats
}  // namespace OHOS


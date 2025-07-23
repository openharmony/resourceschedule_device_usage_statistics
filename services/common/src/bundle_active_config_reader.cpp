/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "bundle_active_config_reader.h"
#include "config_policy_utils.h"
#include "bundle_active_constant.h"
#include "bundle_active_log.h"
#include "file_ex.h"

using namespace std;
namespace OHOS {
namespace DeviceUsageStats {
const static char* CONFIG_PATH = "etc/device_usage_statistics/device_usage_statistics_config.json";
const static char* APPLICATION_USE_PERIODICALLY_KEY = "application_use_periodically";
const static char* MIN_USE_TIMES = "MinUseTimes";
const static char* MAX_USE_TIMES = "MaxUseTimes";
const static char* MIN_USE_DAYS = "MinUseDays";
const static char* MAX_DATA_SIZE = "MaxDataSize";
const static char* APPLICATION_USE_HIGH_FREQUENCY_JUDGE_THRESHOLD =
    "application_use_high_frequency_judge_threshold";
const static char* MIN_TOTAL_USE_DAYS = "MinTotalUseDays";
const static char* MIN_TOP_USE_HOURS_LIMIT = "MinTopUseHoursLimit";
const static char* MIN_HOUR_USE_DAYS = "MinHourUseDays";
const static char* MAX_HIGH_FREQUENCY_HOUR_NUM = "MaxHighFrequencyHourNum";
const int32_t DEFAULT_MIN_USE_TIMES = 1;
const int32_t DEFAULT_MAX_USE_TIMES = 10;
const int32_t DEFAULT_MIN_USE_DAYS = 3;
const int32_t DEFAULT_MIN_TOTAL_USE_DAYS = 4;
const int32_t DEFAULT_TOP_USE_HOURS_LIMIT = 6;
const int32_t DEFAULT_MIN_HOUR_USE_DAYS = 4;
const int32_t DEFAULT_MAX_HIGH_FREQUENCY_HOUR_NUM = 3;
const int32_t MAX_BUFFER = 2048;
const uint64_t DEFAULT_MAX_DATA_SIZE = 5 * 1024 * 1024;


void BundleActiveConfigReader::LoadConfig()
{
    appUsePeriodicallyConfig_ = {DEFAULT_MIN_USE_TIMES, DEFAULT_MAX_USE_TIMES, DEFAULT_MIN_USE_DAYS};
    appHighFreqPeriodThresholdConfig_ = {DEFAULT_MIN_TOTAL_USE_DAYS,
        DEFAULT_TOP_USE_HOURS_LIMIT,
        DEFAULT_MIN_HOUR_USE_DAYS,
        DEFAULT_MAX_HIGH_FREQUENCY_HOUR_NUM};
    auto cfgFiles = GetCfgFiles(CONFIG_PATH);
// LCOV_EXCL_START
    if (!cfgFiles) {
        BUNDLE_ACTIVE_LOGE("GetCfgFiles failed");
        return;
    }
// LCOV_EXCL_STOP
    for (const auto& filePath : cfgFiles->paths) {
        LoadConfigFile(filePath);
    }
    BUNDLE_ACTIVE_LOGI("appUsePeriodicallyConfig minUseTimes:%{public}d, maxUseTimes:%{public}d,"
        "minUseDays:%{public}d maxDataSize:%{public}lu", appUsePeriodicallyConfig_.minUseTimes,
        appUsePeriodicallyConfig_.maxUseTimes, appUsePeriodicallyConfig_.minUseDays,
        static_cast<unsigned long>(maxDataSize_));
    BUNDLE_ACTIVE_LOGI("appHighFreqPeriodThresholdConfig minTotalUseDays:%{public}d, minTopUseHoursLimit:%{public}d,"
        "minHourUseDays:%{public}d,"
        "maxHighFreqHourNum:%{public}d",
        appHighFreqPeriodThresholdConfig_.minTotalUseDays,
        appHighFreqPeriodThresholdConfig_.minTopUseHoursLimit,
        appHighFreqPeriodThresholdConfig_.minHourUseDays,
        appHighFreqPeriodThresholdConfig_.maxHighFreqHourNum);
    FreeCfgFiles(cfgFiles);
}

void BundleActiveConfigReader::LoadConfigFile(const char *filePath)
{
    if (!filePath) {
        BUNDLE_ACTIVE_LOGE("file does no exit");
        return;
    }
    cJSON *root = nullptr;
    if (!GetJsonFromFile(filePath, root) || !root) {
        BUNDLE_ACTIVE_LOGE("file is empty %{private}s", filePath);
        return;
    }
// LCOV_EXCL_START
    LoadApplicationUsePeriodically(root);
    LoadAppHighFreqPeriodThresholdConfig(root);
    LoadMaxDataSize(root);
    cJSON_Delete(root);
// LCOV_EXCL_STOP
}

// LCOV_EXCL_START
void BundleActiveConfigReader::LoadApplicationUsePeriodically(cJSON* root)
{
    cJSON *appUsePeriodicallyRoot = cJSON_GetObjectItem(root, APPLICATION_USE_PERIODICALLY_KEY);
    if (!IsValidObject(appUsePeriodicallyRoot)) {
        BUNDLE_ACTIVE_LOGE("application_use_periodically content is empty");
        return;
    }

    int32_t minUseTimes =
        GetIntValue(appUsePeriodicallyRoot, MIN_USE_TIMES, MINIMUM_LIMIT, INT32_MAX, DEFAULT_MIN_USE_TIMES);
    int32_t maxUseTimes =
        GetIntValue(appUsePeriodicallyRoot, MAX_USE_TIMES, MINIMUM_LIMIT, INT32_MAX, DEFAULT_MAX_USE_TIMES);
    int32_t minUseDays =
        GetIntValue(appUsePeriodicallyRoot, MIN_USE_DAYS, MINIMUM_LIMIT, INT32_MAX, DEFAULT_MIN_USE_DAYS);

    appUsePeriodicallyConfig_ = {minUseTimes, maxUseTimes, minUseDays};
}

void BundleActiveConfigReader::LoadAppHighFreqPeriodThresholdConfig(cJSON* root)
{
    cJSON* appHighFreqPeriodThresholdConfigRoot =
        cJSON_GetObjectItem(root, APPLICATION_USE_HIGH_FREQUENCY_JUDGE_THRESHOLD);
    if (!IsValidObject(appHighFreqPeriodThresholdConfigRoot)) {
        BUNDLE_ACTIVE_LOGE("application_use_high_frequency_judge_threshold content is empty");
        return;
    }
    int32_t minTotalUseDays = GetIntValue(appHighFreqPeriodThresholdConfigRoot,
        MIN_TOTAL_USE_DAYS,
        MINIMUM_LIMIT,
        NUM_DAY_ONE_WEEK,
        DEFAULT_MIN_TOTAL_USE_DAYS);

    int32_t minTopUseHoursLimit = GetIntValue(appHighFreqPeriodThresholdConfigRoot,
        MIN_TOP_USE_HOURS_LIMIT,
        MINIMUM_LIMIT,
        NUM_HOUR_ONE_DAY,
        DEFAULT_TOP_USE_HOURS_LIMIT);
    int32_t minHourUseDays = GetIntValue(appHighFreqPeriodThresholdConfigRoot,
        MIN_HOUR_USE_DAYS,
        MINIMUM_LIMIT,
        NUM_DAY_ONE_WEEK,
        DEFAULT_MIN_HOUR_USE_DAYS);
    int32_t maxHighFreqHourNum = GetIntValue(appHighFreqPeriodThresholdConfigRoot,
        MAX_HIGH_FREQUENCY_HOUR_NUM,
        MINIMUM_LIMIT,
        NUM_HOUR_ONE_DAY,
        DEFAULT_MAX_HIGH_FREQUENCY_HOUR_NUM);

    appHighFreqPeriodThresholdConfig_ = {minTotalUseDays, minTopUseHoursLimit, minHourUseDays, maxHighFreqHourNum};
}

void BundleActiveConfigReader::LoadMaxDataSize(cJSON* root)
{
    cJSON *maxDataSizeItem = cJSON_GetObjectItem(root, MAX_DATA_SIZE);
    if (!IsValidNumber(maxDataSizeItem)) {
        BUNDLE_ACTIVE_LOGE("not have max data size key");
        return;
    }
    maxDataSize_ = static_cast<uint64_t>(maxDataSizeItem->valueint);
}
// LCOV_EXCL_STOP

bool BundleActiveConfigReader::GetJsonFromFile(const char *filePath, cJSON *&root)
{
    std::string realPath;
    if (!BundleActiveConfigReader::ConvertFullPath(filePath, realPath)) {
        BUNDLE_ACTIVE_LOGE("Get real path failed %{private}s", filePath);
        return false;
    }
    BUNDLE_ACTIVE_LOGD("Read from %{private}s", realPath.c_str());
    std::string data;
    if (!LoadStringFromFile(realPath.c_str(), data)) {
        BUNDLE_ACTIVE_LOGE("load string from %{private}s failed", realPath.c_str());
        return false;
    }
// LCOV_EXCL_START
    if (data.empty()) {
        return false;
    }
    root = cJSON_Parse(data.c_str());
    if (!root) {
        BUNDLE_ACTIVE_LOGE("parse %{private}s json error", realPath.c_str());
        return false;
    }
    return true;
// LCOV_EXCL_STOP
}

bool BundleActiveConfigReader::ConvertFullPath(const std::string& partialPath, std::string& fullPath)
{
// LCOV_EXCL_START
    if (partialPath.empty() || partialPath.length() >= PATH_MAX) {
        return false;
    }
// LCOV_EXCL_STOP
    char tmpPath[PATH_MAX] = {0};
    if (realpath(partialPath.c_str(), tmpPath) == nullptr) {
        return false;
    }
    fullPath = tmpPath;
    return true;
}

AppUsePeriodicallyConfig BundleActiveConfigReader::GetApplicationUsePeriodicallyConfig()
{
    return appUsePeriodicallyConfig_;
}

AppHighFrequencyPeriodThresholdConfig BundleActiveConfigReader::GetAppHighFrequencyPeriodThresholdConfig()
{
    return appHighFreqPeriodThresholdConfig_;
}
// LCOV_EXCL_START
uint64_t BundleActiveConfigReader::GetMaxDataSize()
{
    if (maxDataSize_ == 0) {
        return DEFAULT_MAX_DATA_SIZE;
    }
    return maxDataSize_;
}

int32_t BundleActiveConfigReader::GetIntValue(
    cJSON* root, const char* parameterName, int32_t minLimit, int32_t maxLimit, int32_t defaultValue)
{
    cJSON* item = cJSON_GetObjectItem(root, parameterName);
    if (!IsValidNumber(item)) {
        BUNDLE_ACTIVE_LOGE("Configuration parameter %{private}s error", parameterName);
        return defaultValue;
    }
    int32_t value = static_cast<int32_t>(item->valueint);
    if (value < minLimit || value > maxLimit) {
        BUNDLE_ACTIVE_LOGE("Configuration parameter %{private}s is invalid value", parameterName);
        return defaultValue;
    }
    return value;
}

bool BundleActiveConfigReader::IsValidObject(cJSON* item)
{
    return item != nullptr && cJSON_IsObject(item);
}

bool BundleActiveConfigReader::IsValidNumber(cJSON* item)
{
    return item != nullptr && cJSON_IsNumber(item);
}
// LCOV_EXCL_STOP
}  // namespace DeviceUsageStats
}  // namespace OHOS
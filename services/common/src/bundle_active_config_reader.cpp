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
const int32_t DEFAULT_MIN_USE_TIMES = 1;
const int32_t DEFAULT_MAX_USE_TIMES = 10;
const int32_t DEFAULT_MIN_USE_DAYS = 3;
const int32_t MAX_BUFFER = 2048;
const uint64_t DEFAULT_MAX_DATA_SIZE = 5 * 1024 * 1024;


void BundleActiveConfigReader::LoadConfig()
{
    appUsePeriodicallyConfig_ = { DEFAULT_MIN_USE_TIMES, DEFAULT_MAX_USE_TIMES, DEFAULT_MIN_USE_DAYS};
    auto cfgFiles = GetCfgFiles(CONFIG_PATH);
    if (!cfgFiles) {
        BUNDLE_ACTIVE_LOGE("GetCfgFiles failed");
        return;
    }
    for (const auto& filePath : cfgFiles->paths) {
        LoadApplicationUsePeriodically(filePath);
        LoadMaxDataSize(filePath);
    }
    BUNDLE_ACTIVE_LOGI("appUsePeriodicallyConfig minUseTimes:%{public}d, maxUseTimes:%{public}d,"
        "minUseDays:%{public}d maxDataSize:%{public}lu", appUsePeriodicallyConfig_.minUseTimes,
        appUsePeriodicallyConfig_.maxUseTimes, appUsePeriodicallyConfig_.minUseDays,
        static_cast<unsigned long>(maxDataSize_));
    FreeCfgFiles(cfgFiles);
};

void BundleActiveConfigReader::LoadApplicationUsePeriodically(const char *filePath)
{
    if (!filePath) {
        return;
    }
    cJSON *root = nullptr;
    if (!GetJsonFromFile(filePath, root) || !root) {
        BUNDLE_ACTIVE_LOGE("file is empty %{private}s", filePath);
        return;
    }
    cJSON *appUsePeriodicallyRoot = cJSON_GetObjectItem(root, APPLICATION_USE_PERIODICALLY_KEY);
    if (!appUsePeriodicallyRoot || !cJSON_IsObject(appUsePeriodicallyRoot)) {
        BUNDLE_ACTIVE_LOGE("application_use_periodically content is empty");
        cJSON_Delete(root);
        return;
    }
    cJSON *minUseTimesItem = cJSON_GetObjectItem(appUsePeriodicallyRoot, MIN_USE_TIMES);
    if (!minUseTimesItem || !cJSON_IsNumber(minUseTimesItem)) {
        BUNDLE_ACTIVE_LOGE("not have MinUseTimes key");
        cJSON_Delete(root);
        return;
    }
    int32_t minUseTimes = static_cast<int32_t>(minUseTimesItem->valueint);
    cJSON *maxUseTimesItem = cJSON_GetObjectItem(appUsePeriodicallyRoot, MAX_USE_TIMES);
    if (!maxUseTimesItem || !cJSON_IsNumber(maxUseTimesItem)) {
        BUNDLE_ACTIVE_LOGE("not have MaxUseTimes key");
        cJSON_Delete(root);
        return;
    }
    int32_t maxUseTimes = static_cast<int32_t>(maxUseTimesItem->valueint);
    cJSON *minUseDaysItem = cJSON_GetObjectItem(appUsePeriodicallyRoot, MIN_USE_DAYS);
    if (!minUseDaysItem || !cJSON_IsNumber(minUseDaysItem)) {
        BUNDLE_ACTIVE_LOGE("not have MinUseDays key");
        cJSON_Delete(root);
        return;
    }
    int32_t minUseDays = static_cast<int32_t>(minUseDaysItem->valueint);
    appUsePeriodicallyConfig_ = { minUseTimes, maxUseTimes, minUseDays};
    cJSON_Delete(root);
};

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
    if (data.empty()) {
        return false;
    }
    root = cJSON_Parse(data.c_str());
    if (!root) {
        BUNDLE_ACTIVE_LOGE("parse %{private}s json error", realPath.c_str());
        return false;
    }
    return true;
}

bool BundleActiveConfigReader::ConvertFullPath(const std::string& partialPath, std::string& fullPath)
{
    if (partialPath.empty() || partialPath.length() >= PATH_MAX) {
        return false;
    }
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
};

void BundleActiveConfigReader::LoadMaxDataSize(const char *filePath)
{
    if (!filePath) {
        return;
    }
    cJSON *root = nullptr;
    if (!GetJsonFromFile(filePath, root) || !root) {
        BUNDLE_ACTIVE_LOGE("file is empty %{private}s", filePath);
        return;
    }
    cJSON *maxDataSizeItem = cJSON_GetObjectItem(root, MAX_DATA_SIZE);
    if (!maxDataSizeItem || !cJSON_IsNumber(maxDataSizeItem)) {
        BUNDLE_ACTIVE_LOGE("not have max data size key");
        cJSON_Delete(root);
        return;
    }
    maxDataSize_ = static_cast<uint64_t>(maxDataSizeItem->valueint);
    cJSON_Delete(root);
};

uint64_t BundleActiveConfigReader::GetMaxDataSize()
{
    if (maxDataSize_ == 0) {
        return DEFAULT_MAX_DATA_SIZE;
    }
    return maxDataSize_;
};

}  // namespace DeviceUsageStats
}  // namespace OHOS
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
const char* CONFIG_PATH = "etc/device_usage_statistics/device_usage_statistics_config.json";
const std::string APPLICATION_USE_PERIODICALLY_KEY = "application_use_periodically";
const std::string MIN_USE_TIMES = "MinUseTimes";
const std::string MAX_USE_TIMES = "MaxUseTimes";
const std::string MIN_USE_DAYS = "MinUseDays";
const int32_t DEFAULT_MIN_USE_TIMES = 1;
const int32_t DEFAULT_MAX_USE_TIMES = 10;
const int32_t DEFAULT_MIN_USE_DAYS = 3;
const int32_t MAX_BUFFER = 2048;


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
    }
    BUNDLE_ACTIVE_LOGI("appUsePeriodicallyConfig minUseTimes:%{public}d, maxUseTimes:%{public}d,"
        "minUseDays:%{public}d", appUsePeriodicallyConfig_.minUseTimes,
        appUsePeriodicallyConfig_.maxUseTimes, appUsePeriodicallyConfig_.minUseDays);
    FreeCfgFiles(cfgFiles);
};

void BundleActiveConfigReader::LoadApplicationUsePeriodically(const char *filePath)
{
    if (!filePath) {
        return;
    }
    Json::Value root;
    if (!GetJsonFromFile(filePath, root) || root.empty()) {
        BUNDLE_ACTIVE_LOGE("file is empty %{private}s", filePath);
        return;
    }
    if (!root.isMember(APPLICATION_USE_PERIODICALLY_KEY)) {
        BUNDLE_ACTIVE_LOGE("not have application_use_periodically key");
        return;
    }
    Json::Value appUsePeriodicallyRoot = root[APPLICATION_USE_PERIODICALLY_KEY];
    if (appUsePeriodicallyRoot.empty() || !appUsePeriodicallyRoot.isObject()) {
        BUNDLE_ACTIVE_LOGE("application_use_periodically content is empty");
        return;
    }
    if (appUsePeriodicallyRoot[MIN_USE_TIMES].empty() || !appUsePeriodicallyRoot[MIN_USE_TIMES].isInt()) {
        BUNDLE_ACTIVE_LOGE("not have MinUseTimes key");
        return;
    }
    int32_t minUseTimes = appUsePeriodicallyRoot[MIN_USE_TIMES].asInt();
    if (appUsePeriodicallyRoot[MAX_USE_TIMES].empty() || !appUsePeriodicallyRoot[MAX_USE_TIMES].isInt()) {
        BUNDLE_ACTIVE_LOGE("not have MaxUseTimes key");
        return;
    }
    int32_t maxUseTimes = appUsePeriodicallyRoot[MAX_USE_TIMES].asInt();
    if (appUsePeriodicallyRoot[MIN_USE_DAYS].empty() || !appUsePeriodicallyRoot[MIN_USE_DAYS].isInt()) {
        BUNDLE_ACTIVE_LOGE("not have MinUseDays key");
        return;
    }
    int32_t minUseDays = appUsePeriodicallyRoot[MIN_USE_DAYS].asInt();
    appUsePeriodicallyConfig_ = { minUseTimes, maxUseTimes, minUseDays};
};

bool BundleActiveConfigReader::GetJsonFromFile(const char *filePath, Json::Value &root)
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
    JSONCPP_STRING errs;
    Json::CharReaderBuilder readerBuilder;
    const unique_ptr<Json::CharReader> jsonReader(readerBuilder.newCharReader());
    bool res = jsonReader->parse(data.c_str(), data.c_str() + data.length(), &root, &errs);
    if (!res || !errs.empty()) {
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

}  // namespace DeviceUsageStats
}  // namespace OHOS
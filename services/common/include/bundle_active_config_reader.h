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

#ifndef BUNDLE_ACTIVE_CONFIG_READER_H
#define BUNDLE_ACTIVE_CONFIG_READER_H
#include <string>
#include "cJSON.h"

namespace OHOS {
namespace DeviceUsageStats {
struct AppUsePeriodicallyConfig {
    int32_t minUseTimes;
    int32_t maxUseTimes;
    int32_t minUseDays;
};

struct AppHighFrequencyPeriodThresholdConfig {
    int32_t minTotalUseDays;
    int32_t minTopUseHoursLimit;
    int32_t minHourUseDays;
    int32_t maxHighFreqHourNum;
};

class BundleActiveConfigReader {
public:
    void LoadConfig();
    AppUsePeriodicallyConfig GetApplicationUsePeriodicallyConfig();
    AppHighFrequencyPeriodThresholdConfig GetAppHighFrequencyPeriodThresholdConfig();
    uint64_t GetMaxDataSize();
private:
    void LoadConfigFile(const char* filePath);
    void LoadApplicationUsePeriodically(cJSON* root);
    void LoadAppHighFreqPeriodThresholdConfig(cJSON* root);
    void LoadMaxDataSize(cJSON* root);
    bool GetJsonFromFile(const char* filePath, cJSON*& root);
    bool ConvertFullPath(const std::string& partialPath, std::string& fullPath);
    int32_t GetIntValue(
        cJSON* root, const char* parameterName, int32_t minLimit, int32_t maxLimit, int32_t defaultValue);
    bool IsValidObject(cJSON* item);
    bool IsValidNumber(cJSON* item);

    AppUsePeriodicallyConfig appUsePeriodicallyConfig_;
    AppHighFrequencyPeriodThresholdConfig appHighFreqPeriodThresholdConfig_;
    uint64_t maxDataSize_ = 0;
};

}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CONFIG_READER_H


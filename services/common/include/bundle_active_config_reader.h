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
class BundleActiveConfigReader {
public:
    void LoadConfig();
    AppUsePeriodicallyConfig GetApplicationUsePeriodicallyConfig();
private:
    void LoadApplicationUsePeriodically(const char* path);
    bool GetJsonFromFile(const char* filePath, cJSON *&root);
    bool ConvertFullPath(const std::string& partialPath, std::string& fullPath);
    AppUsePeriodicallyConfig appUsePeriodicallyConfig_;
};

}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CONFIG_READER_H


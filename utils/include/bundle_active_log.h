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

#ifndef BUNDLE_ACTIVE_LOG_H
#define BUNDLE_ACTIVE_LOG_H

#include <string>
#include "hilog/log.h"

namespace OHOS {
namespace DeviceUsageStats {
#ifndef LOG_TAG_DOMAIN_ID_BUNDLE_ACTIVE
#define LOG_TAG_DOMAIN_ID_BUNDLE_ACTIVE 0xD001710
#endif

#ifndef LOG_TAG_BUNDLE_ACTIVE
#define LOG_TAG_BUNDLE_ACTIVE "BUNDLE_ACTIVE"
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel BUNDLE_ACTIVE_LOG_LABEL = {
    LOG_CORE,
    LOG_TAG_DOMAIN_ID_BUNDLE_ACTIVE,
    LOG_TAG_BUNDLE_ACTIVE
};

enum class BundleActiveLogLevel : uint8_t { DEBUG = 0, INFO, WARN, ERROR, FATAL };

class BundleActiveLog {
public:
    BundleActiveLog() = delete;
    ~BundleActiveLog() = delete;

    static bool JudgeValidLevel(const BundleActiveLogLevel &level);

    static void SetLogLevel(const BundleActiveLogLevel &level)
    {
        logLevel_ = level;
    }

    static const BundleActiveLogLevel &GetLogLevel()
    {
        return logLevel_;
    }

    static std::string GetCurrFileName(const char *str);

private:
    static BundleActiveLogLevel logLevel_;
};

#define BUNDLE_ACTIVE_PRINT_LOG(LEVEL, Level, fmt, ...)                                              \
    if (BundleActiveLog::JudgeValidLevel(BundleActiveLogLevel::LEVEL))                 \
    OHOS::HiviewDFX::HiLog::Level(BUNDLE_ACTIVE_LOG_LABEL,                             \
        "[%{public}s(%{public}s):%{public}d] " fmt,                                    \
        BundleActiveLog::GetCurrFileName(__FILE__).c_str(),                            \
        __FUNCTION__,                                                                  \
        __LINE__,                                                                      \
        ##__VA_ARGS__)

#define BUNDLE_ACTIVE_LOGD(fmt, ...) BUNDLE_ACTIVE_PRINT_LOG(DEBUG, Debug, fmt, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGI(fmt, ...) BUNDLE_ACTIVE_PRINT_LOG(INFO, Info, fmt, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGW(fmt, ...) BUNDLE_ACTIVE_PRINT_LOG(WARN, Warn, fmt, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGE(fmt, ...) BUNDLE_ACTIVE_PRINT_LOG(ERROR, Error, fmt, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGF(fmt, ...) BUNDLE_ACTIVE_PRINT_LOG(FATAL, Fatal, fmt, ##__VA_ARGS__)
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_LOG_H


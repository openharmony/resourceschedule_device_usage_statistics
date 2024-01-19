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
#undef LOG_DOMAIN
#define LOG_DOMAIN 0xD001710

#undef LOG_TAG
#define LOG_TAG "BUNDLE_ACTIVE"

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

#define BUNDLE_ACTIVE_LOGD(fmt, ...) HILOG_DEBUG(LOG_CORE, "[%{public}s(%{public}s):%{public}d]" fmt, \
        BundleActiveLog::GetCurrFileName(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGI(fmt, ...) HILOG_INFO(LOG_CORE, "[%{public}s(%{public}s):%{public}d]" fmt, \
        BundleActiveLog::GetCurrFileName(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGW(fmt, ...) HILOG_WARN(LOG_CORE, "[%{public}s(%{public}s):%{public}d]" fmt, \
        BundleActiveLog::GetCurrFileName(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGE(fmt, ...) HILOG_ERROR(LOG_CORE, "[%{public}s(%{public}s):%{public}d]" fmt, \
        BundleActiveLog::GetCurrFileName(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define BUNDLE_ACTIVE_LOGF(fmt, ...) HILOG_FATAL(LOG_CORE, "[%{public}s(%{public}s):%{public}d]" fmt, \
        BundleActiveLog::GetCurrFileName(__FILE__).c_str(), __FUNCTION__, __LINE__, ##__VA_ARGS__)
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_LOG_H


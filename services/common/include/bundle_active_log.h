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

#include "hilog/log.h"

#define LOG_TAG_BUNDLE_ACTIVE "BUNDLE_ACTIVE"
#define LOG_TAG_DOMAIN_ID_BUNDLE_ACTIVE 0xD001700

static constexpr OHOS::HiviewDFX::HiLogLabel BUNDLE_ACTIVE_LOG_LABEL = {
    LOG_CORE,
    LOG_TAG_DOMAIN_ID_BUNDLE_ACTIVE,
    LOG_TAG_BUNDLE_ACTIVE
};

#define BUNDLE_ACTIVE_LOGF(...) (void)OHOS::HiviewDFX::HiLog::Fatal(BUNDLE_ACTIVE_LOG_LABEL, __VA_ARGS__)
#define BUNDLE_ACTIVE_LOGE(...) (void)OHOS::HiviewDFX::HiLog::Error(BUNDLE_ACTIVE_LOG_LABEL, __VA_ARGS__)
#define BUNDLE_ACTIVE_LOGW(...) (void)OHOS::HiviewDFX::HiLog::Warn(BUNDLE_ACTIVE_LOG_LABEL, __VA_ARGS__)
#define BUNDLE_ACTIVE_LOGI(...) (void)OHOS::HiviewDFX::HiLog::Info(BUNDLE_ACTIVE_LOG_LABEL, __VA_ARGS__)
#define BUNDLE_ACTIVE_LOGD(...) (void)OHOS::HiviewDFX::HiLog::Debug(BUNDLE_ACTIVE_LOG_LABEL, __VA_ARGS__)

#endif
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

#include "bundle_active_open_callback.h"
#include "bundle_active_log.h"
#include "bundle_active_constant.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveOpenCallback::BundleActiveOpenCallback() : NativeRdb::RdbOpenCallback()
{
};

BundleActiveOpenCallback::~BundleActiveOpenCallback()
{
};

int32_t BundleActiveOpenCallback::OnCreate(NativeRdb::RdbStore &rdbStore)
{
    BUNDLE_ACTIVE_LOGI("Create success.");
    return NativeRdb::E_OK;
};

int32_t BundleActiveOpenCallback::OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion)
{
    (void)rdbStore;
    (void)oldVersion;
    (void)newVersion;
    BUNDLE_ACTIVE_LOGI("Upgrade success.");
    return NativeRdb::E_OK;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS


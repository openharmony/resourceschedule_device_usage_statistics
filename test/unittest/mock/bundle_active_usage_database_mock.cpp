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

#include "bundle_active_usage_database.h"

namespace OHOS {
namespace DeviceUsageStats {
using namespace std;
unique_ptr<NativeRdb::ResultSet> BundleActiveUsageDatabase::QueryStatsInfoByStep(uint32_t databaseType,
    const string &sql, const vector<string> &selectionArgs)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveUsageDatabase::QueryStatsInfoByStep observer has been delete");
    return nullptr;
}

shared_ptr<NativeRdb::RdbStore> BundleActiveUsageDatabase::GetBundleActiveRdbStore(uint32_t databaseType)
{
    BUNDLE_ACTIVE_LOGI("BundleActiveUsageDatabase::GetBundleActiveRdbStore observer has been delete");
    return nullptr;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS
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

#include "bundle_active_binary_search.h"

namespace OHOS {
namespace DeviceUsageStats {
BundleActiveBinarySearch::BundleActiveBinarySearch()
{
}

BundleActiveBinarySearch::~BundleActiveBinarySearch()
{
}

int32_t BundleActiveBinarySearch::BinarySearch(const std::vector<int64_t> &tableNameArray, int64_t targetValue)
{
    int32_t low = 0;
    int32_t high = static_cast<int32_t>(tableNameArray.size() - 1);
    int32_t divider = 2;
    while (low <= high) {
        int32_t mid = (low + high) / divider;
        int64_t midValue = tableNameArray.at(mid);
        if (midValue < targetValue) {
            low = mid + 1;
        } else if (midValue > targetValue) {
            high = mid - 1;
        } else {
            return mid;
        }
    }
    return -1;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


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

#ifndef BUNDLE_ACTIVE_USAGE_HISTORY_H
#define BUNDLE_ACTIVE_USAGE_HISTORY_H

#include "ibundle_active_service.h"
#include "bundle_active_group_common.h"
namespace OHOS {
namespace DeviceUsageStats {
class BundleActivePackageHistory {
public:
    int64_t lastBootFromUsedTimeStamp_;
    int64_t lastScreenUsedTimeStamp_;
    int64_t lastGroupCalculatedTimeStamp_;
    int32_t lastCalculatedGroup_;
    int32_t currentGroup_;
    uint32_t reasonInGroup_;
    int64_t bundleAliveTimeoutTimeStamp_;
    int64_t bundleDailyTimeoutTimeStamp_;
    bool isChanged_;
    BundleActivePackageHistory();
    ~BundleActivePackageHistory() {}
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_USAGE_HISTORY_H


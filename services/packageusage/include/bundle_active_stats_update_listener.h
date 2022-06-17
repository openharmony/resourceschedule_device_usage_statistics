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

#ifndef BUNDLE_ACTIVE_STATS_UPDATE_LISTENER_H
#define BUNDLE_ACTIVE_STATS_UPDATE_LISTENER_H

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveStatsUpdateListener {
    virtual void OnStatsChanged(const int32_t userId) = 0;
    virtual void OnStatsReload() = 0;
    virtual void OnSystemUpdate(int32_t userId) = 0;
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_STATS_UPDATE_LISTENER_H


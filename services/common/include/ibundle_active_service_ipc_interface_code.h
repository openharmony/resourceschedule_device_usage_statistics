/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_ACTIVE_SERVICE_IPC_INTERFACE_CODE_H
#define BUNDLE_ACTIVE_SERVICE_IPC_INTERFACE_CODE_H

 /* SAID: 1907 */
namespace OHOS {
namespace DeviceUsageStats {
    enum class IBundleActiveServiceInterfaceCode {
        REPORT_EVENT = 1,
        IS_BUNDLE_IDLE = 2,
        QUERY_BUNDLE_STATS_INFO_BY_INTERVAL = 3,
        QUERY_BUNDLE_EVENTS = 4,
        QUERY_BUNDLE_STATS_INFOS = 5,
        QUERY_CURRENT_BUNDLE_EVENTS = 6,
        QUERY_APP_GROUP = 7,
        SET_APP_GROUP = 8,
        QUERY_MODULE_USAGE_RECORDS = 9,
        REGISTER_APP_GROUP_CALLBACK = 10,
        UNREGISTER_APP_GROUP_CALLBACK = 11,
        QUERY_DEVICE_EVENT_STATES = 12,
        QUERY_NOTIFICATION_NUMBER = 13
    };
}  // namespace BackgroundTaskMgr
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_SERVICE_IPC_INTERFACE_CODE_H
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

#ifndef IAPP_GROUP_CALLBACK_H
#define IAPP_GROUP_CALLBACK_H

#include <ipc_types.h>
#include <iremote_broker.h>

#include "bundle_active_log.h"
#include "app_group_callback_info.h"

namespace OHOS {
namespace DeviceUsageStats {
class IAppGroupCallback : public IRemoteBroker {
public:

    /**
     * @brief Called back when a app priority group change.
     *
     * @param appGroupCallbackInfo app group info.
     */
    virtual void OnAppGroupChanged(
        const AppGroupCallbackInfo &appGroupCallbackInfo) = 0;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"Resourceschedule.IAppGroupCallback");

protected:
    enum class message {
        ON_BUNDLE_GROUP_CHANGED = 1
    };
};
}  // namespace BackgroundTaskMgr
}  // namespace OHOS
#endif  // IAPP_GROUP_CALLBACK_H
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

#include "bundle_active_log.h"
#include "app_group_callback_info.h"

namespace OHOS {
namespace DeviceUsageStats {
AppGroupCallbackInfo::AppGroupCallbackInfo(int32_t userId, int32_t oldGroup, int32_t newGroup,
    uint32_t changeReason, std::string bundleName)
{
    userId_ = userId;
    oldGroup_ = oldGroup;
    newGroup_ = newGroup;
    changeReason_ = changeReason;
    bundleName_ = bundleName;
}

int32_t AppGroupCallbackInfo::GetUserId() const
{
    return userId_;
}

int32_t AppGroupCallbackInfo::GetOldGroup() const
{
    return oldGroup_;
}

int32_t AppGroupCallbackInfo::GetNewGroup() const
{
    return newGroup_;
}

uint32_t AppGroupCallbackInfo::GetChangeReason() const
{
    return changeReason_;
}

std::string AppGroupCallbackInfo::GetBundleName() const
{
    return bundleName_;
}

bool AppGroupCallbackInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(userId_)) {
        BUNDLE_ACTIVE_LOGE("Failed to write userId_");
        return false;
    }

    if (!parcel.WriteInt32(oldGroup_)) {
        BUNDLE_ACTIVE_LOGE("Failed to write creator oldGroup_");
        return false;
    }

    if (!parcel.WriteInt32(newGroup_)) {
        BUNDLE_ACTIVE_LOGE("Failed to write creator newGroup_");
        return false;
    }

    if (!parcel.WriteUint32(changeReason_)) {
        BUNDLE_ACTIVE_LOGE("Failed to write creator changeReason_");
        return false;
    }

    if (!parcel.WriteString(bundleName_)) {
        BUNDLE_ACTIVE_LOGE("Failed to write bundleName_");
        return false;
    }
    return true;
}

AppGroupCallbackInfo* AppGroupCallbackInfo::Unmarshalling(Parcel &parcel)
{
    AppGroupCallbackInfo* result = new (std::nothrow) AppGroupCallbackInfo();
    result->userId_ = parcel.ReadInt32();
    result->oldGroup_ = parcel.ReadInt32();
    result->newGroup_ = parcel.ReadInt32();
    result->changeReason_ = parcel.ReadUint32();
    result->bundleName_ = parcel.ReadString();
    return result;
}
}
}


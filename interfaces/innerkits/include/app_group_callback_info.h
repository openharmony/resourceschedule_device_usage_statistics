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

#ifndef APP_GROUP_CALLBACK_INFO_H
#define APP_GROUP_CALLBACK_INFO_H

#include <memory>

#include "parcel.h"

namespace OHOS {
namespace DeviceUsageStats {
class AppGroupCallbackInfo : public Parcelable {
public:
    AppGroupCallbackInfo() {};
    AppGroupCallbackInfo(int32_t userId, int32_t oldGroup, int32_t newGroup, uint32_t changeReason,
        std::string bundleName);

    /**
     * @brief Get the user id.
     *
     * @return The id of user.
     */
    int32_t GetUserId() const;

    /**
     * @brief Get the old group.
     *
     * @return old group of app.
     */
    int32_t GetOldGroup() const;

    /**
     * @brief Get the new group.
     *
     * @return the new group of app.
     */
    int32_t GetNewGroup() const;

    /**
     * @brief Get the reason of change group.
     *
     * @return the reason of change group.
     */
    uint32_t GetChangeReason() const;

    /**
     * @brief Get the name of bundle.
     *
     * @return The name of bundle.
     */
    std::string GetBundleName() const;

    /**
     * @brief Marshals a purpose into a parcel.
     *
     * @param parcel Indicates the parcel object for marshalling.
     * @return True if success, else false.
     */
    bool Marshalling(Parcel &parcel) const;
    static AppGroupCallbackInfo* Unmarshalling(Parcel &parcel);

private:
    bool ReadFromParcel(Parcel &parcel);

private:
    int32_t oldGroup_ {0};
    int32_t newGroup_ {0};
    int32_t userId_ {-1};
    uint32_t changeReason_ {0};
    std::string bundleName_ {""};
};
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // APP_GROUP_CALLBACK_INFO_H
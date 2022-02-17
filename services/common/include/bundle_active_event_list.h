/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef BUNDLE_ACTIVE_EVENT_LIST_H
#define BUNDLE_ACTIVE_EVENT_LIST_H

#include "ibundle_active_service.h"
#include "bundle_active_event.h"

namespace OHOS {
namespace BundleActive {
class BundleActiveEventList {
public:
    BundleActiveEventList();
    int Size();
    void Clear();
    void Insert(BundleActiveEvent event);
    int FindBestIndex(int64_t timeStamp);
    void Merge(const BundleActiveEventList& right);
private:
    std::vector<BundleActiveEvent> events_;
};
}
}
#endif
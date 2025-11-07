/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "bundleactiveclient_fuzzer.h"
#include "bundle_active_client.h"
#include "bundle_active_event.h"

#include <string>
#include <vector>
#include <algorithm>

using namespace OHOS::DeviceUsageStats;

namespace OHOS {

namespace {
constexpr int32_t DEFAULT_USER_ID = 100;
constexpr int32_t DEFAULT_MAX_NUM = 10;
constexpr int64_t DEFAULT_BEGIN_TIME = 0;
constexpr int64_t DEFAULT_END_TIME = 1000;
constexpr size_t MAX_BUNDLE_NAME_LEN = 128;

enum FuzzCase {
    REPORT_EVENT = 0,
    IS_BUNDLE_IDLE,
    QUERY_STATS_INTERVAL,
    QUERY_EVENTS,
    SET_APP_GROUP,
    QUERY_ALL_STATS,
    QUERY_HIGH_FREQ,
    QUERY_APP_GROUP,
    QUERY_MODULE_USAGE,
    QUERY_DEVICE_STATS,
    FUZZ_CASE_COUNT
};

template<typename T>
bool SafeRead(const uint8_t*& data, size_t& size, T& out)
{
    static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
    if (size < sizeof(T)) {
        return false;
    }
    out = std::bit_cast<T>(*reinterpret_cast<const T*>(data));
    data += sizeof(T);
    size -= sizeof(T);
    return true;
}

std::string ExtractBundleName(const uint8_t*& data, size_t& size)
{
    size_t len = std::min(size, MAX_BUNDLE_NAME_LEN);
    std::string name(reinterpret_cast<const char*>(data), len);
    data += len;
    size -= len;
    return name;
}
} // namespace

bool DoSomethingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return false;
    }

    try {
        int32_t choice = 0;
        if (!SafeRead(data, size, choice)) {
            return false;
        }

        BundleActiveClient& client = BundleActiveClient::GetInstance();

        std::string bundleName = ExtractBundleName(data, size);
        int32_t intervalType = 0;
        int32_t newGroup = 0;
        int32_t maxNum = DEFAULT_MAX_NUM;
        int64_t beginTime = DEFAULT_BEGIN_TIME;
        int64_t endTime = DEFAULT_END_TIME;

        SafeRead(data, size, intervalType);
        SafeRead(data, size, newGroup);
        SafeRead(data, size, maxNum);
        SafeRead(data, size, beginTime);
        SafeRead(data, size, endTime);

        if (beginTime > endTime) {
            std::swap(beginTime, endTime);
        }
        if (maxNum <= 0) {
            maxNum = 1;
        }

        switch (choice % FUZZ_CASE_COUNT) {
            case REPORT_EVENT: {
                BundleActiveEvent event;
                if (!SafeRead(data, size, event.eventId_)) {
                    event.eventId_ = BundleActiveEvent::ABILITY_FOREGROUND;
                }
                event.bundleName_ = bundleName;
                event.timeStamp_ = endTime;
                client.ReportEvent(event, DEFAULT_USER_ID);
                break;
            }
            case IS_BUNDLE_IDLE: {
                bool isIdle = false;
                client.IsBundleIdle(isIdle, bundleName, DEFAULT_USER_ID);
                break;
            }
            case QUERY_STATS_INTERVAL: {
                std::vector<BundleActivePackageStats> stats;
                client.QueryBundleStatsInfoByInterval(stats, intervalType, beginTime, endTime, DEFAULT_USER_ID);
                break;
            }
            case QUERY_EVENTS: {
                std::vector<BundleActiveEvent> events;
                client.QueryBundleEvents(events, beginTime, endTime, DEFAULT_USER_ID);
                break;
            }
            case SET_APP_GROUP: {
                client.SetAppGroup(bundleName, newGroup, DEFAULT_USER_ID);
                break;
            }
            case QUERY_ALL_STATS: {
                std::vector<BundleActivePackageStats> stats;
                client.QueryBundleStatsInfos(stats, intervalType, beginTime, endTime);
                break;
            }
            case QUERY_HIGH_FREQ: {
                std::vector<BundleActivePackageStats> stats;
                client.QueryHighFrequencyUsageBundleInfos(stats, DEFAULT_USER_ID, maxNum);
                break;
            }
            case QUERY_APP_GROUP: {
                int32_t appGroup = 0;
                client.QueryAppGroup(appGroup, bundleName, DEFAULT_USER_ID);
                break;
            }
            case QUERY_MODULE_USAGE: {
                std::vector<BundleActiveModuleRecord> results;
                client.QueryModuleUsageRecords(maxNum, results, DEFAULT_USER_ID);
                break;
            }
            case QUERY_DEVICE_STATS: {
                std::vector<BundleActiveEventStats> stats;
                client.QueryDeviceEventStats(beginTime, endTime, stats, DEFAULT_USER_ID);
                break;
            }
            default:
                break;
        }
    } catch (...) {
        return false;
    }

    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    return OHOS::DoSomethingWithMyAPI(data, size) ? 0 : 1;
}

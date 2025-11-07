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

struct FuzzParams {
    std::string bundleName;
    int32_t intervalType = 0;
    int32_t newGroup = 0;
    int32_t maxNum = DEFAULT_MAX_NUM;
    int64_t beginTime = DEFAULT_BEGIN_TIME;
    int64_t endTime = DEFAULT_END_TIME;
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

void HandleReportEvent(const FuzzParams& params, const uint8_t*& data, size_t& size)
{
    BundleActiveEvent event;
    if (!SafeRead(data, size, event.eventId_)) {
        event.eventId_ = BundleActiveEvent::ABILITY_FOREGROUND;
    }
    event.bundleName_ = params.bundleName;
    event.timeStamp_ = params.endTime;
    BundleActiveClient::GetInstance().ReportEvent(event, DEFAULT_USER_ID);
}

void HandleIsBundleIdle(const FuzzParams& params)
{
    bool isIdle = false;
    BundleActiveClient::GetInstance().IsBundleIdle(isIdle, params.bundleName, DEFAULT_USER_ID);
}

void HandleQueryStatsInterval(const FuzzParams& params)
{
    std::vector<BundleActivePackageStats> stats;
    BundleActiveClient::GetInstance()
        .QueryBundleStatsInfoByInterval(stats, params.intervalType, params.beginTime, params.endTime, DEFAULT_USER_ID);
}

void HandleQueryEvents(const FuzzParams& params)
{
    std::vector<BundleActiveEvent> events;
    BundleActiveClient::GetInstance().QueryBundleEvents(events, params.beginTime, params.endTime, DEFAULT_USER_ID);
}

void HandleSetAppGroup(const FuzzParams& params)
{
    BundleActiveClient::GetInstance().SetAppGroup(params.bundleName, params.newGroup, DEFAULT_USER_ID);
}

void HandleQueryAllStats(const FuzzParams& params)
{
    std::vector<BundleActivePackageStats> stats;
    BundleActiveClient::GetInstance().QueryBundleStatsInfos(stats, params.intervalType, params.beginTime, params.endTime);
}

void HandleQueryHighFreq(const FuzzParams& params)
{
    std::vector<BundleActivePackageStats> stats;
    BundleActiveClient::GetInstance().QueryHighFrequencyUsageBundleInfos(stats, DEFAULT_USER_ID, params.maxNum);
}

void HandleQueryAppGroup(const FuzzParams& params)
{
    int32_t appGroup = 0;
    BundleActiveClient::GetInstance().QueryAppGroup(appGroup, params.bundleName, DEFAULT_USER_ID);
}

void HandleQueryModuleUsage(const FuzzParams& params)
{
    std::vector<BundleActiveModuleRecord> results;
    BundleActiveClient::GetInstance().QueryModuleUsageRecords(params.maxNum, results, DEFAULT_USER_ID);
}

void HandleQueryDeviceStats(const FuzzParams& params)
{
    std::vector<BundleActiveEventStats> stats;
    BundleActiveClient::GetInstance().QueryDeviceEventStats(params.beginTime, params.endTime, stats, DEFAULT_USER_ID);
}

/*
 * Split dispatch into two parts to keep single-function size under 50 lines.
 * Part1 handles cases: REPORT_EVENT .. SET_APP_GROUP (0..4)
 */
void DispatchFuzzCasePart1(int32_t choice, const FuzzParams& params, const uint8_t*& data, size_t& size)
{
    switch (choice) {
        case REPORT_EVENT:
            HandleReportEvent(params, data, size);
            break;
        case IS_BUNDLE_IDLE:
            HandleIsBundleIdle(params);
            break;
        case QUERY_STATS_INTERVAL:
            HandleQueryStatsInterval(params);
            break;
        case QUERY_EVENTS:
            HandleQueryEvents(params);
            break;
        case SET_APP_GROUP:
            HandleSetAppGroup(params);
            break;
        default:
            break;
    }
}

/*
 * Part2 handles remaining cases: QUERY_ALL_STATS .. QUERY_DEVICE_STATS (5..9)
 */
void DispatchFuzzCasePart2(int32_t choice, const FuzzParams& params, const uint8_t*& data, size_t& size)
{
    switch (choice) {
        case QUERY_ALL_STATS:
            HandleQueryAllStats(params);
            break;
        case QUERY_HIGH_FREQ:
            HandleQueryHighFreq(params);
            break;
        case QUERY_APP_GROUP:
            HandleQueryAppGroup(params);
            break;
        case QUERY_MODULE_USAGE:
            HandleQueryModuleUsage(params);
            break;
        case QUERY_DEVICE_STATS:
            HandleQueryDeviceStats(params);
            break;
        default:
            break;
    }
}

/*
 * Minimal wrapper: decides which part to call. Keeps wrapper very small.
 */
void DispatchFuzzCase(int32_t choice, const FuzzParams& params, const uint8_t*& data, size_t& size)
{
    int32_t idx = choice % FUZZ_CASE_COUNT;
    if (idx >= 0 && idx <= SET_APP_GROUP) {
        DispatchFuzzCasePart1(idx, params, data, size);
    } else {
        DispatchFuzzCasePart2(idx, params, data, size);
    }
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

        FuzzParams params;
        params.bundleName = ExtractBundleName(data, size);
        SafeRead(data, size, params.intervalType);
        SafeRead(data, size, params.newGroup);
        SafeRead(data, size, params.maxNum);
        SafeRead(data, size, params.beginTime);
        SafeRead(data, size, params.endTime);

        if (params.beginTime > params.endTime) {
            std::swap(params.beginTime, params.endTime);
        }
        if (params.maxNum <= 0) {
            params.maxNum = 1;
        }

        DispatchFuzzCase(choice, params, data, size);
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

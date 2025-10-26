#include "bundleactiveclient_fuzzer.h"
#include "bundle_active_client.h"
#include "bundle_active_event.h"
#include <cstring>
#include <stdexcept>

using namespace OHOS::DeviceUsageStats;

namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr || size < sizeof(int32_t)) {
        return false;
    }

    try {
        int32_t choice = 0;
        if (size < sizeof(int32_t)) return false;
        std::memcpy(&choice, data, sizeof(int32_t));
        data += sizeof(int32_t);
        size -= sizeof(int32_t);

        BundleActiveClient& client = BundleActiveClient::GetInstance();

        std::string bundleName;
        int32_t userId = 100;
        int32_t intervalType = 0;
        int64_t beginTime = 0;
        int64_t endTime = 1000;
        int32_t newGroup = 0;
        int32_t maxNum = 10;

        size_t nameLen = std::min(size, static_cast<size_t>(128));
        bundleName = std::string(reinterpret_cast<const char*>(data), nameLen);
        data += nameLen;
        size -= nameLen;
        if (size >= sizeof(int32_t)) {
            std::memcpy(&intervalType, data, sizeof(int32_t));
            data += sizeof(int32_t);
            size -= sizeof(int32_t);
        }
        if (size >= sizeof(int32_t)) {
            std::memcpy(&newGroup, data, sizeof(int32_t));
            data += sizeof(int32_t);
            size -= sizeof(int32_t);
        }
        if (size >= sizeof(int32_t)) {
            std::memcpy(&maxNum, data, sizeof(int32_t));
            data += sizeof(int32_t);
            size -= sizeof(int32_t);
        }
        if (size >= sizeof(int64_t)) {
            std::memcpy(&beginTime, data, sizeof(int64_t));
            data += sizeof(int64_t);
            size -= sizeof(int64_t);
        }
        if (size >= sizeof(int64_t)) {
            std::memcpy(&endTime, data, sizeof(int64_t));
            data += sizeof(int64_t);
            size -= sizeof(int64_t);
        }
        if (beginTime > endTime) {
            std::swap(beginTime, endTime);
        }
        if (maxNum <= 0) {
            maxNum = 1;
        }

        switch (choice % 10) {
            case 0: {
                BundleActiveEvent event;
                if (size >= sizeof(int32_t)) {
                    std::memcpy(&event.eventId_, data, sizeof(int32_t));
                } else {
                    event.eventId_ = BundleActiveEvent::ABILITY_FOREGROUND;
                }
                event.bundleName_ = bundleName;
                event.timeStamp_ = endTime;
                client.ReportEvent(event, userId);
                break;
            }
            case 1: {
                bool isBundleIdle = false;
                client.IsBundleIdle(isBundleIdle, bundleName, userId);
                break;
            }
            case 2: {
                std::vector<BundleActivePackageStats> stats;
                client.QueryBundleStatsInfoByInterval(stats, intervalType, beginTime, endTime, userId);
                break;
            }
            case 3: {
                std::vector<BundleActiveEvent> events;
                client.QueryBundleEvents(events, beginTime, endTime, userId);
                break;
            }
            case 4: {
                client.SetAppGroup(bundleName, newGroup, userId);
                break;
            }
            case 5: {
                std::vector<BundleActivePackageStats> stats;
                client.QueryBundleStatsInfos(stats, intervalType, beginTime, endTime);
                break;
            }
            case 6: {
                std::vector<BundleActivePackageStats> stats;
                client.QueryHighFrequencyUsageBundleInfos(stats, userId, maxNum);
                break;
            }
            case 7: {
                int32_t appGroup = 0;
                client.QueryAppGroup(appGroup, bundleName, userId);
                break;
            }
            case 8: {
                std::vector<BundleActiveModuleRecord> results;
                client.QueryModuleUsageRecords(maxNum, results, userId);
                break;
            }
            case 9: {
                std::vector<BundleActiveEventStats> eventStats;
                client.QueryDeviceEventStats(beginTime, endTime, eventStats, userId);
                break;
            }
            default:
                break;
        }
    } catch (const std::exception& e) {
        // 捕获异常防止崩溃
        return false;
    }

    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    return OHOS::DoSomethingInterestingWithMyAPI(data, size) ? 0 : 1;
}

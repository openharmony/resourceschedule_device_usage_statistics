/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "bundleactivepackagestats_fuzzer.h"
#include "bundle_active_package_stats.h"
#include <fuzzer/FuzzedDataProvider.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <array>

using namespace OHOS::DeviceUsageStats;

namespace {
// 定义可能的 eventId 值列表，用于覆盖所有可能的状态转换
constexpr std::array<int32_t, 11> EVENT_IDS = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// 生成随机字符串
std::string ConsumeRandomString(FuzzedDataProvider& fdp, size_t maxLen = 100)
{
    auto len = fdp.ConsumeIntegralInRange<size_t>(0, maxLen);
    return fdp.ConsumeRandomLengthString(len);
}

// 生成随机时间戳
int64_t ConsumeTimestamp(FuzzedDataProvider& fdp)
{
    return fdp.ConsumeIntegral<int64_t>();
}

// 生成随机 UID
int32_t ConsumeUid(FuzzedDataProvider& fdp)
{
    return fdp.ConsumeIntegral<int32_t>();
}

// 对 BundleActivePackageStats 进行多轮 Update 调用
void PerformUpdates(BundleActivePackageStats& stats, FuzzedDataProvider& fdp, size_t numUpdates)
{
    for (size_t i = 0; i < numUpdates; ++i) {
        std::string longTimeTaskName = ConsumeRandomString(fdp, 50);
        int64_t timeStamp = ConsumeTimestamp(fdp);
        int32_t eventId = fdp.PickValueInArray(EVENT_IDS);
        std::string abilityId = ConsumeRandomString(fdp, 50);
        int32_t uid = ConsumeUid(fdp);

        stats.Update(longTimeTaskName, timeStamp, eventId, abilityId, uid);

        // 随机调用其他方法
        if (fdp.ConsumeBool()) {
            stats.IncrementTimeUsed(ConsumeTimestamp(fdp));
        }
        if (fdp.ConsumeBool()) {
            stats.IncrementServiceTimeUsed(ConsumeTimestamp(fdp));
        }
        if (fdp.ConsumeBool()) {
            stats.IncrementBundleLaunchedCount();
        }
    }
}

// 测试序列化与反序列化，并验证 ToString
void TestMarshalling(const BundleActivePackageStats& stats)
{
    OHOS::Parcel parcel;
    if (!stats.Marshalling(parcel)) {
        return;
    }

    parcel.RewindRead(0);
    BundleActivePackageStats* unmarshalled = BundleActivePackageStats::Unmarshalling(parcel);
    if (unmarshalled != nullptr) {
        (void)unmarshalled->ToString(); // 触发 ToString 调用
        delete unmarshalled;
    }
}

// 对 stats 进行多轮增量操作
void PerformIncrements(BundleActivePackageStats& stats, FuzzedDataProvider& fdp, size_t numIncrements)
{
    for (size_t i = 0; i < numIncrements; ++i) {
        if (fdp.ConsumeBool()) {
            stats.IncrementTimeUsed(ConsumeTimestamp(fdp));
        }
        if (fdp.ConsumeBool()) {
            stats.IncrementServiceTimeUsed(ConsumeTimestamp(fdp));
        }
        if (fdp.ConsumeBool()) {
            stats.IncrementBundleLaunchedCount();
        }
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);

    // 1. 测试默认构造函数
    BundleActivePackageStats stats1;

    // 2. 测试拷贝构造函数
    BundleActivePackageStats stats2(stats1);

    // 3. 对 stats1 进行多轮 Update 调用
    auto numUpdates1 = fdp.ConsumeIntegralInRange<size_t>(1, 50);
    PerformUpdates(stats1, fdp, numUpdates1);

    // 4. 测试 Marshalling/Unmarshalling
    TestMarshalling(stats1);

    // 5. 对 stats2 进行一定数量的操作
    auto numUpdates2 = fdp.ConsumeIntegralInRange<size_t>(0, 20);
    PerformUpdates(stats2, fdp, numUpdates2);

    // 6. 测试 ToString 方法
    (void)stats1.ToString();
    (void)stats2.ToString();

    // 7. 测试赋值（通过拷贝构造模拟）
    BundleActivePackageStats stats3(stats1);

    // 8. 边界测试：空字符串参数
    if (fdp.remaining_bytes() > 0) {
        stats1.Update("", 0, 0, "", 0);
    }

    // 9. 多次调用增量方法
    auto numIncrements = fdp.ConsumeIntegralInRange<size_t>(0, 10);
    PerformIncrements(stats1, fdp, numIncrements);

    // 10. 最后再测试一次序列化/反序列化
    TestMarshalling(stats1);

    return 0;
}
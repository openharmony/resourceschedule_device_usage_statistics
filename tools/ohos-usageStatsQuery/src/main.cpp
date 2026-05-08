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

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>
#include <cstring>
#include <cerrno>
#include <climits>

#include "cJSON.h"
#include "bundle_active_client.h"
#include "bundle_active_package_stats.h"
#include "bundle_active_event.h"
#include "bundle_active_event_stats.h"
#include "bundle_active_module_record.h"
#include "bundle_active_high_frequency_period.h"
#include "bundle_state_inner_errors.h"

using namespace OHOS::DeviceUsageStats;
using OHOS::ErrCode;
using OHOS::ERR_OK;

#define CLI_LOG(fmt, ...) fprintf(stderr, "[LOG] " fmt "\n", ##__VA_ARGS__)
#define CLI_ERROR(fmt, ...) fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)
#define MAX_RETURN_COUNT (1000)
#define MIN_RETURN_COUNT (1)
#define DEFAULT_USER_ID (-1)
#define DEFAULT_INTERVAL_TYPE (-1)
#define MAX_INTERVAL_TYPE (3)
#define DEFAULT_DAY_RANGE (7)
#define DEFAULT_HIGH_FREQ_COUNT (20)
#define MIN_ARG_COUNT (2)
#define HELP_ARGC_MINIMAL (2)
#define HELP_ARGC_WITH_CMD (3)
#define HELP_CMD_ARG_START_INDEX (2)
#define ARG_START_INDEX (1)
#define DECIMAL_BASE (10)

typedef std::function<int(int, char**)> CommandHandler;

struct Command {
    const char* name;
    const char* description;
    const char* usage;
    const char* parameters;
    const char* examples;
    CommandHandler handler;
};

static std::unordered_map<std::string, Command> g_commands;
static const char* PROGRAM_NAME = "ohos-usageStatsQuery";
static const char* TOOL_DESCRIPTION =
    "Application usage statistics query tool，"
    "提供应用使用时长、事件记录、高频使用时段等查询功能。"
    "用于开发者或系统管理员分析应用使用情况。"
    "仅支持具有ohos.permission.BUNDLE_ACTIVE_INFO权限的系统应用调用。";

int OutputSuccess(cJSON* data)
{
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "type", "result");
    cJSON_AddStringToObject(response, "status", "success");
    cJSON_AddItemToObject(response, "data", data);
    char* jsonStr = cJSON_PrintUnformatted(response);
    std::cout << jsonStr << std::endl;
    free(jsonStr);
    cJSON_Delete(response);
    return 0;
}

int OutputError(const std::string& code, const std::string& message, const std::string& suggestion)
{
    cJSON* response = cJSON_CreateObject();
    cJSON_AddStringToObject(response, "type", "result");
    cJSON_AddStringToObject(response, "status", "failed");
    cJSON_AddStringToObject(response, "data", "");
    cJSON_AddStringToObject(response, "errCode", code.c_str());
    cJSON_AddStringToObject(response, "errMsg", message.c_str());
    cJSON_AddStringToObject(response, "suggestion", suggestion.c_str());
    char* jsonStr = cJSON_PrintUnformatted(response);
    std::cout << jsonStr << std::endl;
    free(jsonStr);
    cJSON_Delete(response);
    return 1;
}

std::string ParseArg(int argc, char** argv, const std::string& key, const std::string& defaultValue = "")
{
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.find("--" + key) == 0) {
            if (arg.find("=") != std::string::npos) {
                return arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                return argv[i + 1];
            }
        }
    }
    return defaultValue;
}

int32_t ParseArgInt(int argc, char** argv, const std::string& key, int32_t defaultValue = 0)
{
    std::string value = ParseArg(argc, argv, key);
    if (value.empty()) {
        return defaultValue;
    }
    char* endptr = nullptr;
    errno = 0;
    long result = strtol(value.c_str(), &endptr, DECIMAL_BASE);
    if (errno == ERANGE || endptr == nullptr || *endptr != '\0') {
        return defaultValue;
    }
    if (result < INT32_MIN || result > INT32_MAX) {
        return defaultValue;
    }
    return static_cast<int32_t>(result);
}

int64_t ParseArgLong(int argc, char** argv, const std::string& key, int64_t defaultValue = 0)
{
    std::string value = ParseArg(argc, argv, key);
    if (value.empty()) {
        return defaultValue;
    }
    char* endptr = nullptr;
    errno = 0;
    long long result = strtoll(value.c_str(), &endptr, DECIMAL_BASE);
    if (errno == ERANGE || endptr == nullptr || *endptr != '\0') {
        return defaultValue;
    }
    if (result < INT64_MIN || result > INT64_MAX) {
        return defaultValue;
    }
    return static_cast<int64_t>(result);
}

bool CheckHelpFlag(int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            return true;
        }
    }
    return false;
}

void ShowQueryStatsIntervalHelp()
{
CLI_LOG("%s query-stats-interval - 按时间间隔查询应用使用统计。"
    "用于分析应用在特定时间范围内的使用时长和启动次数。"
    "仅支持有效的时间范围查询。", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Usage:");
CLI_LOG("  %s query-stats-interval --interval <type> --begin <timestamp> "
    "--end <timestamp> [--user <userId>]", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Parameters:");
CLI_LOG("  --interval <integer>   时间间隔类型（必需，范围：0-3，"
    "0=按天，1=按周，2=按月，3=按年）");
CLI_LOG("  --begin <number>       开始时间戳（必需，单位：毫秒）");
CLI_LOG("  --end <number>         结束时间戳（必需，单位：毫秒）");
CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
CLI_LOG("  --help                 显示帮助信息");
CLI_LOG("");
CLI_LOG("Examples:");
CLI_LOG("  %s query-stats-interval --interval 0 --begin 1609459200000 "
    "--end 1609545600000", PROGRAM_NAME);
CLI_LOG("  %s query-stats-interval --interval 1 --begin 1609459200000 "
    "--end 1610064000000 --user 100", PROGRAM_NAME);
}

void ShowQueryEventsHelp()
{
CLI_LOG("%s query-events - 查询应用事件记录。"
    "用于获取应用在特定时间范围内的使用事件（如启动、切换等）。"
    "仅支持有效的时间范围查询。", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Usage:");
CLI_LOG("  %s query-events --begin <timestamp> --end <timestamp> "
    "[--user <userId>] [--max <count>]", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Parameters:");
CLI_LOG("  --begin <number>       开始时间戳（必需，单位：毫秒）");
CLI_LOG("  --end <number>         结束时间戳（必需，单位：毫秒）");
CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
CLI_LOG("  --max <integer>        最大返回数量（可选，范围：1-1000，默认：1000）");
CLI_LOG("  --help                 显示帮助信息");
CLI_LOG("");
CLI_LOG("Examples:");
CLI_LOG("  %s query-events --begin 1609459200000 --end 1609545600000",
    PROGRAM_NAME);
CLI_LOG("  %s query-events --begin 1609459200000 --end 1609545600000 "
    "--max 500 --user 100", PROGRAM_NAME);
}

void ShowQueryHighFreqBundleHelp()
{
CLI_LOG("%s query-high-freq-bundle - 查询高频使用应用列表。"
    "用于获取最近一段时间内使用频率最高的应用。"
    "仅支持有效的查询参数。", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Usage:");
CLI_LOG("  %s query-high-freq-bundle [--user <userId>] [--max <count>] "
    "[--days <range>]", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Parameters:");
CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
CLI_LOG("  --max <integer>        最大返回数量（可选，范围：1-1000，默认：20）");
CLI_LOG("  --days <integer>       查询天数范围（可选，默认：7）");
CLI_LOG("  --help                 显示帮助信息");
CLI_LOG("");
CLI_LOG("Examples:");
CLI_LOG("  %s query-high-freq-bundle", PROGRAM_NAME);
CLI_LOG("  %s query-high-freq-bundle --max 50 --days 14", PROGRAM_NAME);
CLI_LOG("  %s query-high-freq-bundle --user 100 --max 50 --days 14", PROGRAM_NAME);
}

void ShowQueryNotificationStatsHelp()
{
CLI_LOG("%s query-notification-stats - 查询通知事件统计。"
    "用于获取应用在特定时间范围内的通知使用情况。"
    "仅支持有效的时间范围查询。", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Usage:");
CLI_LOG("  %s query-notification-stats --begin <timestamp> --end <timestamp> "
    "[--user <userId>]", PROGRAM_NAME);
CLI_LOG("");
CLI_LOG("Parameters:");
CLI_LOG("  --begin <number>       开始时间戳（必需，单位：毫秒）");
CLI_LOG("  --end <number>         结束时间戳（必需，单位：毫秒）");
CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
CLI_LOG("  --help                 显示帮助信息");
CLI_LOG("");
CLI_LOG("Examples:");
CLI_LOG("  %s query-notification-stats --begin 1609459200000 "
    "--end 1609545600000", PROGRAM_NAME);
CLI_LOG("  %s query-notification-stats --begin 1609459200000 "
    "--end 1609545600000 --user 100", PROGRAM_NAME);
}

cJSON* PackageStatsToJson(const BundleActivePackageStats& stats)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "bundleName", stats.bundleName_.c_str());
    cJSON_AddNumberToObject(obj, "beginTimeStamp", stats.beginTimeStamp_);
    cJSON_AddNumberToObject(obj, "endTimeStamp", stats.endTimeStamp_);
    cJSON_AddNumberToObject(obj, "lastTimeUsed", stats.lastTimeUsed_);
    cJSON_AddNumberToObject(obj, "totalInFrontTime", stats.totalInFrontTime_);
    cJSON_AddNumberToObject(obj, "lastContiniousTaskUsed", stats.lastContiniousTaskUsed_);
    cJSON_AddNumberToObject(obj, "totalContiniousTaskUsedTime", stats.totalContiniousTaskUsedTime_);
    cJSON_AddNumberToObject(obj, "startCount", stats.startCount_);
    cJSON_AddNumberToObject(obj, "bundleStartedCount", stats.bundleStartedCount_);
    cJSON_AddNumberToObject(obj, "lastEvent", stats.lastEvent_);
    cJSON_AddNumberToObject(obj, "userId", stats.userId_);
    cJSON_AddNumberToObject(obj, "uid", stats.uid_);
    cJSON_AddNumberToObject(obj, "appIndex", stats.appIndex_);
    return obj;
}

cJSON* EventToJson(const BundleActiveEvent& event)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "bundleName", event.bundleName_.c_str());
    cJSON_AddNumberToObject(obj, "timeStamp", event.timeStamp_);
    cJSON_AddNumberToObject(obj, "eventId", event.eventId_);
    cJSON_AddNumberToObject(obj, "uid", event.uid_);
    cJSON_AddStringToObject(obj, "abilityName", event.abilityName_.c_str());
    cJSON_AddStringToObject(obj, "abilityId", event.abilityId_.c_str());
    cJSON_AddStringToObject(obj, "moduleName", event.moduleName_.c_str());
    cJSON_AddStringToObject(obj, "continuousTaskAbilityName", event.continuousTaskAbilityName_.c_str());
    return obj;
}

cJSON* EventStatsToJson(const BundleActiveEventStats& stats)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddNumberToObject(obj, "eventId", stats.eventId_);
    cJSON_AddNumberToObject(obj, "beginTimeStamp", stats.beginTimeStamp_);
    cJSON_AddNumberToObject(obj, "endTimeStamp", stats.endTimeStamp_);
    cJSON_AddNumberToObject(obj, "lastEventTime", stats.lastEventTime_);
    cJSON_AddNumberToObject(obj, "totalTime", stats.totalTime_);
    cJSON_AddNumberToObject(obj, "count", stats.count_);
    cJSON_AddNumberToObject(obj, "uid", stats.uid_);
    cJSON_AddStringToObject(obj, "name", stats.name_.c_str());
    return obj;
}

cJSON* ModuleRecordToJson(const BundleActiveModuleRecord& record)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "bundleName", record.bundleName_.c_str());
    cJSON_AddStringToObject(obj, "moduleName", record.moduleName_.c_str());
    cJSON_AddStringToObject(obj, "abilityName", record.abilityName_.c_str());
    cJSON_AddStringToObject(obj, "deviceId", record.deviceId_.c_str());
    cJSON_AddNumberToObject(obj, "launchedCount", record.launchedCount_);
    cJSON_AddNumberToObject(obj, "lastModuleUsedTime", record.lastModuleUsedTime_);
    cJSON_AddNumberToObject(obj, "userId", record.userId_);
    cJSON_AddNumberToObject(obj, "uid", record.uid_);
    cJSON_AddBoolToObject(obj, "removed", record.removed_);
    cJSON_AddBoolToObject(obj, "installFreeSupported", record.installFreeSupported_);
    cJSON_AddBoolToObject(obj, "isNewAdded", record.isNewAdded_);
    return obj;
}

cJSON* HighFrequencyPeriodToJson(const BundleActiveHighFrequencyPeriod& period)
{
    cJSON* obj = cJSON_CreateObject();
    cJSON_AddStringToObject(obj, "bundleName", period.bundleName_.c_str());
    cJSON* highFreqHours = cJSON_CreateArray();
    for (size_t i = 0; i < period.highFreqHours_.size(); i++) {
        cJSON_AddItemToArray(highFreqHours, cJSON_CreateNumber(period.highFreqHours_[i]));
    }
    cJSON_AddItemToObject(obj, "highFreqHours", highFreqHours);
    return obj;
}

int CmdCheckBundleIdle(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s check-bundle-idle - 检查指定应用是否处于空闲状态。"
        "用于判断应用是否可被系统优化。仅适用于已安装应用。", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s check-bundle-idle --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      应用包名（必需，格式：com.example.app）");
        CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
        CLI_LOG("  --help                 显示帮助信息");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s check-bundle-idle --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s check-bundle-idle --bundle com.example.app --user 100", PROGRAM_NAME);
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "bundleName参数缺失",
            "请使用 --bundle <应用包名> 提供应用包名参数，例如：--bundle com.example.app");
    }

    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);

    bool isBundleIdle = false;
    ErrCode ret = BundleActiveClient::GetInstance().IsBundleIdle(isBundleIdle, bundleName, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "查询应用空闲状态失败",
        "请检查：1. BundleActiveService是否正常运行；"
        "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }

    cJSON* data = cJSON_CreateObject();
    cJSON_AddBoolToObject(data, "isBundleIdle", isBundleIdle);
    cJSON_AddStringToObject(data, "bundleName", bundleName.c_str());
    cJSON_AddNumberToObject(data, "userId", userId);
    return OutputSuccess(data);
}

int CmdCheckBundlePeriod(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s check-bundle-period - 检查应用是否在使用时段（仅Native Token可调用）。"
        "用于判断应用活跃状态。不适用于系统应用调用。", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s check-bundle-period --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      应用包名（必需，格式：com.example.app）");
        CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
        CLI_LOG("  --help                 显示帮助信息");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s check-bundle-period --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s check-bundle-period --bundle com.example.app --user 100", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("注意：此命令仅支持Native Token调用，系统应用无法使用。");
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "bundleName参数缺失",
            "请使用 --bundle <应用包名> 提供应用包名参数，例如：--bundle com.example.app");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    bool isUsePeriod = false;
    ErrCode ret = BundleActiveClient::GetInstance().IsBundleUsePeriod(isUsePeriod, bundleName, userId);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "查询应用使用时段失败",
            "请检查：1. BundleActiveService是否正常运行；2. 此命令仅支持Native Token调用");
    }

    cJSON* data = cJSON_CreateObject();
    cJSON_AddBoolToObject(data, "isUsePeriod", isUsePeriod);
    cJSON_AddStringToObject(data, "bundleName", bundleName.c_str());
    cJSON_AddNumberToObject(data, "userId", userId);
    return OutputSuccess(data);
}

int CmdQueryStatsInterval(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        ShowQueryStatsIntervalHelp();
        return 0;
    }
    int32_t intervalType = ParseArgInt(argc, argv, "interval", DEFAULT_INTERVAL_TYPE);
    if (intervalType < 0 || intervalType > MAX_INTERVAL_TYPE) {
        return OutputError("ERR_INTERVAL_TYPE", "intervalType参数无效",
            "请使用有效的 --interval 参数（0-3），0=按天，1=按周，2=按月，3=按年");
    }
    int64_t beginTime = ParseArgLong(argc, argv, "begin", 0);
    int64_t endTime = ParseArgLong(argc, argv, "end", 0);
    if (beginTime <= 0 || endTime <= 0) {
        return OutputError("ERR_TIME_INVALID", "时间参数缺失或无效",
            "请提供有效的 --begin 和 --end 时间戳参数（单位：毫秒），"
            "例如：--begin 1609459200000 --end 1609545600000");
    }
    if (endTime <= beginTime) {
        return OutputError("ERR_TIME_INTERVAL", "时间范围无效",
            "结束时间必须大于开始时间，请调整时间范围");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    std::vector<BundleActivePackageStats> packageStats;
    ErrCode ret = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(
        packageStats, intervalType, beginTime, endTime, userId);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "查询使用统计失败",
            "请检查：1. BundleActiveService是否正常运行；"
            "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }
    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "intervalType", intervalType);
    cJSON_AddNumberToObject(data, "beginTime", beginTime);
    cJSON_AddNumberToObject(data, "endTime", endTime);
    cJSON_AddNumberToObject(data, "userId", userId);
    cJSON* statsArray = cJSON_CreateArray();
    for (const auto& stats : packageStats) {
        cJSON_AddItemToArray(statsArray, PackageStatsToJson(stats));
    }
    cJSON_AddItemToObject(data, "packageStats", statsArray);
    cJSON_AddNumberToObject(data, "count", packageStats.size());
    return OutputSuccess(data);
}

int CmdQueryEvents(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        ShowQueryEventsHelp();
        return 0;
    }
    int64_t beginTime = ParseArgLong(argc, argv, "begin", 0);
    int64_t endTime = ParseArgLong(argc, argv, "end", 0);
    if (beginTime <= 0 || endTime <= 0) {
        return OutputError("ERR_TIME_INVALID", "时间参数缺失或无效",
            "请提供有效的 --begin 和 --end 时间戳参数（单位：毫秒），"
            "例如：--begin 1609459200000 --end 1609545600000");
    }
    if (endTime <= beginTime) {
        return OutputError("ERR_TIME_INTERVAL", "时间范围无效",
            "结束时间必须大于开始时间，请调整时间范围");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int32_t maxNum = ParseArgInt(argc, argv, "max", MAX_RETURN_COUNT);
    if (maxNum < MIN_RETURN_COUNT || maxNum > MAX_RETURN_COUNT) {
        return OutputError("ERR_MAXNUM_INVALID", "maxNum参数无效",
            "请使用有效的 --max 参数（范围：1-1000），例如：--max 500");
    }
    std::vector<BundleActiveEvent> events;
    ErrCode ret = BundleActiveClient::GetInstance().QueryBundleEvents(
        events, beginTime, endTime, userId, maxNum);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "查询应用事件失败",
            "请检查：1. BundleActiveService是否正常运行；"
            "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }
    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "beginTime", beginTime);
    cJSON_AddNumberToObject(data, "endTime", endTime);
    cJSON_AddNumberToObject(data, "userId", userId);
    cJSON_AddNumberToObject(data, "maxNum", maxNum);
    cJSON* eventsArray = cJSON_CreateArray();
    for (const auto& event : events) {
        cJSON_AddItemToArray(eventsArray, EventToJson(event));
    }
    cJSON_AddItemToObject(data, "events", eventsArray);
    cJSON_AddNumberToObject(data, "count", events.size());
    return OutputSuccess(data);
}

int CmdQueryAppGroup(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s query-app-group - 查询应用优先级分组。"
        "用于获取应用在资源调度中的优先级等级。仅适用于已安装应用。", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-app-group --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      应用包名（必需，格式：com.example.app）");
        CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
        CLI_LOG("  --help                 显示帮助信息");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s query-app-group --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s query-app-group --bundle com.example.app --user 100", PROGRAM_NAME);
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "bundleName参数缺失",
            "请使用 --bundle <应用包名> 提供应用包名参数，例如：--bundle com.example.app");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int32_t appGroup = 0;
    ErrCode ret = BundleActiveClient::GetInstance().QueryAppGroup(appGroup, bundleName, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "查询应用优先级分组失败",
        "请检查：1. BundleActiveService是否正常运行；"
        "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }

    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "appGroup", appGroup);
    cJSON_AddStringToObject(data, "bundleName", bundleName.c_str());
    cJSON_AddNumberToObject(data, "userId", userId);
    return OutputSuccess(data);
}

int CmdQueryHighFreqBundle(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        ShowQueryHighFreqBundleHelp();
        return 0;
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int32_t maxNum = ParseArgInt(argc, argv, "max", DEFAULT_HIGH_FREQ_COUNT);
    int32_t queryDayRange = ParseArgInt(argc, argv, "days", DEFAULT_DAY_RANGE);
    if (maxNum < MIN_RETURN_COUNT || maxNum > MAX_RETURN_COUNT) {
        return OutputError("ERR_MAXNUM_INVALID", "maxNum参数无效",
            "请使用有效的 --max 参数（范围：1-1000），例如：--max 50");
    }
    std::vector<BundleActivePackageStats> packageStats;
    ErrCode ret = BundleActiveClient::GetInstance().QueryHighFrequencyUsageBundleInfos(
        packageStats, userId, maxNum, queryDayRange);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "查询高频使用应用失败",
            "请检查：1. BundleActiveService是否正常运行；"
            "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }
    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "userId", userId);
    cJSON_AddNumberToObject(data, "maxNum", maxNum);
    cJSON_AddNumberToObject(data, "queryDayRange", queryDayRange);
    cJSON* statsArray = cJSON_CreateArray();
    for (const auto& stats : packageStats) {
        cJSON_AddItemToArray(statsArray, PackageStatsToJson(stats));
    }
    cJSON_AddItemToObject(data, "packageStats", statsArray);
    cJSON_AddNumberToObject(data, "count", packageStats.size());
    return OutputSuccess(data);
}

int CmdQueryModuleRecords(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s query-module-records - 查询模块使用记录。"
        "用于获取应用模块（如Ability）的使用情况。"
        "仅支持有效的查询参数。", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-module-records [--max <count>] [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --max <integer>        最大返回数量（可选，范围：1-1000，默认：1000）");
        CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
        CLI_LOG("  --help                 显示帮助信息");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s query-module-records", PROGRAM_NAME);
        CLI_LOG("  %s query-module-records --max 500", PROGRAM_NAME);
        CLI_LOG("  %s query-module-records --max 500 --user 100", PROGRAM_NAME);
        return 0;
    }

    int32_t maxNum = ParseArgInt(argc, argv, "max", MAX_RETURN_COUNT);
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);

    if (maxNum < MIN_RETURN_COUNT || maxNum > MAX_RETURN_COUNT) {
        return OutputError("ERR_MAXNUM_INVALID", "maxNum参数无效",
            "请使用有效的 --max 参数（范围：1-1000），例如：--max 500");
    }

    std::vector<BundleActiveModuleRecord> results;
    ErrCode ret = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "查询模块使用记录失败",
        "请检查：1. BundleActiveService是否正常运行；"
        "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }

    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "maxNum", maxNum);
    cJSON_AddNumberToObject(data, "userId", userId);
    cJSON* recordsArray = cJSON_CreateArray();
    for (const auto& record : results) {
        cJSON_AddItemToArray(recordsArray, ModuleRecordToJson(record));
    }
    cJSON_AddItemToObject(data, "moduleRecords", recordsArray);
    cJSON_AddNumberToObject(data, "count", results.size());
    return OutputSuccess(data);
}

int CmdQueryNotificationStats(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        ShowQueryNotificationStatsHelp();
        return 0;
    }
    int64_t beginTime = ParseArgLong(argc, argv, "begin", 0);
    int64_t endTime = ParseArgLong(argc, argv, "end", 0);
    if (beginTime <= 0 || endTime <= 0) {
        return OutputError("ERR_TIME_INVALID", "时间参数缺失或无效",
            "请提供有效的 --begin 和 --end 时间戳参数（单位：毫秒），"
            "例如：--begin 1609459200000 --end 1609545600000");
    }
    if (endTime <= beginTime) {
        return OutputError("ERR_TIME_INTERVAL", "时间范围无效",
            "结束时间必须大于开始时间，请调整时间范围");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    std::vector<BundleActiveEventStats> eventStats;
    ErrCode ret = BundleActiveClient::GetInstance().QueryNotificationEventStats(
        beginTime, endTime, eventStats, userId);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "查询通知事件统计失败",
            "请检查：1. BundleActiveService是否正常运行；"
            "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }
    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "beginTime", beginTime);
    cJSON_AddNumberToObject(data, "endTime", endTime);
    cJSON_AddNumberToObject(data, "userId", userId);
    cJSON* statsArray = cJSON_CreateArray();
    for (const auto& stats : eventStats) {
        cJSON_AddItemToArray(statsArray, EventStatsToJson(stats));
    }
    cJSON_AddItemToObject(data, "eventStats", statsArray);
    cJSON_AddNumberToObject(data, "count", eventStats.size());
    return OutputSuccess(data);
}

int CmdQueryHighFreqPeriod(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s query-high-freq-period - 查询高频使用时段。"
        "用于获取应用高频使用的小时分布情况。仅支持有效的用户查询。", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-high-freq-period [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
        CLI_LOG("  --help                 显示帮助信息");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s query-high-freq-period", PROGRAM_NAME);
        CLI_LOG("  %s query-high-freq-period --user 100", PROGRAM_NAME);
        return 0;
    }

    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);

    std::vector<BundleActiveHighFrequencyPeriod> appFreqHours;
    ErrCode ret = BundleActiveClient::GetInstance().QueryHighFrequencyPeriodBundle(appFreqHours, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "查询高频使用时段失败",
        "请检查：1. BundleActiveService是否正常运行；"
        "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }
    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "userId", userId);
    cJSON* periodsArray = cJSON_CreateArray();
    for (const auto& period : appFreqHours) {
        cJSON_AddItemToArray(periodsArray, HighFrequencyPeriodToJson(period));
    }
    cJSON_AddItemToObject(data, "highFreqPeriods", periodsArray);
    cJSON_AddNumberToObject(data, "count", appFreqHours.size());
    return OutputSuccess(data);
}

int CmdQueryLatestUsedTime(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s query-latest-used-time - 查询应用今日最后使用时间。"
        "用于获取应用当天最后一次被使用的时间戳。"
        "仅适用于已安装应用。", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-latest-used-time --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      应用包名（必需，格式：com.example.app）");
        CLI_LOG("  --user <integer>       用户ID（可选，默认：-1 表示当前用户）");
        CLI_LOG("  --help                 显示帮助信息");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s query-latest-used-time --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s query-latest-used-time --bundle com.example.app --user 100", PROGRAM_NAME);
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "bundleName参数缺失",
            "请使用 --bundle <应用包名> 提供应用包名参数，例如：--bundle com.example.app");
    }

    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int64_t latestUsedTime = 0;
    ErrCode ret = BundleActiveClient::GetInstance().QueryBundleTodayLatestUsedTime(
        latestUsedTime, bundleName, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "查询应用今日最后使用时间失败",
        "请检查：1. BundleActiveService是否正常运行；"
        "2. 是否具有ohos.permission.BUNDLE_ACTIVE_INFO权限");
    }

    cJSON* data = cJSON_CreateObject();
    cJSON_AddNumberToObject(data, "latestUsedTime", latestUsedTime);
    cJSON_AddStringToObject(data, "bundleName", bundleName.c_str());
    cJSON_AddNumberToObject(data, "userId", userId);
    return OutputSuccess(data);
}

void ShowMainHelp()
{
    CLI_LOG("%s - %s", PROGRAM_NAME, TOOL_DESCRIPTION);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s <command> [options]", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --help                 显示帮助信息");
    CLI_LOG("");
    CLI_LOG("SubCommands:");
    CLI_LOG("  check-bundle-idle          检查应用是否空闲");
    CLI_LOG("  check-bundle-period        检查应用是否在使用时段（仅Native Token）");
    CLI_LOG("  query-stats-interval       按时间间隔查询使用统计");
    CLI_LOG("  query-events               查询应用事件记录");
    CLI_LOG("  query-app-group            查询应用优先级分组");
    CLI_LOG("  query-high-freq-bundle     查询高频使用应用");
    CLI_LOG("  query-module-records       查询模块使用记录");
    CLI_LOG("  query-notification-stats   查询通知事件统计");
    CLI_LOG("  query-high-freq-period     查询高频使用时段");
    CLI_LOG("  query-latest-used-time     查询应用今日最后使用时间");
    CLI_LOG("");
    CLI_LOG("Examples:");
    CLI_LOG("  %s --help", PROGRAM_NAME);
    CLI_LOG("  %s check-bundle-idle --help", PROGRAM_NAME);
    CLI_LOG("  %s check-bundle-idle --bundle com.example.app", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("注意：所有命令需要 ohos.permission.BUNDLE_ACTIVE_INFO 权限");
}

int CmdHelp(int argc, char** argv)
{
    std::string targetCmd;
    int startIdx = ARG_START_INDEX;
    if (argc > startIdx && strcmp(argv[startIdx], "help") == 0) {
        startIdx = HELP_CMD_ARG_START_INDEX;
    }
    for (int i = startIdx; i < argc; i++) {
        if (argv[i][0] != '-') {
            targetCmd = argv[i];
            break;
        }
    }

    if (targetCmd.empty()) {
        ShowMainHelp();
        return 0;
    }

    auto it = g_commands.find(targetCmd);
    if (it == g_commands.end()) {
        return OutputError("ERR_INVALID_COMMAND", std::string("未知命令：") + targetCmd,
            "请运行 " + std::string(PROGRAM_NAME) + " --help 查看可用命令列表");
    }

    char* helpArgv[HELP_ARGC_WITH_CMD] = {
        const_cast<char*>(PROGRAM_NAME),
        const_cast<char*>("--help"),
        nullptr
    };
    return it->second.handler(HELP_ARGC_WITH_CMD, helpArgv);
}

void InitCommands()
{
    g_commands["check-bundle-idle"] = {
        "check-bundle-idle", "检查应用是否空闲", nullptr, nullptr, nullptr, CmdCheckBundleIdle
    };
    g_commands["check-bundle-period"] = {
        "check-bundle-period", "检查应用是否在使用时段（仅Native Token）",
        nullptr, nullptr, nullptr, CmdCheckBundlePeriod
    };
    g_commands["query-stats-interval"] = {
        "query-stats-interval", "按时间间隔查询使用统计",
        nullptr, nullptr, nullptr, CmdQueryStatsInterval
    };
    g_commands["query-events"] = {
        "query-events", "查询应用事件记录", nullptr, nullptr, nullptr, CmdQueryEvents
    };
    g_commands["query-app-group"] = {
        "query-app-group", "查询应用优先级分组",
        nullptr, nullptr, nullptr, CmdQueryAppGroup
    };
    g_commands["query-high-freq-bundle"] = {
        "query-high-freq-bundle", "查询高频使用应用",
        nullptr, nullptr, nullptr, CmdQueryHighFreqBundle
    };
    g_commands["query-module-records"] = {
        "query-module-records", "查询模块使用记录",
        nullptr, nullptr, nullptr, CmdQueryModuleRecords
    };
    g_commands["query-notification-stats"] = {
        "query-notification-stats", "查询通知事件统计",
        nullptr, nullptr, nullptr, CmdQueryNotificationStats
    };
    g_commands["query-high-freq-period"] = {
        "query-high-freq-period", "查询高频使用时段",
        nullptr, nullptr, nullptr, CmdQueryHighFreqPeriod
    };
    g_commands["query-latest-used-time"] = {
        "query-latest-used-time", "查询应用今日最后使用时间",
        nullptr, nullptr, nullptr, CmdQueryLatestUsedTime
    };
    g_commands["help"] = {
        "help", "显示帮助信息", nullptr, nullptr, nullptr, CmdHelp
    };
}

void PrintUsage(const char* prog)
{
CLI_ERROR("用法：%s <command> [options]", prog);
CLI_ERROR("运行 %s --help 获取更多信息", prog);
}

int main(int argc, char** argv)
{
    if (argc >= MIN_ARG_COUNT && strcmp(argv[1], "--help") == 0) {
        InitCommands();
        CmdHelp(HELP_ARGC_MINIMAL, argv);
        return 0;
    }

    if (argc < MIN_ARG_COUNT) {
        PrintUsage(argv[0]);
        return 1;
    }

    InitCommands();

    std::string cmdName = argv[1];

    for (int i = MIN_ARG_COUNT; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0) {
            auto it = g_commands.find(cmdName);
            if (it != g_commands.end()) {
                return it->second.handler(argc, argv);
            }
        }
    }

    auto it = g_commands.find(cmdName);
    if (it == g_commands.end()) {
        return OutputError("ERR_INVALID_COMMAND",
            std::string("Unknown command: ") + cmdName,
            std::string("Please run ") + PROGRAM_NAME + " --help to see available commands");
    }

    int cmdArgc = argc - MIN_ARG_COUNT;
    char** cmdArgv = argv + MIN_ARG_COUNT;

    return it->second.handler(cmdArgc, cmdArgv);
}
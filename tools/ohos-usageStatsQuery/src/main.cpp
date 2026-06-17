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
#include "privacy_kit.h"
#include "ipc_skeleton.h"
#include "access_token.h"

using namespace OHOS::Security::AccessToken;

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
#define OUTPUT_SUCCESS_CODE (0)
#define OUTPUT_ERROR_CODE (1)

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
    "Application usage statistics query tool. "
    "Provides query functions for application usage duration, event records, "
    "high-frequency usage periods, etc. Used by developers or system administrators "
    "to analyze application usage. Requires ohos.permission.cli.BUNDLE_ACTIVE_INFO permission.";

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
    // 上报敏感权限使用记录
    AccessTokenID tokenID = OHOS::IPCSkeleton::GetSelfTokenID();
    PrivacyKit::AddPermissionUsedRecord(tokenID, "ohos.permission.cli.BUNDLE_ACTIVE_INFO", 1, 0);
    return OUTPUT_SUCCESS_CODE;
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
    // 上报敏感权限使用记录
    AccessTokenID tokenID = OHOS::IPCSkeleton::GetSelfTokenID();
    PrivacyKit::AddPermissionUsedRecord(tokenID, "ohos.permission.cli.BUNDLE_ACTIVE_INFO", 0, 1);
    return OUTPUT_ERROR_CODE;
}

std::string ParseArg(int argc, char** argv, const std::string& key, const std::string& defaultValue = "")
{
    if (argc <= 0 || argv == nullptr) {
        return defaultValue;
    }
    for (int i = 0; i < argc; i++) {
        if (argv[i] == nullptr) {
            continue;
        }
        std::string arg = argv[i];
        if (arg.find("--" + key) == 0) {
            if (arg.find("=") != std::string::npos) {
                return arg.substr(arg.find("=") + 1);
            } else if (i + 1 < argc && argv[i + 1] != nullptr && argv[i + 1][0] != '-') {
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
    size_t dotPos = value.find('.');
    if (dotPos != std::string::npos) {
        value = value.substr(0, dotPos);
    }
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
    size_t dotPos = value.find('.');
    if (dotPos != std::string::npos) {
        value = value.substr(0, dotPos);
    }
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
    if (argc <= 0 || argv == nullptr) {
        return false;
    }
    for (int i = 0; i < argc; i++) {
        if (argv[i] != nullptr && strcmp(argv[i], "--help") == 0) {
            return true;
        }
    }
    return false;
}

void ShowQueryStatsIntervalHelp()
{
    CLI_LOG("%s query-stats-interval - Query application usage statistics by time interval. "
        "Analyzes application usage duration and startup counts within a specific time range. "
        "Only supports valid time range queries.", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s query-stats-interval --interval <type> --begin <timestamp> "
        "--end <timestamp> [--user <userId>]", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --interval <integer>   Time interval type (required, range: 0-3, "
        "0=daily, 1=weekly, 2=monthly, 3=yearly)");
    CLI_LOG("  --begin <number>       Begin timestamp (required, unit: milliseconds)");
    CLI_LOG("  --end <number>         End timestamp (required, unit: milliseconds)");
    CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
    CLI_LOG("  --help                 Show help information");
    CLI_LOG("");
    CLI_LOG("Examples:");
    CLI_LOG("  %s query-stats-interval --interval 0 --begin 1609459200000 "
        "--end 1609545600000", PROGRAM_NAME);
    CLI_LOG("  %s query-stats-interval --interval 1 --begin 1609459200000 "
        "--end 1610064000000 --user 100", PROGRAM_NAME);
}

void ShowQueryEventsHelp()
{
    CLI_LOG("%s query-events - Query application event records. "
        "Retrieves application usage events within a specific time range "
        "(such as startup, switch, etc.). Only supports valid time range queries.", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s query-events --begin <timestamp> --end <timestamp> "
        "[--user <userId>] [--max <count>]", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --begin <number>       Begin timestamp (required, unit: milliseconds)");
    CLI_LOG("  --end <number>         End timestamp (required, unit: milliseconds)");
    CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
    CLI_LOG("  --max <integer>        Max return count (optional, range: 1-1000, default: 1000)");
    CLI_LOG("  --help                 Show help information");
    CLI_LOG("");
    CLI_LOG("Examples:");
    CLI_LOG("  %s query-events --begin 1609459200000 --end 1609545600000",
        PROGRAM_NAME);
    CLI_LOG("  %s query-events --begin 1609459200000 --end 1609545600000 "
        "--max 500 --user 100", PROGRAM_NAME);
}

void ShowQueryHighFreqBundleHelp()
{
    CLI_LOG("%s query-high-freq-bundle - Query high-frequency usage application list. "
        "Retrieves the most frequently used applications within a recent time period. "
        "Only supports valid query parameters.", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s query-high-freq-bundle [--user <userId>] [--max <count>] "
        "[--days <range>]", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
    CLI_LOG("  --max <integer>        Max return count (optional, range: 1-1000, default: 20)");
    CLI_LOG("  --days <integer>       Query day range (optional, default: 7)");
    CLI_LOG("  --help                 Show help information");
    CLI_LOG("");
    CLI_LOG("Examples:");
    CLI_LOG("  %s query-high-freq-bundle", PROGRAM_NAME);
    CLI_LOG("  %s query-high-freq-bundle --max 50 --days 14", PROGRAM_NAME);
    CLI_LOG("  %s query-high-freq-bundle --user 100 --max 50 --days 14", PROGRAM_NAME);
}

void ShowQueryNotificationStatsHelp()
{
    CLI_LOG("%s query-notification-stats - Query notification event statistics. "
        "Retrieves application notification usage within a specific time range. "
        "Only supports valid time range queries.", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Usage:");
    CLI_LOG("  %s query-notification-stats --begin <timestamp> --end <timestamp> "
        "[--user <userId>]", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Parameters:");
    CLI_LOG("  --begin <number>       Begin timestamp (required, unit: milliseconds)");
    CLI_LOG("  --end <number>         End timestamp (required, unit: milliseconds)");
    CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
    CLI_LOG("  --help                 Show help information");
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
    for (const auto& hour : period.highFreqHours_) {
        cJSON_AddItemToArray(highFreqHours, cJSON_CreateNumber(hour));
    }
    cJSON_AddItemToObject(obj, "highFreqHours", highFreqHours);
    return obj;
}

int CmdCheckBundleIdle(int argc, char** argv)
{
    if (CheckHelpFlag(argc, argv)) {
        CLI_LOG("%s check-bundle-idle - Check if the specified application is in idle state. "
        "Used to determine whether the application can be optimized by the system. "
        "Only applicable to installed applications.", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s check-bundle-idle --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      Bundle name (required, format: com.example.app)");
        CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
        CLI_LOG("  --help                 Show help information");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s check-bundle-idle --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s check-bundle-idle --bundle com.example.app --user 100", PROGRAM_NAME);
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "Missing bundleName parameter",
            "Please use --bundle <bundle_name> to provide the bundle name parameter, "
            "e.g., --bundle com.example.app");
    }

    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);

    bool isBundleIdle = false;
    ErrCode ret = BundleActiveClient::GetInstance().IsBundleIdle(isBundleIdle, bundleName, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "Failed to query application idle status",
        "Please check: 1. Whether BundleActiveService is running normally; "
        "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        CLI_LOG("%s check-bundle-period - Check if application is in usage period (Native Token only). "
        "Used to determine application active status. Not applicable for system application calls.", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s check-bundle-period --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      Bundle name (required, format: com.example.app)");
        CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
        CLI_LOG("  --help                 Show help information");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s check-bundle-period --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s check-bundle-period --bundle com.example.app --user 100", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Note: This command only supports Native Token calls, system applications cannot use it.");
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "Missing bundleName parameter",
            "Please use --bundle <bundle_name> to provide the bundle name parameter, "
            "e.g., --bundle com.example.app");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    bool isUsePeriod = false;
    ErrCode ret = BundleActiveClient::GetInstance().IsBundleUsePeriod(isUsePeriod, bundleName, userId);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "Failed to query application usage period",
            "Please check: 1. Whether BundleActiveService is running normally; "
            "2. This command only supports Native Token calls");
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
        return OutputError("ERR_INTERVAL_TYPE", "Invalid intervalType parameter",
            "Please use a valid --interval parameter (0-3), "
            "0=daily, 1=weekly, 2=monthly, 3=yearly");
    }
    int64_t beginTime = ParseArgLong(argc, argv, "begin", 0);
    int64_t endTime = ParseArgLong(argc, argv, "end", 0);
    if (beginTime <= 0 || endTime <= 0) {
        return OutputError("ERR_TIME_INVALID", "Missing or invalid time parameters",
            "Please provide valid --begin and --end timestamp parameters (unit: milliseconds), "
            "e.g., --begin 1609459200000 --end 1609545600000");
    }
    if (endTime <= beginTime) {
        return OutputError("ERR_TIME_INTERVAL", "Invalid time range",
            "End time must be greater than begin time, please adjust the time range");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    std::vector<BundleActivePackageStats> packageStats;
    ErrCode ret = BundleActiveClient::GetInstance().QueryBundleStatsInfoByInterval(
        packageStats, intervalType, beginTime, endTime, userId);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "Failed to query usage statistics",
            "Please check: 1. Whether BundleActiveService is running normally; "
            "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        return OutputError("ERR_TIME_INVALID", "Missing or invalid time parameters",
            "Please provide valid --begin and --end timestamp parameters (unit: milliseconds), "
            "e.g., --begin 1609459200000 --end 1609545600000");
    }
    if (endTime <= beginTime) {
        return OutputError("ERR_TIME_INTERVAL", "Invalid time range",
            "End time must be greater than begin time, please adjust the time range");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int32_t maxNum = ParseArgInt(argc, argv, "max", MAX_RETURN_COUNT);
    if (maxNum < MIN_RETURN_COUNT || maxNum > MAX_RETURN_COUNT) {
        return OutputError("ERR_MAXNUM_INVALID", "Invalid maxNum parameter",
            "Please use a valid --max parameter (range: 1-1000), e.g., --max 500");
    }
    std::vector<BundleActiveEvent> events;
    ErrCode ret = BundleActiveClient::GetInstance().QueryBundleEvents(
        events, beginTime, endTime, userId, maxNum);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "Failed to query application events",
            "Please check: 1. Whether BundleActiveService is running normally; "
            "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        CLI_LOG("%s query-app-group - Query application priority group. "
        "Used to get the priority level of the application in resource scheduling. "
        "Only applicable to installed applications.", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-app-group --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      Bundle name (required, format: com.example.app)");
        CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
        CLI_LOG("  --help                 Show help information");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s query-app-group --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s query-app-group --bundle com.example.app --user 100", PROGRAM_NAME);
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "Missing bundleName parameter",
            "Please use --bundle <bundle_name> to provide the bundle name parameter, "
            "e.g., --bundle com.example.app");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int32_t appGroup = 0;
    ErrCode ret = BundleActiveClient::GetInstance().QueryAppGroup(appGroup, bundleName, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "Failed to query application priority group",
        "Please check: 1. Whether BundleActiveService is running normally; "
        "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        return OutputError("ERR_MAXNUM_INVALID", "Invalid maxNum parameter",
            "Please use a valid --max parameter (range: 1-1000), e.g., --max 50");
    }
    std::vector<BundleActivePackageStats> packageStats;
    ErrCode ret = BundleActiveClient::GetInstance().QueryHighFrequencyUsageBundleInfos(
        packageStats, userId, maxNum, queryDayRange);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "Failed to query high-frequency usage applications",
            "Please check: 1. Whether BundleActiveService is running normally; "
            "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        CLI_LOG("%s query-module-records - Query module usage records. "
        "Used to get usage information of application modules (e.g., Ability). "
        "Only supports valid query parameters.", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-module-records [--max <count>] [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --max <integer>        Max return count (optional, range: 1-1000, default: 1000)");
        CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
        CLI_LOG("  --help                 Show help information");
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
        return OutputError("ERR_MAXNUM_INVALID", "Invalid maxNum parameter",
            "Please use a valid --max parameter (range: 1-1000), e.g., --max 500");
    }

    std::vector<BundleActiveModuleRecord> results;
    ErrCode ret = BundleActiveClient::GetInstance().QueryModuleUsageRecords(maxNum, results, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "Failed to query module usage records",
        "Please check: 1. Whether BundleActiveService is running normally; "
        "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        return OutputError("ERR_TIME_INVALID", "Missing or invalid time parameters",
            "Please provide valid --begin and --end timestamp parameters (unit: milliseconds), "
            "e.g., --begin 1609459200000 --end 1609545600000");
    }
    if (endTime <= beginTime) {
        return OutputError("ERR_TIME_INTERVAL", "Invalid time range",
            "End time must be greater than begin time, please adjust the time range");
    }
    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    std::vector<BundleActiveEventStats> eventStats;
    ErrCode ret = BundleActiveClient::GetInstance().QueryNotificationEventStats(
        beginTime, endTime, eventStats, userId);
    if (ret != ERR_OK) {
        return OutputError("ERR_QUERY_FAILED", "Failed to query notification event statistics",
            "Please check: 1. Whether BundleActiveService is running normally; "
            "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        CLI_LOG("%s query-high-freq-period - Query high-frequency usage periods. "
        "Used to get the hourly distribution of high-frequency application usage. "
        "Only supports valid user queries.", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-high-freq-period [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
        CLI_LOG("  --help                 Show help information");
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
    return OutputError("ERR_QUERY_FAILED", "Failed to query high-frequency usage periods",
        "Please check: 1. Whether BundleActiveService is running normally; "
        "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
        CLI_LOG("%s query-latest-used-time - Query application latest usage time today. "
        "Used to get the timestamp of the last time the application was used today. "
        "Only applicable to installed applications.", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Usage:");
        CLI_LOG("  %s query-latest-used-time --bundle <bundleName> [--user <userId>]", PROGRAM_NAME);
        CLI_LOG("");
        CLI_LOG("Parameters:");
        CLI_LOG("  --bundle <string>      Bundle name (required, format: com.example.app)");
        CLI_LOG("  --user <integer>       User ID (optional, default: -1 for current user)");
        CLI_LOG("  --help                 Show help information");
        CLI_LOG("");
        CLI_LOG("Examples:");
        CLI_LOG("  %s query-latest-used-time --bundle com.example.app", PROGRAM_NAME);
        CLI_LOG("  %s query-latest-used-time --bundle com.example.app --user 100", PROGRAM_NAME);
        return 0;
    }

    std::string bundleName = ParseArg(argc, argv, "bundle");
    if (bundleName.empty()) {
        return OutputError("ERR_BUNDLENAME_EMPTY", "Missing bundleName parameter",
            "Please use --bundle <bundle_name> to provide the bundle name parameter, "
            "e.g., --bundle com.example.app");
    }

    int32_t userId = ParseArgInt(argc, argv, "user", DEFAULT_USER_ID);
    int64_t latestUsedTime = 0;
    ErrCode ret = BundleActiveClient::GetInstance().QueryBundleTodayLatestUsedTime(
        latestUsedTime, bundleName, userId);
    if (ret != ERR_OK) {
    return OutputError("ERR_QUERY_FAILED", "Failed to query application latest usage time today",
        "Please check: 1. Whether BundleActiveService is running normally; "
        "2. Whether ohos.permission.cli.BUNDLE_ACTIVE_INFO permission is granted");
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
    CLI_LOG("  --help                 Show help information");
    CLI_LOG("");
    CLI_LOG("SubCommands:");
    CLI_LOG("  check-bundle-idle          Check if application is idle");
    CLI_LOG("  check-bundle-period        Check if application is in usage period (Native Token only)");
    CLI_LOG("  query-stats-interval       Query usage statistics by time interval");
    CLI_LOG("  query-events               Query application event records");
    CLI_LOG("  query-app-group            Query application priority group");
    CLI_LOG("  query-high-freq-bundle     Query high-frequency usage applications");
    CLI_LOG("  query-module-records       Query module usage records");
    CLI_LOG("  query-notification-stats   Query notification event statistics");
    CLI_LOG("  query-high-freq-period     Query high-frequency usage periods");
    CLI_LOG("  query-latest-used-time     Query application latest usage time today");
    CLI_LOG("");
    CLI_LOG("Examples:");
    CLI_LOG("  %s --help", PROGRAM_NAME);
    CLI_LOG("  %s check-bundle-idle --help", PROGRAM_NAME);
    CLI_LOG("  %s check-bundle-idle --bundle com.example.app", PROGRAM_NAME);
    CLI_LOG("");
    CLI_LOG("Note: All commands require ohos.permission.cli.BUNDLE_ACTIVE_INFO permission");
}

int CmdHelp(int argc, char** argv)
{
    std::string targetCmd;
    int startIdx = ARG_START_INDEX;
    if (argc > startIdx && argv != nullptr && argv[startIdx] != nullptr &&
        strcmp(argv[startIdx], "help") == 0) {
        startIdx = HELP_CMD_ARG_START_INDEX;
    }
    for (int i = startIdx; i < argc && argv != nullptr; i++) {
        if (argv[i] != nullptr && argv[i][0] != '-') {
            targetCmd = argv[i];
            break;
        }
    }

    if (targetCmd.empty()) {
        ShowMainHelp();
        return OUTPUT_SUCCESS_CODE;
    }

    auto it = g_commands.find(targetCmd);
    if (it == g_commands.end()) {
        return OutputError("ERR_INVALID_COMMAND", std::string("Unknown command: ") + targetCmd,
            "Please run " + std::string(PROGRAM_NAME) + " --help to see available commands");
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
        "check-bundle-idle", "Check if application is idle", nullptr, nullptr, nullptr, CmdCheckBundleIdle
    };
    g_commands["check-bundle-period"] = {
        "check-bundle-period", "Check if application is in usage period (Native Token only)",
        nullptr, nullptr, nullptr, CmdCheckBundlePeriod
    };
    g_commands["query-stats-interval"] = {
        "query-stats-interval", "Query usage statistics by time interval",
        nullptr, nullptr, nullptr, CmdQueryStatsInterval
    };
    g_commands["query-events"] = {
        "query-events", "Query application event records", nullptr, nullptr, nullptr, CmdQueryEvents
    };
    g_commands["query-app-group"] = {
        "query-app-group", "Query application priority group",
        nullptr, nullptr, nullptr, CmdQueryAppGroup
    };
    g_commands["query-high-freq-bundle"] = {
        "query-high-freq-bundle", "Query high-frequency usage applications",
        nullptr, nullptr, nullptr, CmdQueryHighFreqBundle
    };
    g_commands["query-module-records"] = {
        "query-module-records", "Query module usage records",
        nullptr, nullptr, nullptr, CmdQueryModuleRecords
    };
    g_commands["query-notification-stats"] = {
        "query-notification-stats", "Query notification event statistics",
        nullptr, nullptr, nullptr, CmdQueryNotificationStats
    };
    g_commands["query-high-freq-period"] = {
        "query-high-freq-period", "Query high-frequency usage periods",
        nullptr, nullptr, nullptr, CmdQueryHighFreqPeriod
    };
    g_commands["query-latest-used-time"] = {
        "query-latest-used-time", "Query application latest usage time today",
        nullptr, nullptr, nullptr, CmdQueryLatestUsedTime
    };
    g_commands["help"] = {
        "help", "Show help information", nullptr, nullptr, nullptr, CmdHelp
    };
}

void PrintUsage(const char* prog)
{
    CLI_ERROR("Usage: %s <command> [options]", prog);
    CLI_ERROR("Run %s --help for more information", prog);
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
        return OUTPUT_ERROR_CODE;
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
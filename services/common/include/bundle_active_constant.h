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

#ifndef BUNDLE_ACTIVE_CONSTANT_H
#define BUNDLE_ACTIVE_CONSTANT_H

#include <string>

namespace OHOS {
namespace DeviceUsageStats {
const int32_t MAX_FILES_EVERY_INTERVAL_TYPE[] = {30, 30, 12, 10};
const int32_t DURATION_TABLE_ROW_NUMBER = 1;
const int32_t TABLE_ROW_ZERO = 0;
const int32_t SORTED_TABLE_ARRAY_NUMBER = 4;
const int32_t NO_UPDATE_ROW = 0;
const int32_t NO_DELETE_ROW = 0;
const int32_t EVENT_TABLE_NUMBER = 1;
const int32_t APP_GROUP_TABLE_NUMBER = 2;
const int32_t TABLE_NOT_EXIST = 0;
const int32_t DAILY_DATABASE_INDEX = 0;
const int32_t WEEKLY_DATABASE_INDEX = 1;
const int32_t MONTHLY_DATABASE_INDEX = 2;
const int32_t YEARLY_DATABASE_INDEX = 3;
const int32_t EVENT_DATABASE_INDEX = 4;
const int32_t APP_GROUP_DATABASE_INDEX = 5;
const int32_t BUNDLE_ACTIVE_CURRENT_VERSION = 1;
const int32_t BUNDLE_ACTIVE_SUCCESS = 0;
const int32_t BUNDLE_ACTIVE_FAIL = -1;
const int32_t BUNDLE_ACTIVE_RDB_VERSION = 1;
const int32_t BUNDLE_ACTIVE_DB_INDEX_NORMAL = 0;
const int32_t BUNDLE_ACTIVE_DB_INDEX_MODULE = 1;
const int32_t BUNDLE_ACTIVE_DB_INDEX_FORM = 2;
const int32_t USER_ID_COLUMN_INDEX = 0;
const int32_t BUNDLE_NAME_COLUMN_INDEX = 1;
const int32_t BUNDLE_STARTED_COUNT_COLUMN_INDEX = 2;
const int32_t LAST_TIME_COLUMN_INDEX = 3;
const int32_t LAST_TIME_CONTINUOUS_TASK_COLUMN_INDEX = 4;
const int32_t TOTAL_TIME_COLUMN_INDEX = 5;
const int32_t TOTAL_TIME_CONTINUOUS_TASK_COLUMN_INDEX = 6;
const int32_t EVENT_ID_COLUMN_INDEX = 2;
const int32_t TIME_STAMP_COLUMN_INDEX = 3;
const int32_t ABILITY_ID_COLUMN_INDEX = 4;
const int32_t LAST_BOOT_FROM_USED_TIME_COLUMN_INDEX = 2;
const int32_t LAST_SCREEN_USED_TIME_COLUMN_INDEX = 3;
const int32_t CURRENT_GROUP_COLUMN_INDEX = 4;
const int32_t REASON_IN_GROUP_COLUMN_INDEX = 5;
const int32_t BUNDLE_ALIVE_TIMEOUT_TIME_COLUMN_INDEX = 6;
const int32_t BUNDLE_DAILY_TIMEOUT_TIME_COLUMN_INDEX = 7;
const int32_t BOOT_BASED_DURATION_COLUMN_INDEX = 0;
const int32_t SCREEN_ON_DURATION_COLUMN_INDEX = 1;
const int32_t DURATION_FLAG_COLUMN_INDEX = 2;
const int32_t MODULE_NAME_COLUMN_INDEX = 2;
const int32_t MODULE_USED_COUNT_COLUMN_INDEX = 3;
const int32_t MODULE_LAST_TIME_COLUMN_INDEX = 4;
const int32_t FORM_NAME_COLUMN_INDEX = 3;
const int32_t FORM_DIMENSION_COLUMN_INDEX = 4;
const int32_t FORM_ID_COLUMN_INDEX = 5;
const int32_t FORM_COUNT_COLUMN_INDEX = 6;
const int32_t FORM_LAST_TIME_COLUMN_INDEX = 7;
const int32_t QUERY_CONDITION_VALID = 0;
const int32_t QUERY_CONDITION_INVALID = -1;
const int64_t TWO_SECONDS = 2 * 1000LL;
const int64_t THIRTY_MINUTE = 30 * 60 * 1000LL;
const int64_t SIX_DAY_IN_MILLIS_MAX_DEBUG = 6 * 1 * 10 * 60 * 1000LL;
const int64_t SIX_DAY_IN_MILLIS_MAX = 6 * 24 * 60 * 60 * 1000LL;
const int64_t ONE_DAY_TIME_DEBUG = 1 * 10 * 60 * 1000LL;
const int64_t ONE_DAY_TIME = 1 * 24 * 60 * 60 * 1000LL;
const int64_t ONE_WEEK_TIME_DEBUG = 1 * 20 * 60 * 1000LL;
const int64_t ONE_WEEK_TIME = 7 * 24 * 60 * 60 * 1000LL;
const int64_t ONE_MONTH_TIME_DEBUG = 1 * 30 * 60 * 1000LL;
const int64_t ONE_MONTH_TIME = 30 * 24 * 60 * 60 * 1000LL;
const int64_t ONE_YEAR_TIME_DEBUG = 1 * 40 * 60 * 1000LL;
const int64_t ONE_YEAR_TIME = 365 * 24 * 60 * 60 * 1000LL;
const int64_t LAST_TIME_IN_MILLIS_MIN = 0LL;
const int64_t EVENT_TIME_IN_MILLIS_MIN = 0LL;
const int64_t EVENT_BEGIN_TIME_INITIAL_VALUE = -1LL;
const std::string PACKAGE_LOG_TABLE = "PackageLog";
const std::string PACKAGE_LOG_TABLE_INDEX_PREFIX = "PackageLogIndex";
const std::string EVENT_LOG_TABLE = "DeviceEventLog";
const std::string EVENT_LOG_TABLE_INDEX_PREFIX = "DeviceEventLogIndex";
const std::string DURATION_LOG_TABLE = "DurationLog";
const std::string BUNDLE_HISTORY_LOG_TABLE = "BundleHistoryLog";
const std::string BUNDLE_HISTORY_LOG_TABLE_INDEX_PREFIX = "BundleHistoryLogIndex";
const std::string MODULE_RECORD_LOG_TABLE = "ModuleRecordLog";
const std::string MODULE_RECORD_LOG_TABLE_INDEX_PREFIX = "ModuleRecordLogIndex";
const std::string FORM_RECORD_LOG_TABLE = "FormRecordLog";
const std::string FORM_RECORD_LOG_TABLE_INDEX_PREFIX = "FormRecordLogIndex";
const std::string UNKNOWN_TABLE_NAME = "unknownTableName";
const std::string BUNDLE_ACTIVE_DB_USER_ID = "userId";
const std::string BUNDLE_ACTIVE_DB_BUNDLE_NAME = "bundleName";
const std::string BUNDLE_ACTIVE_DB_MODULE_NAME = "moduleName";
const std::string BUNDLE_ACTIVE_DB_FORM_NAME = "formName";
const std::string BUNDLE_ACTIVE_DB_FORM_DIMENSION = "formDimension";
const std::string BUNDLE_ACTIVE_DB_FORM_ID = "formId";
const std::string BUNDLE_ACTIVE_DB_MODULE_LAUNCHED_COUNT = "launchedCount";
const std::string BUNDLE_ACTIVE_DB_FORM_TOUCH_COUNT = "touchCount";
const std::string BUNDLE_ACTIVE_DB_BUNDLE_STARTED_COUNT = "bundleStartedCount";
const std::string BUNDLE_ACTIVE_DB_LAST_TIME = "lastTime";
const std::string BUNDLE_ACTIVE_DB_LAST_TIME_CONTINUOUS_TASK = "lastTimeContinuousTask";
const std::string BUNDLE_ACTIVE_DB_TOTAL_TIME = "totalTime";
const std::string BUNDLE_ACTIVE_DB_TOTAL_TIME_CONTINUOUS_TASK = "totalTimeContinuousTask";
const std::string BUNDLE_ACTIVE_DB_EVENT_ID = "eventId";
const std::string BUNDLE_ACTIVE_DB_TIME_STAMP = "timeStamp";
const std::string BUNDLE_ACTIVE_DB_ABILITY_ID = "abilityId";
const std::string BUNDLE_ACTIVE_DB_LAST_BOOT_FROM_USED_TIME = "lastBootFromUsedTime";
const std::string BUNDLE_ACTIVE_DB_LAST_SCREEN_USED_TIME = "lastScreenUsedTime";
const std::string BUNDLE_ACTIVE_DB_CURRENT_GROUP = "currentGroup";
const std::string BUNDLE_ACTIVE_DB_REASON_IN_GROUP = "reasonInGroup";
const std::string BUNDLE_ACTIVE_DB_BUNDLE_ALIVE_TIMEOUT_TIME = "bundleAliveTimeoutTime";
const std::string BUNDLE_ACTIVE_DB_BUNDLE_DAILY_TIMEOUT_TIME = "bundleDailyTimeoutTime";
const std::string BUNDLE_ACTIVE_DB_BOOT_BASED_DURATION = "bootBasedDuration";
const std::string BUNDLE_ACTIVE_DB_SCREEN_ON_DURATION = "screenOnDuration";
const std::string REFRESH_DATABASE_RUNNER_NAME = "RefreshDatabase";
const std::string BUNDLE_ACTIVE_DATABASE_DIR = "/data/service/el1/public/bundle_usage/";
const std::string BUNDLE_ACTIVE_VERSION_FILE = "/version";
const std::string DATABASE_FILE_TABLE_NAME = "table";
const std::string SQLITE_MASTER_NAME = "name";
const std::string COMMON_EVENT_UNLOCK_SCREEN = "common.event.UNLOCK_SCREEN";
const std::string COMMON_EVENT_LOCK_SCREEN = "common.event.LOCK_SCREEN";
const std::string DATABASE_TYPE[] = {"daily", "weekly", "monthly", "yearly", "event", "usageGroup"};
const std::string SUFFIX_TYPE[] = {".db"};
const std::string OPERATION_SYSTEM_LOCK = "SYSTEM_LOCK";
const std::string OPERATION_SYSTEM_UNLOCK = "SYSTEM_UNLOCK";
const std::string OPERATION_SYSTEM_SLEEP = "SYSTEM_SLEEP";
const std::string OPERATION_SYSTEM_WAKEUP = "SYSTEM_WAKEUP";
}  // namespace DeviceUsageStats
}  // namespace OHOS
#endif  // BUNDLE_ACTIVE_CONSTANT_H


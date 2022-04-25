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

#include "bundle_active_shell_command.h"

#include <iostream>
#include <getopt.h>

#include "iservice_registry.h"
#include "singleton.h"

#include "bundle_active_client.h"

namespace OHOS {
namespace DeviceUsageStats {
namespace {
    static constexpr int32_t MIN_BUNDLE_ACTIVE_DUMP_PARAMS_NUM = 4;

    static const struct option OPTIONS[] = {
        {"help", no_argument, nullptr, 'h'},
        {"all", no_argument, nullptr, 'A'},
    };

    static const std::string HELP_MSG = "usage: deviceusagestats <command> [<options>]\n"
                                    "These are common commands list:\n"
                                    "  help                         list available commands\n"
                                    "  dump                         dump the info of bundleactive\n";

    static const std::string DUMP_HELP_MSG =
        "usage: bundleactive dump [<options>]\n"
        "options list:\n"
        "  -h                                                             help menu\n"
        "  -A                                                                                    \n"
        "      Events [beginTime] [endTime] [userId]                      get events for one user\n"
        "      PackageUsage [intervalType] [beginTime] [endTime] [userId] get package usage for one user\n"
        "      ModuleUsage [maxNum] [userId]                              get module usage for one user\n";
} // namespace

BundleActiveShellCommand::BundleActiveShellCommand(int32_t argc, char *argv[]) : ShellCommand(argc,
    argv, "deviceusagestats")
{}

int32_t BundleActiveShellCommand::CreateCommandMap()
{
    commandMap_ = {
        {"help", std::bind(&BundleActiveShellCommand::RunAsHelpCommand, this)},
        {"dump", std::bind(&BundleActiveShellCommand::RunAsDumpCommand, this)},
    };
    return 0;
}

int32_t BundleActiveShellCommand::CreateMessageMap()
{
    messageMap_ = {};
    return 0;
}

int32_t BundleActiveShellCommand::init()
{
    return 0;
}

int32_t BundleActiveShellCommand::RunAsHelpCommand()
{
    resultReceiver_.append(HELP_MSG);
    return 0;
}

int32_t BundleActiveShellCommand::RunAsDumpCommand()
{
    int32_t ind = 0;
    int32_t option = getopt_long(argc_, argv_, "hA", OPTIONS, &ind);
    int32_t ret = 0;
    std::vector<std::string> infos;
    switch (option) {
        case 'h':
            resultReceiver_.append(DUMP_HELP_MSG);
            break;
        case 'A':
            if (argc_ < MIN_BUNDLE_ACTIVE_DUMP_PARAMS_NUM) {
                resultReceiver_.append("Please input correct params.\n");
                resultReceiver_.append(DUMP_HELP_MSG);
                return -1;
            }
            ret = BundleActiveClient::GetInstance().ShellDump(argList_, infos);
            break;
        default:
            resultReceiver_.append("Please input correct params.\n");
            resultReceiver_.append(DUMP_HELP_MSG);
            ret = -1;
            break;
    }
    for (auto info : infos) {
        resultReceiver_.append(info);
    }
    return ret;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


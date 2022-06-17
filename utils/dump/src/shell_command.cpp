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

#include "bundle_active_log.h"
#include "shell_command.h"

namespace OHOS {
namespace DeviceUsageStats {
ShellCommand::ShellCommand(int32_t argc, char *argv[], std::string name)
{
    argc_ = argc;
    argv_ = argv;
    name_ = name;

    if (argc < MIN_ARGUMENT_NUMBER) {
        cmd_ = "help";
        return;
    }
    cmd_ = argv[1];
    for (int32_t i = 2; i < argc; i++) {
        argList_.push_back(argv[i]);
    }
    for (const auto& arg : argList_) {
        BUNDLE_ACTIVE_LOGI("arg is %{public}s", arg.c_str());
    }
}

int32_t ShellCommand::OnCommand()
{
    int32_t result = 0;

    auto respond = commandMap_[cmd_];
    if (respond == nullptr) {
        resultReceiver_.append(GetCommandErrorMsg());
        respond = commandMap_["help"];
    }

    if (init() == 0) {
        respond();
    } else {
        result = -1;
    }

    return result;
}

std::string ShellCommand::ExecCommand()
{
    int32_t result = CreateCommandMap();
    if (result != 0) {
        BUNDLE_ACTIVE_LOGE("failed to create command map.");
    }

    result = CreateMessageMap();
    if (result != 0) {
        BUNDLE_ACTIVE_LOGE("failed to create message map.");
    }

    result = OnCommand();
    if (result != 0) {
        BUNDLE_ACTIVE_LOGE("failed to execute your command.");
        resultReceiver_ = "error: failed to execute your command.\n";
    }

    return resultReceiver_;
}

std::string ShellCommand::GetCommandErrorMsg() const
{
    std::string commandErrorMsg =
        name_ + ": '" + cmd_ + "' is not a valid " + name_ + " command. See '" + name_ + " help'.\n";
    return commandErrorMsg;
}
}  // namespace DeviceUsageStats
}  // namespace OHOS


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

#ifndef BUNDLE_ACTIVE_SHELL_COMMAND_H
#define BUNDLE_ACTIVE_SHELL_COMMAND_H

#include "shell_command.h"
#include "bundle_active_client.h"

namespace OHOS {
namespace DeviceUsageStats {
class BundleActiveShellCommand : public ShellCommand {
public:
    BundleActiveShellCommand(int32_t argc, char *argv[]);

private:
    int32_t CreateCommandMap() override;
    int32_t CreateMessageMap() override;
    int32_t init() override;
    int32_t RunAsHelpCommand();
    int32_t RunAsDumpCommand();
};
} // namespace DeviceUsageStats
} // namespace OHOS
#endif // BUNDLE_ACTIVE_SHELL_COMMAND_H


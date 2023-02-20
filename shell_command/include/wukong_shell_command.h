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

#ifndef TEST_WUKONG_SHELL_COMMAND_H
#define TEST_WUKONG_SHELL_COMMAND_H

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include "bundle_mgr_interface.h"
#include "shell_command.h"
#include "wukong_define.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
/**
 * @brief WuKong command parser.
 */
class WuKongShellCommand : public OHOS::AAFwk::ShellCommand {
public:
    WuKongShellCommand(int argc, char *argv[]);
    ~WuKongShellCommand() = default;
    ErrCode init() override;

    /**
     * @brief Create a Command Map object
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode CreateCommandMap() override;

    /**
     * @brief Create a Message Map that "error: wukong internal error."
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode CreateMessageMap() override;

    /**
     * @brief Get the Wu Kong Version.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetWuKongVersion();

    /**
     * @brief run stop command.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RunStopCommand();

    /**
     * @brief run the test to the command.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RunTestCommand();

    /**
     * @brief get WUKONG_HELP_MSG.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode RunAsHelpCommand();

    /**
     * @brief  get all launch app and ability name.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ShowAllAppInfo();

    /**
     * @brief Get command arguments
     * @return command arguments.
     */
    char **GetArgv();

    /**
     * @brief Get the command argument count.
     * @return argument count.
     */
    int GetArgc();

    /**
     * @brief Set command result information.
     * @param receiver command result.
     */
    void ResultReceiverAppend(const std::string receiver);

private:
};
}  // namespace WuKong
}  // namespace OHOS

#endif  // TEST_WUKONG_SHELL_COMMAND_H

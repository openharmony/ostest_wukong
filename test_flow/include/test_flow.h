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

#ifndef TEST_WUKONG_TEST_FLOW_H
#define TEST_WUKONG_TEST_FLOW_H

#include <string>
#include <unistd.h>

#include "app_manager.h"
#include "input_action.h"
#include "semaphore_ex.h"
#include "timer.h"
#include "wukong_define.h"
#include "wukong_shell_command.h"
#include "wukong_util.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
using namespace std;
/**
 * @brief TestFlow base clasee, provided test flow for the CommandRun parse
 * command and run test.
 */
class TestFlow {
public:
    TestFlow(WuKongShellCommand &shellcommand);
    virtual ~TestFlow() {};

    /**
     * @brief Check the command arguments, and set the command result receiver.
     * @return Return ERR_OK to run next test step.
     */
    ErrCode CheckVaildityCmd();

    /**
     * @brief Run test flow
     * @return Return ERR_OK to run next test step.
     */
    ErrCode Run();

    /**
     * @brief Run test flow
     * @param code stop code.
     */
    void Stop(ErrCode code);

protected:
    /**
     * @brief Get child class cammand formant for the getopt to parse command
     * arguments.
     * @param shortOpts Output command short arguments.
     * @param opts Output command options.
     * @return Return ERR_OK to continue.
     */
    virtual ErrCode GetOptionArguments(std::string &shortOpts, const struct option *opts) = 0;

    /**
     * @brief parse command unknown option, and print invaild information.
     * arguments.
     * @param optopt option.
     * @return Return ERR_OK to continue.
     */
    virtual ErrCode HandleUnknownOption(const char optopt) = 0;

    /**
     * @brief Check an option value from the getopt.
     * @param option Option val.
     * @return Return ERR_OK to continue.
     */
    virtual ErrCode HandleNormalOption(const int option) = 0;

    /**
     * @brief Input action initialization
     * @return Return ERR_OK to run next test step.
     */
    virtual ErrCode EnvInit() = 0;

    /**
     * @brief Run the test step
     * @return Return ERR_OK, the test flow finished.
     */
    virtual ErrCode RunStep() = 0;

    // Wukong shell command.
    WuKongShellCommand &shellcommand_;

    // stop test flow
    bool isFinished_;

    // default test time is 1 second.
    const int oneSecond_ = 1000;

private:
    NamedSemaphore semStop_;
};
}  // namespace WuKong
}  // namespace OHOS

#endif  // TEST_WUKONG_TEST_FLOW_H

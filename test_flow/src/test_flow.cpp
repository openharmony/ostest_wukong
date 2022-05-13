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

#include "test_flow.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
TestFlow::TestFlow(WuKongShellCommand &shellcommand)
    : shellcommand_(shellcommand), isFinished_(false), semStop_(SEMPHORE_STOP_NAME, 1)
{
}

ErrCode TestFlow::CheckVaildityCmd()
{
    OHOS::ErrCode result = OHOS::ERR_OK;

    // get command option and argumnets from child class.
    std::string shortOpts = "";
    struct option *longOpts = nullptr;
    result = GetOptionArguments(shortOpts, longOpts);
    if (OHOS::ERR_OK != result) {
        return result;
    }

    // get shell command argumnents from shellcommand.
    int argc_ = shellcommand_.GetArgc();
    char **argv_ = shellcommand_.GetArgv();
    int counter = 0;
    while (true) {
        int option = -1;
        counter++;
        option = getopt_long(argc_, argv_, shortOpts.c_str(), longOpts, nullptr);
        // process error
        if (optind < 0 || optind > argc_) {
            return OHOS::ERR_INVALID_VALUE;
        }
        // process error
        if (option == -1) {
            if (counter == 1 && strcmp(argv_[optind], argv_[1]) == 0) {
                ERROR_LOG(" you must specify an option at least.");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        // process error
        if (option == '?') {
            result = HandleUnknownOption(optopt);
            break;
        }
        // process correct
        result = HandleNormalOption(option);
        if (result != OHOS::ERR_OK) {
            break;
        }
    }
    if (result == OHOS::ERR_OK) {
        AppManager::GetInstance()->SetAbilityController();
        result = EnvInit();
        if (result == OHOS::ERR_OK) {
            WuKongUtil::GetInstance()->GetAllAppInfo();
        } else {
            ERROR_LOG("event init failed");
        }
    }
    return result;
}

ErrCode TestFlow::Run()
{
    TRACK_LOG_STD();
    OHOS::ErrCode result = OHOS::ERR_OK;
    int count = 0;
    // init report
    result = OHOS::WuKong::Report::GetInstance().Init();
    if (result != OHOS::ERR_OK) {
        ERROR_LOG("report init error");
        return result;
    }

    // Open the stop semaphore, check stop.
    bool res = semStop_.Open();
    if (!res) {
        ERROR_LOG("Open stop semaphore failed.");
        return OHOS::ERR_INVALID_VALUE;
    }
    // if the semaphore is 1, wait it.
    if (semStop_.GetValue() == 1) {
        semStop_.Wait();
    }

    // run test step, check test status, and control test step.
    while (!isFinished_) {
        result = RunStep();
        DEBUG_LOG_STR("Step: (%d) Result: (%d)", ++count, result);
        if (semStop_.GetValue() == 1) {
            TEST_RUN_LOG("Finished: (Stop)");
            isFinished_ = true;
        }
    }

    // recover stop semaphore.
    if (semStop_.GetValue() == 0) {
        semStop_.Post();
    }
    semStop_.Close();

    // save report
    result = OHOS::WuKong::Report::GetInstance().Finish();
    if (result != OHOS::ERR_OK) {
        ERROR_LOG("report finish error");
    }
    TRACK_LOG_END();
    return result;
}

void TestFlow::Stop(OHOS::ErrCode code)
{
    TRACK_LOG_STD();
    DEBUG_LOG_STR("Stop Result: (%d)", code);
    isFinished_ = true;
    TRACK_LOG_END();
}
}  // namespace WuKong
}  // namespace OHOS

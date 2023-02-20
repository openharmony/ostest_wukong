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

#include "component_manager.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
bool g_isPermissionBundle = false;
}
TestFlow::TestFlow(WuKongShellCommand &shellcommand)
    : shellcommand_(shellcommand),
      isFinished_(false),
      semStop_(SEMPHORE_STOP_NAME, 1)
{
}

ErrCode TestFlow::CheckVaildityCmd()
{
    OHOS::ErrCode result = OHOS::ERR_OK;

    // get command option and arguments from child class.
    std::string shortOpts = "";
    auto longOpts = GetOptionArguments(shortOpts);
    if (longOpts == nullptr) {
        return OHOS::ERR_INVALID_VALUE;
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
        if (result != OHOS::ERR_OK) {
            return result;
        }
        result = WuKongUtil::GetInstance()->GetAllAppInfo();
    }
    return result;
}

ErrCode TestFlow::Run()
{
    TRACK_LOG_STD();
    OHOS::ErrCode result = OHOS::ERR_OK;
    int count = 0;
    // init report

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
        if (g_isPermissionBundle == true) {
            result = ComponentManager::GetInstance()->PermoissionInput();
            if (result == OHOS::ERR_OK) {
                g_isPermissionBundle = false;
            }
            DEBUG_LOG_STR("PermoissionInput Result: (%d)", result);
        } else {
            result = RunStep();
            if (!isFinished_) {
                DEBUG_LOG_STR("Step: (%d) Result: (%d)", ++count, result);
            }
        }
        if (semStop_.GetValue() == 1) {
            TEST_RUN_LOG("Finished: (Stop)");
            isFinished_ = true;
        }
    }

    TEST_RUN_LOG("all test Finished");
    // recover stop semaphore.
    if (semStop_.GetValue() == 0) {
        semStop_.Post();
    }
    semStop_.Close();

    // save report
    OHOS::WuKong::Report::GetInstance()->Finish();
    TRACK_LOG_END();
    return result;
}

void TestFlow::Stop(OHOS::ErrCode code)
{
    isFinished_ = true;
}
void TestFlow::OnStatusUpdated(ComponentStatus status)
{
    DEBUG_LOG_STR("Component Status: (%d)", status);
}

void TestFlow::OnScreenUpdated()
{
    TRACK_LOG_STD();
    TRACK_LOG_END();
}

void TestFlow::OnPermissionScreenShown()
{
    TRACK_LOG_STD();
    TRACK_LOG_END();
    g_isPermissionBundle = true;
}
}  // namespace WuKong
}  // namespace OHOS

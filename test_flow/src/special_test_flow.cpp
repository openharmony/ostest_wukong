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

#include "special_test_flow.h"

#include <string>

#include "report.h"
#include "string_ex.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string SPECIAL_TEST_HELP_MSG =
    "usage: wukong special [<arguments>]\n"
    "These are wukong special arguments list:\n"
    "   -h, --help                 special test help\n"
    "   -t, --touch[x,y]           touch event \n"
    "   -c, --count                total count of test\n"
    "   -i, --interval             interval\n"
    "   -S, --swap[option]         swap event\n"
    "                              option is -s| -e| -b\n"
    "                              -s, --start: the start point of swap\n"
    "                              -e, --end: the end point of swap\n"
    "                              -b, --bilateral: swap go and back\n"
    "   -k, --spec_insomnia        power on/off event\n"
    "   -T, --time                 total time of test\n"
    "   -C, --component            component event\n"
    "   -p, --screenshot           get screenshot(only in componment input)\n"
    "   -r, --record               record user operation\n"
    "   -R, --replay               replay user operation\n";

const std::string SHORT_OPTIONS = "c:hi:T:t:kSbs:e:C:pr:R:";
const struct option LONG_OPTIONS[] = {
    {"count", required_argument, nullptr, 'c'},      // test count
    {"help", no_argument, nullptr, 'h'},             // help information
    {"interval", required_argument, nullptr, 'i'},   // test interval
    {"touch", required_argument, nullptr, 't'},      // touch
    {"spec_insomnia", no_argument, nullptr, 'k'},    // sleep and awake
    {"time", required_argument, nullptr, 'T'},       // test time
    {"swap", required_argument, nullptr, 'S'},       // swap
    {"bilateral", no_argument, nullptr, 'b'},        // swap go and back
    {"start", no_argument, nullptr, 's'},            // the start point of swap
    {"end", no_argument, nullptr, 'e'},              // the end point of swap
    {"component", required_argument, nullptr, 'C'},  // the end point of swap
    {"screenshot", no_argument, nullptr, 'p'},       // get photo of screenshot
    {"record", required_argument, nullptr, 'r'},     // record user operation
    {"replay", required_argument, nullptr, 'R'}      // replay user operation
};
const int ONE_MINUTE = 60000;
bool g_commandSWAPENABLE = false;
bool g_commandHELPENABLE = false;
bool g_commandTIMEENABLE = false;
bool g_commandTOUCHENABLE = false;
bool g_commandPOWERENABLE = false;
bool g_commandGOBACKENABLE = false;
bool g_commandCOUNTENABLE = false;
bool g_commandCOMPONENTENABLE = false;
bool g_commandSCREENSHOTENABLE = false;
bool g_commandRECORDABLE = false;
bool g_commandREPLAYABLE = false;

const int NUMBER_TWO = 2;
}  // namespace
using namespace std;

SpecialTestFlow::SpecialTestFlow(WuKongShellCommand &shellcommand) : TestFlow(shellcommand)
{
}

SpecialTestFlow::~SpecialTestFlow()
{
    if (timer_ != nullptr) {
        timer_->Shutdown();
        timer_->Unregister(timerId_);
        timer_ = nullptr;
    }
}

ErrCode SpecialTestFlow::EnvInit()
{
    ErrCode result = OHOS::ERR_OK;
    const std::string paramError = "param is incorrect";
    if (g_commandSWAPENABLE == true) {
        if (swapStartPoint_.size() == NUMBER_TWO && swapEndPoint_.size() == NUMBER_TWO) {
            // set the params of touch special test
            std::shared_ptr<SwapParam> swapParam = std::make_shared<SwapParam>();
            swapParam->startX_ = stoi(swapStartPoint_[0]);
            swapParam->startY_ = stoi(swapStartPoint_[1]);
            swapParam->endX_ = stoi(swapEndPoint_[0]);
            swapParam->endY_ = stoi(swapEndPoint_[1]);
            swapParam->isGoBack_ = g_commandGOBACKENABLE;
            if (specialTestObject_ == nullptr) {
                specialTestObject_ = swapParam;
            }
        } else {
            DEBUG_LOG(paramError.c_str());
            shellcommand_.ResultReceiverAppend(paramError + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    } else if (g_commandTOUCHENABLE == true) {
        if (touchParam_.size() == NUMBER_TWO) {
            // set the params of swap special test
            std::shared_ptr<TouchParam> touchParam = std::make_shared<TouchParam>();
            touchParam->x_ = stoi(touchParam_[0]);
            touchParam->y_ = stoi(touchParam_[1]);
            if (specialTestObject_ == nullptr) {
                specialTestObject_ = touchParam;
            }
        } else {
            DEBUG_LOG(paramError.c_str());
            shellcommand_.ResultReceiverAppend(paramError + "\n");
            result = OHOS::ERR_INVALID_VALUE;
        }
    } else if (g_commandCOMPONENTENABLE == true) {
        std::shared_ptr<ComponentParam> componentParam = std::make_shared<ComponentParam>();
        for (auto name : bundleName_) {
            componentParam->PushBundleName(name);
        }
        componentParam->isAllFinished_ = false;
        specialTestObject_ = componentParam;
    } else if (g_commandRECORDABLE == true) {
        std::shared_ptr<RecordParam> recordParam = std::make_shared<RecordParam>();
        recordParam->recordName_ = specialRecordName_;
        recordParam->recordStatus_ = true;
        if (specialTestObject_ == nullptr) {
            specialTestObject_ = recordParam;
        }
    } else if (g_commandREPLAYABLE == true) {
        std::shared_ptr<RecordParam> replayParam = std::make_shared<RecordParam>();
        replayParam->recordName_ = specialRecordName_;
        replayParam->recordStatus_ = false;
        if (specialTestObject_ == nullptr) {
            specialTestObject_ = replayParam;
        }
    }

    // if time test flow, register timer.
    if (g_commandTIMEENABLE) {
        RegisterTimer();
    }
    return result;
}

ErrCode SpecialTestFlow::RunStep()
{
    // control the count test flow
    if (g_commandCOUNTENABLE == true) {
        totalCount_--;
        if (totalCount_ < 0) {
            isFinished_ = true;
            return OHOS::ERR_OK;
        }
    }
    // order test
    ErrCode result;
    if (g_commandSCREENSHOTENABLE) {
        std::string screenStorePath;
        result = WuKongUtil::GetInstance()->WukongScreenCap(screenStorePath);
        if (result == OHOS::ERR_OK) {
            Report::GetInstance()->RecordScreenPath(screenStorePath);
        }
    }
    InputType inputTypeId = DistrbuteInputType();
    std::shared_ptr<InputAction> inputaction = InputFactory::GetInputAction(inputTypeId);
    result = inputaction->OrderInput(specialTestObject_);
    if (result != OHOS::ERR_OK) {
        WARN_LOG("This test failed");
    }
    if (g_commandCOMPONENTENABLE) {
        if (specialTestObject_->isAllFinished_) {
            isFinished_ = true;
        }
    }
    if (g_commandRECORDABLE) {
        isFinished_ = true;
    }
    if (g_commandREPLAYABLE) {
        isFinished_ = true;
    }
    usleep(intervalArgs_ * oneSecond_);
    return result;
}

InputType SpecialTestFlow::DistrbuteInputType()
{
    InputType iputType = INPUTTYPE_INVALIDINPUT;

    if (g_commandTOUCHENABLE == true) {
        iputType = INPUTTYPE_TOUCHINPUT;
    } else if (g_commandSWAPENABLE) {
        iputType = INPUTTYPE_SWAPINPUT;
    } else if (g_commandPOWERENABLE) {
        iputType = INPUTTYPE_HARDKEYINPUT;
    } else if (g_commandCOMPONENTENABLE) {
        iputType = INPUTTYPE_ELEMENTINPUT;
    } else if (g_commandRECORDABLE) {
        iputType = INPUTTYPE_RECORDINPUT;
    } else if (g_commandREPLAYABLE) {
        iputType = INPUTTYPE_REPPLAYINPUT;
    }
    return iputType;
}

const struct option *SpecialTestFlow::GetOptionArguments(std::string &shortOpts)
{
    shortOpts = SHORT_OPTIONS;
    return LONG_OPTIONS;
}

ErrCode SpecialTestFlow::HandleUnknownOption(const char optopt)
{
    ErrCode result = OHOS::ERR_OK;
    switch (optopt) {
        case 'T':
        case 'i':
        case 'c':
        case 's':
        case 'e':
        case 'C':
            shellcommand_.ResultReceiverAppend("error: option '-");
            shellcommand_.ResultReceiverAppend(string(1, optopt));
            shellcommand_.ResultReceiverAppend("' requires a value.\n");
            result = OHOS::ERR_INVALID_VALUE;
            break;
        case 'h': {
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
        default: {
            // 'wukong special' with a unknown option: wukong special -x
            shellcommand_.ResultReceiverAppend(
                "'wukong special' with an unknown option, please reference help information:\n");
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
    }
    shellcommand_.ResultReceiverAppend(SPECIAL_TEST_HELP_MSG);
    return result;
}

ErrCode SpecialTestFlow::HandleNormalOption(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    switch (option) {
        case 'S': {
            g_commandSWAPENABLE = true;
            break;
        }
        case 'k': {
            g_commandPOWERENABLE = true;
            break;
        }
        case 'c':
        case 'T':
            CheckArgument(option);
            break;
        case 'h': {
            shellcommand_.ResultReceiverAppend(SPECIAL_TEST_HELP_MSG);
            result = ERR_NO_INIT;
            g_commandHELPENABLE = true;
            break;
        }
        case 'i': {
            intervalArgs_ = std::stoi(optarg);
            TEST_RUN_LOG(("Interval: " + std::to_string(intervalArgs_)).c_str());
            break;
        }
        case 't': {
            SplitStr(optarg, ",", touchParam_);
            // check if param is valid
            result = CheckPosition(touchParam_);
            g_commandTOUCHENABLE = true;
            break;
        }
        case 'b': {
            g_commandGOBACKENABLE = true;
            break;
        }
        case 's': {
            SplitStr(optarg, ",", swapStartPoint_);
            // check if param is valid
            result = CheckPosition(swapStartPoint_);
            break;
        }
        case 'e': {
            SplitStr(optarg, ",", swapEndPoint_);
            // check if param is valid
            result = CheckPosition(swapEndPoint_);
            break;
        }
        case 'C': {
            SplitStr(optarg, ",", bundleName_);
            result = WuKongUtil::GetInstance()->CheckArgumentList(bundleName_);
            g_commandCOMPONENTENABLE = true;
            break;
        }
        case 'p': {
            g_commandSCREENSHOTENABLE = true;
            break;
        }
        case 'r': {
            g_commandRECORDABLE = true;
            specialRecordName_ = optarg;
            break;
        }
        case 'R': {
            g_commandREPLAYABLE = true;
            specialRecordName_ = optarg;
            break;
        }
    }
    WuKongUtil::GetInstance()->SetOrderFlag(true);
    return result;
}

ErrCode SpecialTestFlow::CheckArgument(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    switch (option) {
        case 'c': {
            // check if the '-c' and 'T' is exist at the same time
            if (g_commandTIMEENABLE == false) {
                g_commandCOUNTENABLE = true;
                countArgs_ = std::stoi(optarg);
                TEST_RUN_LOG(("Count: " + std::to_string(countArgs_)).c_str());
                totalCount_ = countArgs_;
            } else {
                DEBUG_LOG(PARAM_COUNT_TIME_ERROR);
                shellcommand_.ResultReceiverAppend(std::string(PARAM_COUNT_TIME_ERROR) + "\n");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        case 'T': {
            // check if the '-c' and 'T' is exist at the same time
            if (g_commandCOUNTENABLE == false) {
                totalTime_ = std::stof(optarg);
                TEST_RUN_LOG(("Time: " + std::to_string(totalTime_)).c_str());
                g_commandTIMEENABLE = true;
            } else {
                DEBUG_LOG(PARAM_TIME_COUNT_ERROR);
                shellcommand_.ResultReceiverAppend(std::string(PARAM_TIME_COUNT_ERROR) + "\n");
                result = OHOS::ERR_INVALID_VALUE;
            }
            break;
        }
        default: {
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
    }
    return result;
}

void SpecialTestFlow::RegisterTimer()
{
    if (timer_ == nullptr) {
        timer_ = std::make_shared<Utils::Timer>("wukong");
        timerId_ = timer_->Register(std::bind(&SpecialTestFlow::TestTimeout, this), totalTime_ * ONE_MINUTE, true);
        timer_->Setup();
    }
}

void SpecialTestFlow::TestTimeout()
{
    g_commandTIMEENABLE = false;
    isFinished_ = true;
}

ErrCode SpecialTestFlow::CheckPosition(std::vector<std::string> argumentlist)
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;

    // get the size of screen
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    if (argumentlist.size() > 0) {
        if (stoi(argumentlist[0]) > screenWidth || stoi(argumentlist[1]) > screenHeight || stoi(argumentlist[0]) < 0 ||
            stoi(argumentlist[1]) < 0) {
            std::string paramError = "the param of position is incorrect";
            DEBUG_LOG(paramError.c_str());
            shellcommand_.ResultReceiverAppend(paramError + "\n");
            result = OHOS::ERR_NO_INIT;
        }
    }
    return result;
}

ErrCode SpecialTestFlow::LauncherApp()
{
    std::shared_ptr<InputAction> inputaction = InputFactory::GetInputAction(INPUTTYPE_APPSWITCHINPUT);
    ErrCode result = inputaction->OrderInput(specialTestObject_);
    if (result != OHOS::ERR_OK) {
        ERROR_LOG("launcher app failed");
    }
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

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
    "   -S, --swap[option]         swap event percent\n"
    "                              option is -s| -e| -b\n"
    "                              -s, --start: the start point of swap\n"
    "                              -e, --end: the end point of swap\n"
    "                              -b, --bilateral: swap go and back\n"
    "   -k, --spec_insomnia        power on/off event\n"
    "   -T, --time                 total time of test\n";
const std::string SHORT_OPTIONS = "c:hi:T:t:kSbs:e:";
const struct option LONG_OPTIONS[] = {
    {"count", required_argument, nullptr, 'c'},     // test count
    {"help", no_argument, nullptr, 'h'},            // help information
    {"interval", required_argument, nullptr, 'i'},  // test interval
    {"touch", required_argument, nullptr, 't'},     // touch
    {"spec_insomnia", no_argument, nullptr, 'k'},   // sleep and awake
    {"time", required_argument, nullptr, 'T'},      // test time
    {"swap", required_argument, nullptr, 'S'},      // swap
    {"bilateral", no_argument, nullptr, 'b'},       // swap go and back
    {"statrt", no_argument, nullptr, 's'},          // the start point of swap
    {"end", no_argument, nullptr, 'e'},             // the end point of swap
};
const int oneMintue = 60000;
bool g_commandSWAPENABLE = false;
bool g_commandHELPENABLE = false;
bool g_commandTIMEENABLE = false;
bool g_commandTOUCHENABLE = false;
bool g_commandPOWERENABLE = false;
bool g_commandGOBACKENABLE = false;
bool g_commandCOUNTENABLE = false;

static unsigned int NUMBER_TWO = 2;
}  // namespace
using namespace std;

SpecialTestFlow::SpecialTestFlow(WuKongShellCommand &shellcommand) : TestFlow(shellcommand) {}

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
    }

    // if time test flow, registe timer.
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
    ErrCode result = OHOS::ERR_OK;
    InputType inputTypeId = DistrbuteInputType();
    std::shared_ptr<InputAction> inputaction = InputFactory::GetInputAction(inputTypeId);
    result = inputaction->OrderInput(specialTestObject_);
    if (result != OHOS::ERR_OK) {
        WARN_LOG("This test failed");
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
    }
    return iputType;
}

ErrCode SpecialTestFlow::GetOptionArguments(std::string &shortOpts, const struct option *opts)
{
    shortOpts = SHORT_OPTIONS;
    opts = LONG_OPTIONS;
    return OHOS::ERR_OK;
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
            shellcommand_.ResultReceiverAppend("error: option '");
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
            DEBUG_LOG_STR("Interval: (%ld)", intervalArgs_);
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
            result = CheckPosition(swapStartPoint_);
            break;
        }
        case 'e': {
            SplitStr(optarg, ",", swapEndPoint_);
            result = CheckPosition(swapEndPoint_);
            break;
        }
    }
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
                DEBUG_LOG_STR("Count: (%d)", countArgs_);
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
                DEBUG_LOG_STR("Time: (%ld)", totalTime_);
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
        timerId_ = timer_->Register(std::bind(&SpecialTestFlow::TestTimeout, this), totalTime_ * oneMintue, true);
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
    ErrCode result = OHOS::ERR_OK;
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    std::string paramError = "the param of position is incorrect";

    // get the size of screen
    WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (argumentlist.size() > 0) {
        if (stoi(argumentlist[0]) > screenWidth || stoi(argumentlist[1]) > screenHeight || stoi(argumentlist[0]) < 0 ||
            stoi(argumentlist[1]) < 0) {
            DEBUG_LOG(paramError.c_str());
            shellcommand_.ResultReceiverAppend(paramError + "\n");
            result = OHOS::ERR_NO_INIT;
        }
    }
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

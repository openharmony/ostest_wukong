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

#include "random_test_flow.h"

#include <string>

#include "input_factory.h"
#include "report.h"
#include "wukong_define.h"
#include "ability_manager_client.h"
#include "component_manager.h"
#include "accessibility_ui_test_ability.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string RANDOM_TEST_HELP_MSG =
    "usage: wukong exec [<arguments>]\n"
    "These are wukong exec arguments list:\n"
    "   -h, --help                 random test help\n"
    "   -a, --appswitch            appswitch event percent\n"
    "   -b, --bundle               the bundle name of allowlist\n"
    "   -p, --prohibit             the bundle name of blocklist\n"
    "   -t, --touch                touch event percent\n"
    "   -c, --count                test count\n"
    "   -i, --interval             interval\n"
    "   -s, --seed                 random seed\n"
    "   -m, --mouse                mouse event percent\n"
    "   -k, --keyboard             keyboard event percent\n"
    "   -H, --hardkey              hardkey event percent\n"
    "   -S, --swap                 swap event percent\n"
    "   -T, --time                 test time\n"
    "   -C, --component            component event percent\n";

const std::string SHORT_OPTIONS = "a:b:c:hi:k:p:s:t:T:H:m:S:C:";
const struct option LONG_OPTIONS[] = {
    {"help", no_argument, nullptr, 'h'},             // help
    {"seed", required_argument, nullptr, 's'},       // test seed
    {"time", required_argument, nullptr, 'T'},       // test time
    {"count", required_argument, nullptr, 'c'},      // test count
    {"interval", required_argument, nullptr, 'i'},   // test interval
    {"bundle", required_argument, nullptr, 'b'},     // test haps
    {"appswitch", required_argument, nullptr, 'a'},  // switch app percent
    {"keyboard", required_argument, nullptr, 'k'},   // keyboard percent
    {"mouse", required_argument, nullptr, 'm'},      // mouse percent
    {"touch", required_argument, nullptr, 't'},      // touch percent
    {"swap", required_argument, nullptr, 'S'},       // swap percent
    {"hardkey", required_argument, nullptr, 'H'},    // hardkey percent
    {"prohibit", required_argument, nullptr, 'p'},   // prohibit
    {"component", required_argument, nullptr, 'C'},  // prohibit
};

/**
 * WuKong default input action percent.
 */
const vector<int> DEFAULT_INPUT_PERCENT = {
    10,  // INPUTTYPE_TOUCHINPUT,      input touch event
    3,   // INPUTTYPE_SWAPINPUT,       input swap event
    1,   // INPUTTYPE_MOUSEINPUT,      input mouse event
    2,   // INPUTTYPE_KEYBOARDINPUT,   input keyboard event
    70,  // INPUTTYPE_ELEMENTINPUT,    input element event
    10,  // INPUTTYPE_APPSWITCHINPUT,  input appswitch event
    4    // INPUTTYPE_HARDKEYINPUT,    input hardkey event
};

const map<int, InputType> OPTION_INPUT_PERCENT = {
    {'a', INPUTTYPE_APPSWITCHINPUT},  // input appswitch event
    {'C', INPUTTYPE_ELEMENTINPUT},    // input element event
    {'k', INPUTTYPE_KEYBOARDINPUT},   // input keyboard event
    {'S', INPUTTYPE_SWAPINPUT},       // input swap event
    {'m', INPUTTYPE_MOUSEINPUT},      // input mouse event
    {'t', INPUTTYPE_TOUCHINPUT},      // input touch event
    {'H', INPUTTYPE_HARDKEYINPUT}     // input hardkey event
};

const int ONE_HUNDRED_PERCENT = 100;
// one minute (ms)
const int ONE_MINUTE = 60000;
bool g_commandSEEDENABLE = false;
bool g_commandHELPENABLE = false;
bool g_commandTIMEENABLE = false;
bool g_commandCOUNTENABLE = false;
bool g_isAppStarted = false;
}  // namespace
using namespace std;

RandomTestFlow::RandomTestFlow(WuKongShellCommand &shellcommand)
    : TestFlow(shellcommand),
      inputPercent_(INPUTTYPE_INVALIDINPUT, 0)
{
}

RandomTestFlow::~RandomTestFlow()
{
    if (timer_ != nullptr) {
        timer_->Shutdown();
        timer_->Unregister(timerId_);
        timer_ = nullptr;
    }
}

ErrCode RandomTestFlow::InitEventPercent()
{
    int sumPercent = 0;
    int sumLastDefaultPercent = ONE_HUNDRED_PERCENT;
    vector<int> lastDefaultPercent = DEFAULT_INPUT_PERCENT;
    for (auto input : inputPercent_) {
        TRACK_LOG_STR("input: (%02d)", input);
    }
    for (int type = 0; type < INPUTTYPE_INVALIDINPUT; type++) {
        // add type to count input list for random algorithm.
        for (int index = 0; index < inputPercent_[type]; index++) {
            eventList_.push_back(type);
        }
        // check argument percent, and set last default percent.
        if (inputPercent_[type] > 0) {
            sumLastDefaultPercent -= lastDefaultPercent[type];
            lastDefaultPercent[type] = 0;
        }
        sumPercent += inputPercent_[type];
    }
    TRACK_LOG_STR("sumPercent: %d", sumPercent);
    // check the sum percent more than 100%, and exit wukong.
    if (sumPercent > ONE_HUNDRED_PERCENT) {
        shellcommand_.ResultReceiverAppend("all event percentage more than 1, please reset params.\n");
        shellcommand_.ResultReceiverAppend(RANDOM_TEST_HELP_MSG);
        return OHOS::ERR_INVALID_VALUE;
    }

    // sum the last default percent for calculate last percent.
    int lastPercent = ONE_HUNDRED_PERCENT - sumPercent;
    int lastInputPercent = 0;
    for (int type = 0; type < INPUTTYPE_INVALIDINPUT; type++) {
        if (lastDefaultPercent[type] <= 0 || lastDefaultPercent[type] > ONE_HUNDRED_PERCENT ||
            sumLastDefaultPercent <= 0) {
            continue;
        }
        lastInputPercent = (int)(lastPercent * ((float)(lastDefaultPercent[type]) / sumLastDefaultPercent));
        // add type to count input list for random algorithm.
        for (int index = 0; index < lastInputPercent; index++) {
            eventList_.push_back(type);
        }
        sumPercent += lastInputPercent;
    }

    // if the sumPercent less than 100%, add INPUTTYPE_TOUCHINPUT to random algorithm.
    for (int index = 0; index < ONE_HUNDRED_PERCENT - sumPercent; index++) {
        eventList_.push_back(INPUTTYPE_TOUCHINPUT);
    }

    return OHOS::ERR_OK;
}

ErrCode RandomTestFlow::EnvInit()
{
    // init event list percent.
    ErrCode result = InitEventPercent();
    if (result != OHOS::ERR_OK) {
        return result;
    }

    // init srand and print seed information.
    if (g_commandSEEDENABLE) {
        srand(seedArgs_);
    } else {
        time_t tempSeed = time(nullptr);
        srand((unsigned int)tempSeed);
        seedArgs_ = (int)time(nullptr);
    }
    Report::GetInstance()->SetSeed(std::to_string(seedArgs_));
    TEST_RUN_LOG(("Seed: " + std::to_string(seedArgs_)).c_str());

    // shuffle the event list.
    RandomShuffle();

    // if time test flow, register timer.
    if (g_commandTIMEENABLE) {
        RegisterTimer();
    }
    return result;
}

ErrCode RandomTestFlow::SetInputPercent(const int option)
{
    InputType inputType = INPUTTYPE_INVALIDINPUT;
    auto it = OPTION_INPUT_PERCENT.find(option);
    if (it == OPTION_INPUT_PERCENT.end()) {
        return OHOS::ERR_INVALID_VALUE;
    }
    inputType = it->second;

    float percent = 0.0;
    try {
        percent = std::stof(optarg);
    } catch (const std::exception &e) {
        // try the option argument string convert float.
        shellcommand_.ResultReceiverAppend("error: option '");
        shellcommand_.ResultReceiverAppend(string((char *)(&option)));
        shellcommand_.ResultReceiverAppend("' requires a value.\n");
        shellcommand_.ResultReceiverAppend(RANDOM_TEST_HELP_MSG);
        return OHOS::ERR_INVALID_VALUE;
    }

    // check valid of the option argument
    if (percent > 1 || percent < 0) {
        shellcommand_.ResultReceiverAppend("the input percent more than 1 (100%).\n");
        shellcommand_.ResultReceiverAppend(RANDOM_TEST_HELP_MSG);
        return OHOS::ERR_INVALID_VALUE;
    }

    // convert float to int (0 ~ 100)
    inputPercent_[inputType] = (int)(percent * ONE_HUNDRED_PERCENT);
    return OHOS::ERR_OK;
}

ErrCode RandomTestFlow::InputScene(std::shared_ptr<InputAction> inputaction, bool inputFlag)
{
    ErrCode result = OHOS::ERR_OK;
    if (inputFlag) {
        result = inputaction->RandomInput();
    } else {
        ComponentManager::GetInstance()->BackToPrePage();
    }
    return result;
}

bool RandomTestFlow::SetBlockPage()
{
    auto root = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
    auto accPtr = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance();
    // Get root AccessibilityElementInfo from Accessibility
    accPtr->GetRoot(*(root.get()));
    std::string path = root->GetPagePath();
    bool inputFlag = true;
    TRACK_LOG_STR("Componentpage path: (%s)", path.c_str());
    char const *systemPath = "pages/system";
    if (strstr(path.c_str(), systemPath) != NULL) {
        inputFlag = false;
    }
    return inputFlag;
}

ErrCode RandomTestFlow::RunStep()
{
    ErrCode result;
    // control the count test flow
    if (g_commandCOUNTENABLE == true) {
        totalCount_--;
        if (totalCount_ < 0) {
            isFinished_ = true;
            return OHOS::ERR_OK;
        }
    }
    bool inputFlag = SetBlockPage();
    std::shared_ptr<InputAction> inputaction = nullptr;
    if (!g_isAppStarted) {
        inputaction = InputFactory::GetInputAction(INPUTTYPE_APPSWITCHINPUT);
        if (inputaction == nullptr) {
            ERROR_LOG("inputaction is nullptr");
            return OHOS::ERR_INVALID_VALUE;
        }
        result = InputScene(inputaction, inputFlag);
        if (result != OHOS::ERR_OK) {
            ERROR_LOG("launch app failed and exit");
            return result;
        }
        inputaction = nullptr;
        g_isAppStarted = true;
        usleep(intervalArgs_ * oneSecond_);
    }
    // input event, get event index form event list by random algorithm.
    int eventindex = rand() % ONE_HUNDRED_PERCENT;
    InputType eventTypeId = (InputType)(eventList_.at(eventindex));
    inputaction = InputFactory::GetInputAction(eventTypeId);
    if (inputaction == nullptr) {
        ERROR_LOG("inputaction is nullptr");
        return OHOS::ERR_INVALID_VALUE;
    }
    std::vector<std::string> allowList;
    WuKongUtil::GetInstance()->GetAllowList(allowList);
    if (allowList.size() > 0) {
        std::string bundleName = "com.ohos.launcher";
        auto elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();
        if (elementName.GetBundleName() == bundleName) {
            if (eventTypeId == INPUTTYPE_TOUCHINPUT || eventTypeId == INPUTTYPE_ELEMENTINPUT) {
                return OHOS::ERR_INVALID_VALUE;
            }
        }
    }
    result = InputScene(inputaction, inputFlag);
    usleep(intervalArgs_ * oneSecond_);
    return result;
}

ErrCode RandomTestFlow::HandleNormalOption(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    switch (option) {
        case 't':
        case 'm':
        case 'S':
        case 'k':
        case 'H':
        case 'a':
        case 'C': {
            result = SetInputPercent(option);
            break;
        }
        case 'b': {
            result = WuKongUtil::GetInstance()->SetAllowList(optarg);
            break;
        }
        case 'c': {
            // check if the '-c' and 'T' is exist at the same time
            result = CheckArgument(option);
            break;
        }
        case 'h': {
            shellcommand_.ResultReceiverAppend(RANDOM_TEST_HELP_MSG);
            result = OHOS::ERR_NO_INIT;
            g_commandHELPENABLE = true;
            break;
        }
        case 'i': {
            intervalArgs_ = std::stoi(optarg);
            TEST_RUN_LOG(("Interval: " + std::to_string(intervalArgs_)).c_str());
            break;
        }
        case 's': {
            seedArgs_ = std::stoi(optarg);
            g_commandSEEDENABLE = true;
            break;
        }
        case 'T': {
            // check if the '-c' and 'T' is exist at the same time
            result = CheckArgument(option);
            break;
        }
        case 'p': {
            result = WuKongUtil::GetInstance()->SetBlockList(optarg);
            break;
        }
        default: {
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
    }
    WuKongUtil::GetInstance()->SetOrderFlag(false);
    return result;
}

ErrCode RandomTestFlow::CheckArgument(const int option)
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

const struct option *RandomTestFlow::GetOptionArguments(std::string &shortOpts)
{
    shortOpts = SHORT_OPTIONS;
    return LONG_OPTIONS;
}

ErrCode RandomTestFlow::HandleUnknownOption(const char optopt)
{
    ErrCode result = OHOS::ERR_OK;
    switch (optopt) {
        case 'a':
        case 'b':
        case 'c':
        case 'i':
        case 's':
        case 't':
        case 'S':
        case 'p':
        case 'k':
        case 'H':
        case 'T':
        case 'm':
        case 'C':
            // error: option 'x' requires a value.
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
            // 'wukong exec' with an unknown option: wukong exec -x
            shellcommand_.ResultReceiverAppend(
                "'wukong exec' with an unknown option, please reference help information:\n");
            result = OHOS::ERR_INVALID_VALUE;
            break;
        }
    }
    shellcommand_.ResultReceiverAppend(RANDOM_TEST_HELP_MSG);
    return result;
}

void RandomTestFlow::RandomShuffle()
{
    for (uint32_t i = eventList_.size() - 1; i > 0; --i) {
        std::swap(eventList_[i], eventList_[std::rand() % (i + 1)]);
    }
}

void RandomTestFlow::RegisterTimer()
{
    if (timer_ == nullptr) {
        timer_ = std::make_shared<Utils::Timer>("wukong");
        timerId_ = timer_->Register(std::bind(&RandomTestFlow::TestTimeout, this), totalTime_ * ONE_MINUTE, true);
        timer_->Setup();
    }
}

void RandomTestFlow::TestTimeout()
{
    g_commandTIMEENABLE = false;
    isFinished_ = true;
}
}  // namespace WuKong
}  // namespace OHOS

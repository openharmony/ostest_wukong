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
#include <regex>

#include "report.h"
#include "wukong_define.h"
#include "wukong_util.h"
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
    "   -d, --page                 block page list\n"
    "   -t, --touch                touch event percent\n"
    "   -c, --count                test count\n"
    "   -i, --interval             interval\n"
    "   -s, --seed                 random seed\n"
    "   -m, --mouse                mouse event percent\n"
    "   -k, --keyboard             keyboard event percent\n"
    "   -H, --hardkey              hardkey event percent\n"
    "   -S, --swap                 swap event percent\n"
    "   -T, --time                 test time\n"
    "   -C, --component            component event percent\n"
    "   -r, --rotate               rotate event percent\n"
    "   -e, --allow ability        the ability name of allowlist\n"
    "   -E, --block ability        the ability name of blocklist\n"
    "   -Y, --blockCompId          the id list of block component\n"
    "   -y, --blockCompType        the type list of block component\n"
    "   -I, --screenshot           get screenshot(only in random input)\n"
    "   -B, --checkBWScreen        black and white screen detection\n"
    "   -U, --Uri                  set Uri pages\n"
    "   -x, --Uri-type             set Uri-type \n"
    "   -K, --knuckle              set percent of knuckle event \n"
    "   -f, --finger               set percent of knuckle event using single or double fingers, eg: -f 1,0.5,2,0.5 \n"
    "   -P, --pinch                set percent of pinch event \n"
    "   -D, --direction            set percent of swipe event to each direction, eg: -D u,0.25,l,0.25,d,0.25,r,0.25 \n"
    "   -o, --pause                pause swiping for a while \n"
    "   -w, --crown                set percent of watch crown event \n"
    "   -g, --gestures             set percent of watch gestures event \n"
    "   -l, --idle                 set percent of watch idle event \n"
    "   -j, --keypress             set percent of watch key press event \n"
    "   -F, --float                set percent of float and split event \n"
    "   -q, --collapse             set percent of collapse event for foldable products \n"
    "   -W, --browser              set percent of browser operation event \n";

const std::string SHORT_OPTIONS = "a:b:c:d:e:E:hIBoi:k:p:s:t:T:H:m:S:C:r:Y:y:U:x:K:P:f:D:w:g:l:j:F:q:W:";
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
    {"rotate", required_argument, nullptr, 'r'},     // rotate percent
    {"page", required_argument, nullptr, 'd'},       // block page
    {"allow ability", required_argument, nullptr, 'e'},
    {"block ability", required_argument, nullptr, 'E'},
    {"blockCompId", required_argument, nullptr, 'Y'},
    {"blockCompType", required_argument, nullptr, 'y'},
    {"screenshot", no_argument, nullptr, 'I'},
    {"checkBWScreen", no_argument, nullptr, 'B'},
    {"Uri", required_argument, nullptr, 'U'},
    {"Uri-type", required_argument, nullptr, 'x'},
    {"knuckle", required_argument, nullptr, 'K'},
    {"finger", required_argument, nullptr, 'f'},
    {"pinch", required_argument, nullptr, 'P'},
    {"direction", required_argument, nullptr, 'D'},
    {"pause", no_argument, nullptr, 'o'},
    {"crown", required_argument, nullptr, 'w'},
    {"gestures", required_argument, nullptr, 'g'},
    {"idle", required_argument, nullptr, 'l'},
    {"keypress", required_argument, nullptr, 'j'},
    {"float", required_argument, nullptr, 'F'},
    {"collapse", required_argument, nullptr, 'q'},
    {"browser", required_argument, nullptr, 'W'}
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
    2,   // INPUTTYPE_HARDKEYINPUT,    input hardkey event
    2,   // INPUTTYPE_ROTATE,          input rotate event
    0,   // knuckle
    0,   // pinch
    0,   // watch crown
    0,   // watch gestures
    0,   // watch idle
    0,   // watch key press
    0,   // float
    0,   // collapse
    0    // browser
};

const map<int, InputType> OPTION_INPUT_PERCENT = {
    {'a', INPUTTYPE_APPSWITCHINPUT},  // input appswitch event
    {'C', INPUTTYPE_ELEMENTINPUT},    // input element event
    {'k', INPUTTYPE_KEYBOARDINPUT},   // input keyboard event
    {'S', INPUTTYPE_SWAPINPUT},       // input swap event
    {'m', INPUTTYPE_MOUSEINPUT},      // input mouse event
    {'t', INPUTTYPE_TOUCHINPUT},      // input touch event
    {'H', INPUTTYPE_HARDKEYINPUT},    // input hardkey event
    {'r', INPUTTYPE_ROTATEINPUT},      // input rotate event
    {'K', INPUTTYPE_KNUCKLEINPUT},
    {'P', INPUTTYPE_PINCHINPUT},
    {'w', INPUTTYPE_CROWNINPUT},
    {'g', INPUTTYPE_GESTURESINPUT},
    {'l', INPUTTYPE_IDLEINPUT},
    {'j', INPUTTYPE_KEYPRESSINPUT},
    {'F', INPUTTYPE_FLOATSPLITINPUT},
    {'q', INPUTTYPE_COLLAPSEINPUT},
    {'W', INPUTTYPE_BROWSERINPUT},
};

/**
 * @brief Wukong block page
 */
std::vector<std::string> systemPaths;

const int ONE_HUNDRED_PERCENT = 100;
// one minute (ms)
const int ONE_MINUTE = 60000;
// rotate
const int ROTATE = 114;
bool g_commandSEEDENABLE = false;
bool g_commandHELPENABLE = false;
bool g_commandTIMEENABLE = false;
bool g_commandCOUNTENABLE = false;
bool g_isAppStarted = false;
bool g_commandALLOWABILITYENABLE = false;
bool g_commandBLOCKABILITYENABLE = false;
bool g_commandALLOWBUNDLEENABLE = false;
bool g_commandSCREENSHOTENABLE = false;
bool g_commandCHECKBWSCREEN = false;
bool g_commandURIENABLE = false;
bool g_commandURITYPEENABLE = false;
bool g_commandKnuckleENABLE = false;
bool g_commandSwapENABLE = false;
bool g_commandPauseENABLE = false;

// default false
bool g_commandUITEST = false;

// 需要带百分比参数的命令参数合集
const std::string NEED_PERCENT_OPTIONS = "amkrtwgljCHKPSFqW";
// 需要后面跟配置的命令参数合集
const std::string NEED_ARG_OPTIONS = "abcdefgijklmprstwxyCDEHSTUYFqW";
const std::string ONLY_WATCH_SUPPORT_ARGS = "wglj";
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

    if (inputType == INPUTTYPE_KNUCKLEINPUT) {
        g_commandKnuckleENABLE = true;
    }

    if (inputType == INPUTTYPE_SWAPINPUT) {
        g_commandSwapENABLE = true;
    }

    float percent = 0.0;
    try {
        percent = std::stof(optarg);
        if ((it->first) == ROTATE && percent == 1) {
            g_isAppStarted = true;
        }
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

ErrCode RandomTestFlow::SetBlackWhiteSheet(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    if (option == 'b') {
        result = WuKongUtil::GetInstance()->SetAllowList(optarg);
        g_commandALLOWBUNDLEENABLE = true;
    } else if (option == 'p') {
        result = WuKongUtil::GetInstance()->SetBlockList(optarg);
    } else if (option == 'e') {
        result = WuKongUtil::GetInstance()->SetAllowAbilityList(optarg);
        if (result != OHOS::ERR_INVALID_VALUE) {
            result = CheckArgument(option);
        }
    } else if (option == 'E') {
        result = WuKongUtil::GetInstance()->SetBlockAbilityList(optarg);
        if (result != OHOS::ERR_INVALID_VALUE) {
            result = CheckArgument(option);
        }
    } else if (option == 'd') {
        result = WuKongUtil::GetInstance()->SetBlockPageList(optarg);
    } else if (option == 'Y') {
        WuKongUtil::GetInstance()->SetCompIdBlockList(optarg);
    } else if (option == 'y') {
        WuKongUtil::GetInstance()->SetCompTypeBlockList(optarg);
    } else if (option == 'U') {
        WuKongUtil::GetInstance()->SetComponentUri(optarg);
        g_commandURIENABLE = true;
    } else if (option == 'x') {
        WuKongUtil::GetInstance()->SetComponentUriType(optarg);
        g_commandURITYPEENABLE = true;
    }
    return OHOS::ERR_OK;
}

ErrCode RandomTestFlow::SetRunningParam(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    if (option == 'c' || option == 'T' || option == 'f' || option == 'D') {
        result = CheckArgument(option);
    } else if (option == 'i') {
        std::regex pattern("[0-9]+");
        if (std::regex_match(optarg, pattern)) {
            intervalArgs_ = std::stoi(optarg);
            TEST_RUN_LOG(("Interval: " + std::to_string(intervalArgs_)).c_str());
        } else {
            ERROR_LOG("Setting -i must follow an integer");
            result = OHOS::ERR_INVALID_VALUE;
        }
    } else if (option == 's') {
        seedArgs_ = std::stoi(optarg);
        g_commandSEEDENABLE = true;
    }
    return result;
}

ErrCode RandomTestFlow::SetRunningIndicator(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    if (option == 'h') {
        shellcommand_.ResultReceiverAppend(RANDOM_TEST_HELP_MSG);
        result = OHOS::ERR_NO_INIT;
        g_commandHELPENABLE = true;
    } else if (option == 'I') {
        g_commandSCREENSHOTENABLE = true;
    } else if (option == 'B') {
        g_commandSCREENSHOTENABLE = true;
        g_commandCHECKBWSCREEN = true;
    } else if (option == 'o') {
        if (!g_commandSwapENABLE) {
            ERROR_LOG("invalid param: when -o is configured, -S must be configured.");
            return OHOS::ERR_INVALID_VALUE;
        }
        g_commandPauseENABLE = true;
    }
    return result;
}

ErrCode RandomTestFlow::InputScene(std::shared_ptr<InputAction> inputaction, bool inputFlag)
{
    ErrCode result = OHOS::ERR_OK;
    if (inputFlag) {
        if (g_commandURIENABLE && inputaction->GetInputInfo() == INPUTTYPE_APPSWITCHINPUT) {
            if (g_commandURITYPEENABLE) {
                std::string uri = WuKongUtil::GetInstance()->GetComponentUri();
                std::string uriType = WuKongUtil::GetInstance()->GetComponentUriType();
                result = inputaction->RandomInput(uri, uriType);
            } else if (g_commandALLOWABILITYENABLE) {
                std::string uri = WuKongUtil::GetInstance()->GetComponentUri();
                std::vector<std::string> abilityList;
                WuKongUtil::GetInstance()->GetAllowAbilityList(abilityList);
                result = inputaction->RandomInput(uri, abilityList);
            }
        } else {
            result = inputaction->RandomInput();
        }
    } else {
        ComponentManager::GetInstance()->BackToPrePage();
    }
    return result;
}

bool RandomTestFlow::SetBlockPage(const std::vector<std::string> systemPaths)
{
    auto root = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
    auto accPtr = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance();
    // Get root AccessibilityElementInfo from Accessibility
    accPtr->GetRoot(*(root.get()));
    std::string path = root->GetPagePath();
    bool inputFlag = true;
    TRACK_LOG_STR("Componentpage path: (%s)", path.c_str());
    for (string systemPath : systemPaths) {
        if (path.find(systemPath) != string::npos) {
            INFO_LOG_STR("Block the current page and return. Block page : (%s)", path.c_str());
            inputFlag = false;
            break;
        }
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
    if (g_commandSCREENSHOTENABLE) {
        std::string screenStorePath;
        result = WuKongUtil::GetInstance()->WukongScreenCap(screenStorePath, g_commandUITEST, g_commandCHECKBWSCREEN);
        if (result == OHOS::ERR_OK) {
            Report::GetInstance()->RecordScreenPath(screenStorePath);
        }
    }
    bool inputFlag = SetBlockPage(systemPaths);
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
    if (g_commandBLOCKABILITYENABLE == true) {
        inputFlag = CheckBlockAbility();
    }
    // input event, get event index form event list by random algorithm.
    InputType eventTypeId = (InputType)(eventList_.at(rand() % ONE_HUNDRED_PERCENT));
    inputaction = InputFactory::GetInputAction(eventTypeId);
    if (inputaction == nullptr) {
        ERROR_LOG("inputaction is nullptr");
        return OHOS::ERR_INVALID_VALUE;
    }

    setActionParam(inputaction);
    if (ProtectRightAbility(inputaction, eventTypeId) == OHOS::ERR_INVALID_VALUE) {
        return OHOS::ERR_INVALID_VALUE;
    }
    result = InputScene(inputaction, inputFlag);
    usleep(intervalArgs_ * oneSecond_);
    return result;
}

void RandomTestFlow::setActionParam(std::shared_ptr<InputAction> inputaction)
{
    inputaction->arg_interval = intervalArgs_;
    if (fingerArgMap_.size() > 0) {
        inputaction->fingersMap_ = fingerArgMap_;
        inputaction->fingerVector_ = fingerVector_;
        inputaction->enableFingerArg = true;
    }

    inputaction->enablePause = g_commandPauseENABLE;
    if (directionMap_.size() > 0) {
        inputaction->directionMap_ = directionMap_;
        inputaction->directionVector_ = directionVector_;
    }
}

ErrCode RandomTestFlow::ProtectRightAbility(std::shared_ptr<InputAction> &inputaction, InputType &eventTypeId)
{
    std::vector<std::string> allowList;
    WuKongUtil::GetInstance()->GetAllowList(allowList);
    if (allowList.size() > 0 && g_commandALLOWABILITYENABLE == false) {
        std::string bundleName = "com.ohos.launcher";
        auto elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();
        if (elementName.GetBundleName() == bundleName) {
            if (eventTypeId == INPUTTYPE_TOUCHINPUT || eventTypeId == INPUTTYPE_ELEMENTINPUT) {
                return OHOS::ERR_INVALID_VALUE;
            }
        }
        // allowList 数量大于0 并且 elementName.GetBundleName() 不在allowList里面，重新拉起一个应用
        auto curBundleName = elementName.GetBundleName();
        auto it = find(allowList.begin(), allowList.end(), curBundleName);
        if (it == allowList.end()) {
            inputaction = InputFactory::GetInputAction(INPUTTYPE_APPSWITCHINPUT);
            if (inputaction == nullptr) {
                ERROR_LOG("inputaction is nullptr");
                return OHOS::ERR_INVALID_VALUE;
            }
        }
    } else if (allowList.size() > 0 && g_commandALLOWABILITYENABLE && !g_commandURIENABLE) {
        std::vector<std::string> abilityList;
        WuKongUtil::GetInstance()->GetAllowAbilityList(abilityList);
        auto elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();
        auto curBundleName = elementName.GetBundleName();
        auto curAbilityName = elementName.GetAbilityName();
        TRACK_LOG_STR("curAbilityName : %s", curAbilityName.c_str());
        auto bundleIndex = find(allowList.begin(), allowList.end(), curBundleName);
        auto abilityIndex = find(abilityList.begin(), abilityList.end(), curAbilityName);
        if (bundleIndex == allowList.end() || abilityIndex == abilityList.end()) {
            int index = rand() % abilityList.size();
            // start ability through bundle infomation
            ErrCode result = AppManager::GetInstance()->StartAbilityByBundleInfo(abilityList[index], allowList[0]);
            // print the result of start event
            if (result ==OHOS::ERR_OK) {
                INFO_LOG_STR("Ability Name : ("%s") startup successful", abilityList[index].c_str());
            } else {
                ERROR_LOG_STR("Ability Name : ("%s") startup failed", abilityList[index].c_str());
            }
            TRACK_LOG_STR("ability index : %d", index);
        }
        return OHOS::ERR_OK;
    }
    return OHOS::ERR_OK;
}

ErrCode RandomTestFlow::HandleNormalOption(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    if (NEED_PERCENT_OPTIONS.find(option) != std::string::npos) {
        if (ONLY_WATCH_SUPPORT_ARGS.find(option) != std::string::npos &&
            WuKongUtil::GetInstance()->GetDeviceType() != "wearable") {
            char c = static_cast<char>(option);
            std::string str(1, c);
            ERROR_LOG_STR("Setting %s is only supported by wearable devices.", str.c_str());
            return OHOS::ERR_INVALID_VALUE;
        }
        result = SetInputPercent(option);
    } else {
        result = SetBlackWhiteSheet(option);
        if (result != OHOS::ERR_OK) {
            return result;
        }
        result = SetRunningParam(option);
        if (result != OHOS::ERR_OK) {
            return result;
        }
        result = SetRunningIndicator(option);
    }
    WuKongUtil::GetInstance()->GetBlockPageList(systemPaths);
    WuKongUtil::GetInstance()->SetOrderFlag(false);
    return result;
}

ErrCode RandomTestFlow::CheckArgument(const int option)
{
    ErrCode result = OHOS::ERR_OK;
    switch (option) {
        case 'c': {
            result = CheckArgumentOptionOfc();
            break;
        }
        case 'T': {
            result = CheckArgumentOptionOfT();
            break;
        }
        case 'e': {
            result = CheckArgumentOptionOfe();
            break;
        }
        case 'E': {
            result = CheckArgumentOptionOfE();
            break;
        }
        case 'f': {
            result = CheckArgumentOptionOfF();
            break;
        }
        case 'D': {
            result = CheckArgumentOptionOfD();
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
    if (NEED_ARG_OPTIONS.find(optopt) != std::string::npos) {
        // error: option 'x' requires a value.
        shellcommand_.ResultReceiverAppend("error: option '-");
        shellcommand_.ResultReceiverAppend(string(1, optopt));
        shellcommand_.ResultReceiverAppend("' requires a value.\n");
        result = OHOS::ERR_INVALID_VALUE;
    } else if (optopt == 'h') {
        result = OHOS::ERR_INVALID_VALUE;
    } else {
        // 'wukong exec' with an unknown option: wukong exec -x
        shellcommand_.ResultReceiverAppend(
            "'wukong exec' with an unknown option, please reference help information:\n");
        result = OHOS::ERR_INVALID_VALUE;
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
        timerId_ = timer_->Register([this] () { RandomTestFlow::TestTimeout(); }, totalTime_ * ONE_MINUTE, true);
        timer_->Setup();
    }
}

void RandomTestFlow::TestTimeout()
{
    g_commandTIMEENABLE = false;
    isFinished_ = true;
}

ErrCode RandomTestFlow::CheckArgumentOptionOfe()
{
    if (g_commandALLOWABILITYENABLE == false) {
        g_commandALLOWABILITYENABLE = true;
        if (g_commandALLOWBUNDLEENABLE == true) {
            return OHOS::ERR_OK;
        } else {
            ERROR_LOG("invalid param : When -e is configured, -b must be configured.");
            ERROR_LOG("invalid param : please ensure that the -b is before the -e");
            return OHOS::ERR_INVALID_VALUE;
        }
    } else {
        ERROR_LOG("invalid param : please check params of '-e'.");
        return OHOS::ERR_INVALID_VALUE;
    }
}

ErrCode RandomTestFlow::CheckArgumentOptionOfE()
{
    if (g_commandBLOCKABILITYENABLE == false) {
        g_commandBLOCKABILITYENABLE = true;
        if (g_commandALLOWBUNDLEENABLE == true) {
            return OHOS::ERR_OK;
        } else {
            ERROR_LOG("invalid param : When -E is configure, -b must be configured.");
            ERROR_LOG("invalid param : Plese ensure that the -b is before the -E.");
            return OHOS::ERR_INVALID_VALUE;
        }
    } else {
        ERROR_LOG("invalid param : please check params of '-E'.");
        return OHOS::ERR_INVALID_VALUE;
    }
}

ErrCode RandomTestFlow::CheckArgumentOptionOfc()
{
    // check if the '-c' and 'T' is exist at the same time
    if (g_commandTIMEENABLE == false) {
        std::stringstream ss(optarg);
        if (ss >> countArgs_) {
            g_commandCOUNTENABLE = true;
            TEST_RUN_LOG(("Count: " + std::to_string(countArgs_)).c_str());
            totalCount_ = countArgs_;
            return OHOS::ERR_OK;
        } else {
            ERROR_LOG("Setting -c must follow an interger");
            return OHOS::ERR_INVALID_VALUE;
        }
    } else {
        DEBUG_LOG(PARAM_COUNT_TIME_ERROR);
        shellcommand_.ResultReceiverAppend(std::string(PARAM_COUNT_TIME_ERROR) + "\n");
        return OHOS::ERR_INVALID_VALUE;
    }
}

ErrCode RandomTestFlow::CheckArgumentOptionOfT()
{
    // check if the '-c' and 'T' is exist at the same time
    if (g_commandCOUNTENABLE == false) {
        std::stringstream ss(optarg);
        if (ss >> totalTime_) {
            TEST_RUN_LOG(("Time: " + std::to_string(totalTime_)).c_str());
            g_commandTIMEENABLE = true;
            return OHOS::ERR_OK;
        } else {
            ERROR_LOG("Setting -T must follow a float");
            return OHOS::ERR_INVALID_VALUE;
        }
    } else {
        DEBUG_LOG(PARAM_TIME_COUNT_ERROR);
        shellcommand_.ResultReceiverAppend(std::string(PARAM_TIME_COUNT_ERROR) + "\n");
        return OHOS::ERR_INVALID_VALUE;
    }
}

ErrCode RandomTestFlow::CheckArgumentOptionOfF()
{
    if (!g_commandKnuckleENABLE && !g_commandSwapENABLE) {
        ERROR_LOG("invalid param: when -f is configured, -K or -S must be configured.");
        return OHOS::ERR_INVALID_VALUE;
    }

    auto optargMap = WuKongUtil::GetInstance()->ConvertStringToMap<int>(optarg);
    if (optargMap.size() == 0) {
        ERROR_LOG("Convert param -f to map failed!");
        return OHOS::ERR_INVALID_VALUE;
    }

    std::vector<int> fingerVector;
    WuKongUtil::GetInstance()->ParseMapToVector(optargMap, fingerVector);
    this->fingerArgMap_ = optargMap;
    this->fingerVector_ = fingerVector;

    return OHOS::ERR_OK;
}

ErrCode RandomTestFlow::CheckArgumentOptionOfD()
{
    if (!g_commandSwapENABLE) {
        ERROR_LOG("invalid param: when -D is configured, -S must be configured.");
        return OHOS::ERR_INVALID_VALUE;
    }

    auto directionMap = WuKongUtil::GetInstance()->ConvertStringToMap<char>(optarg);
    if (directionMap.size() == 0) {
        ERROR_LOG("Convert param -D to map failed!");
        return OHOS::ERR_INVALID_VALUE;
    }

    std::vector<char> directionVector;
    WuKongUtil::GetInstance()->ParseMapToVector(directionMap, directionVector);
    this->directionMap_ = directionMap;
    this->directionVector_ = directionVector;

    return OHOS::ERR_OK;
}

bool RandomTestFlow::CheckBlockAbility()
{
    bool inputFlag = true;
    auto elementName = AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();
    auto currentAbility = elementName.GetAbilityName();
    std::vector<string> blockAbilityList;
    WuKongUtil::GetInstance()->GetBlockAbilityList(blockAbilityList);
    auto it = find(blockAbilityList.begin(), blockAbilityList.end(), currentAbility);
    if (it != blockAbilityList.end()) {
        INFO_LOG_STR("Block the current Ability and return. Block Ability : (%s)", currentAbility.c_str());
        inputFlag = false;
    }
    return inputFlag;
}
}  // namespace WuKong
}  // namespace OHOS

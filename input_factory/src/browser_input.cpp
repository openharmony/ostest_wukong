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
#include <vector>
#include <string>

#include "browser_input.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
struct KeyCombination {
    std::vector<int> keys;
    std::string description;
};

const std::vector<KeyCombination> BROWSER_OPER_KEYS = {
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_UP}, "WindowMaximized"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_DOWN}, "WindowMinimized"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_T}, "AddTab"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_W}, "CloseTab"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_N}, "AddWindow"},
    {{OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_F4}, "CloseWindow"}};
}  // namespace

BrowserInput::BrowserInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_BROWSERINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

BrowserInput::~BrowserInput()
{}

ErrCode BrowserInput::RandomInput()
{
    std::string deviceType = WuKongUtil::GetInstance()->GetDeviceType();
    if (deviceType != "2in1" && deviceType != "PC") {
        INFO_LOG("just PC support this action.");
        return OHOS::ERR_OK;
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> comb(0, BROWSER_OPER_KEYS.size() - 1);
    int index = comb(rng);
    auto &combination = BROWSER_OPER_KEYS[index];
    std::string desc = combination.description;
    INFO_LOG_STR("Start simulating browser event: %s.", desc.c_str());
    ErrCode result = MultimodeManager::GetInstance()->KeyCombinationSimulate(combination.keys);

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType BrowserInput::GetInputInfo()
{
    return INPUTTYPE_BROWSERINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

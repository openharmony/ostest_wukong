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

#include "hardkey_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
const int HARDKEY_COUNT = 2;
const int DOWN_TIME = 10;
}  // namespace
HardkeyInput::HardkeyInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_HARDKEYINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

HardkeyInput::~HardkeyInput() {}

ErrCode HardkeyInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    ErrCode result = MultimodeManager::GetInstance()->SingleKeyCodeInput(MMI::KeyEvent::KEYCODE_POWER, DOWN_TIME);
    return result;
}

ErrCode HardkeyInput::RandomInput()
{
    int keycode = MMI::KeyEvent::KEYCODE_VOLUME_UP + rand() % HARDKEY_COUNT;
    ErrCode result = MultimodeManager::GetInstance()->SingleKeyCodeInput(keycode, DOWN_TIME);
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode HardkeyInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

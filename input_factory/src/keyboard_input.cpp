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

#include "keyboard_input.h"

#include "multimode_manager.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
const int SINGLE_CODE_PER = 30;
const int ONE_HUNDRED_PERCENT = 100;
const int DOWN_TIME = 100;
}  // namespace
KeyboardInput::KeyboardInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_KEYBOARDINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}
KeyboardInput::~KeyboardInput()
{
}
ErrCode KeyboardInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<int> keycodelist;
    int keyCodePercent = rand() % ONE_HUNDRED_PERCENT;
    MultimodeManager::GetInstance()->GetKeycodeList(keycodelist);
    if (keycodelist.size() > 0) {
        if (keyCodePercent < SINGLE_CODE_PER) {
            int keycode = keycodelist[(uint32_t)(rand()) % keycodelist.size()];
            result = MultimodeManager::GetInstance()->SingleKeyCodeInput(keycode, DOWN_TIME);
        } else {
            result = MultimodeManager::GetInstance()->MultiKeyCodeInput(DOWN_TIME);
        }
    } else {
        return OHOS::ERR_NO_INIT;
    }
    if (result != OHOS::ERR_OK) {
        return result;
    }
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode KeyboardInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

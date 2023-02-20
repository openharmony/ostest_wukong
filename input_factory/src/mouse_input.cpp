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

#include "mouse_input.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
const int ONEHUNDRED = 100;
const int MOUSE_RIGHT_PERCENT = 20;
const int MOUSE_MID_PERCENT = 10;
}  // namespace
MouseInput::MouseInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_MOUSEINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

MouseInput::~MouseInput()
{
}

ErrCode MouseInput::RandomInput()
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    int mouseType;
    // get the size of screen
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    // generate random point on the screen
    int xClickPosition = rand() % screenWidth;
    int yClickPosition = rand() % screenHeight;
    // distrbute type accord to percentage
    int randomInt = rand() % ONEHUNDRED;
    if (randomInt < MOUSE_MID_PERCENT) {
        mouseType = MMI::PointerEvent::MOUSE_BUTTON_MIDDLE;
    } else if (randomInt < (MOUSE_RIGHT_PERCENT + MOUSE_MID_PERCENT)) {
        mouseType = MMI::PointerEvent::MOUSE_BUTTON_RIGHT;
    } else {
        mouseType = MMI::PointerEvent::MOUSE_BUTTON_LEFT;
    }
    INFO_LOG_STR("Mouse: (%d, %d) Mouse Type: (%s)", xClickPosition, yClickPosition,
                 MouseTypeToString(mouseType).c_str());
    auto multiinput = MultimodeManager::GetInstance();
    result =
        multiinput->PointerInput(xClickPosition, yClickPosition, mouseType, MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    result = multiinput->PointerInput(xClickPosition, yClickPosition, mouseType, MMI::PointerEvent::POINTER_ACTION_UP);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode MouseInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}

std::string MouseInput::MouseTypeToString(int mousetype)
{
    switch (mousetype) {
        case MMI::PointerEvent::MOUSE_BUTTON_MIDDLE:
            return "MOUSE_MIDDLE";
            break;
        case MMI::PointerEvent::MOUSE_BUTTON_RIGHT:
            return "MOUSE_RIGHT";
            break;
        case MMI::PointerEvent::MOUSE_BUTTON_LEFT:
            return "MOUSE_LEFT";
            break;
        default:
            return "BUTTON_NONE";
            break;
    }
}
}  // namespace WuKong
}  // namespace OHOS

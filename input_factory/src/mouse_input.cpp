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
#include <regex>

#include "mouse_input.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
enum MOUSE_BUTTON_ID {
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_MIDDLE,
    BUTTON_SIDE,
    BUTTON_EXTEND,
    BUTTON_FORWARD,
    BUTTON_BACK,
    BUTTON_TASK
};
}  // namespace

MouseInput::MouseInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_MOUSEINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

MouseInput::~MouseInput()
{}

ErrCode MouseInput::RandomInput()
{
    std::string deviceType = WuKongUtil::GetInstance()->GetDeviceType();
    if (deviceType != "2in1" && deviceType != "PC") {
        INFO_LOG("just PC support this action.");
        return OHOS::ERR_OK;
    }

    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    result = MouseAction(screenWidth, screenHeight);

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode MouseInput::MouseAction(int screenWidth, int screenHeight)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> width_dist(0, screenWidth - 1);
    std::uniform_int_distribution<int> height_dist(0, screenHeight - 1);

    int xClickPosition = width_dist(rng);
    int yClickPosition = height_dist(rng);

    // 随机鼠标按键
    std::uniform_int_distribution<int> button_dist(BUTTON_LEFT, BUTTON_TASK);
    MOUSE_BUTTON_ID selectedButton = static_cast<MOUSE_BUTTON_ID>(button_dist(rng));
    int buttonId = static_cast<int>(selectedButton);

    // 随机鼠标动作
    MOUSE_ACTION_ENUM randomMouseAction =
        WuKongUtil::GetInstance()->GetRandomEnumValue<MOUSE_ACTION_ENUM, DIRECTION_LEN>(
            {CLICK, DOUBLE_CLICK, DRAG, MOVE});
    char mouseAction = static_cast<char>(randomMouseAction);
    if (MOUSE_ACTION_STR.find(mouseAction) == std::string::npos) {
        ERROR_LOG("invalid mouse action.");
        return OHOS::ERR_INVALID_VALUE;
    }

    // 随机选择方向
    char direction = static_cast<char>(
        WuKongUtil::GetInstance()->GetRandomEnumValue<SWIPE_DIRECTION, DIRECTION_LEN>({UP, DOWN, LEFT, RIGHT}));
    if (DIRECTION.find(direction) == std::string::npos) {
        ERROR_LOG("invalid direction.");
        return OHOS::ERR_INVALID_VALUE;
    }

    // 随机滚轮方向
    std::uniform_int_distribution<int> forwardOrBack_dist(1, ONE_HALF);
    int forwardOrBack = forwardOrBack_dist(rng);
    int number = forwardOrBack == 1 ? ONE_HALF : 0 - ONE_HALF;
    int xDstPosition = (direction == 'u' || direction == 'd')
                           ? xClickPosition
                           : (direction == 'l' ? xClickPosition - ONE_HALF * DEFAULT_SWIPE_MOVE_LENGTH
                                               : xClickPosition + ONE_HALF * DEFAULT_SWIPE_MOVE_LENGTH);
    int yDstPosition = (direction == 'l' || direction == 'r')
                           ? yClickPosition
                           : (direction == 'u' ? yClickPosition - ONE_HALF * DEFAULT_SWIPE_MOVE_LENGTH
                                               : yClickPosition + ONE_HALF * DEFAULT_SWIPE_MOVE_LENGTH);
    std::string srcPosition = WuKongUtil::GetInstance()->GetPositionStr(xClickPosition, yClickPosition);
    std::string dstPosition = WuKongUtil::GetInstance()->GetPositionStr(xDstPosition, yDstPosition);

    return MultimodeManager::GetInstance()->MouseActionSimulate(
        buttonId, mouseAction, srcPosition, dstPosition, number);
}

InputType MouseInput::GetInputInfo()
{
    return INPUTTYPE_MOUSEINPUT;
}

}  // namespace WuKong
}  // namespace OHOS

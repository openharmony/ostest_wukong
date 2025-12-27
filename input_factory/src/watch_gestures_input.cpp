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

#include "watch_gestures_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
WatchGesturesInput::WatchGesturesInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_GESTURESINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

WatchGesturesInput::~WatchGesturesInput()
{}

ErrCode WatchGesturesInput::OrderInput(const std::shared_ptr<SpcialTestObject> &specialTestObject)
{
    return OHOS::ERR_OK;
}

ErrCode WatchGesturesInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    auto util = WuKongUtil::GetInstance();
    auto multiInput = MultimodeManager::GetInstance();
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    result = util->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    multiInput->PowerKeySimulate();
    int touchX = util->RangeGenerate(screenWidth * 1 / 10, screenWidth * 4 / 10);
    int touchY = util->RangeGenerate(screenHeight * 1 / 10, screenHeight * 4 / 10);

    // 向上或者向下滑动
    bool isUpOrDown = util->NextBool();
    // 向左或者向右滑动
    bool isLeftOrRight = util->NextBool();
    int dstX =
        isUpOrDown ? touchX : (isLeftOrRight ? touchX - DEFAULT_SWIPE_MOVE_LENGTH : touchX + DEFAULT_SWIPE_MOVE_LENGTH);
    int dstY =
        isLeftOrRight ? touchY : (isUpOrDown ? touchY - DEFAULT_SWIPE_MOVE_LENGTH : touchY + DEFAULT_SWIPE_MOVE_LENGTH);
    std::string position1 = util->GetPositionStr(touchX, touchY);
    std::string position2 = util->GetPositionStr(dstX, dstY);

    result = multiInput->WatchSwipeSimulate(position1, position2);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType WatchGesturesInput::GetInputInfo()
{
    return INPUTTYPE_GESTURESINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

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

#include "watch_crown_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
WatchCrownInput::WatchCrownInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_CROWNINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

WatchCrownInput::~WatchCrownInput()
{}

ErrCode WatchCrownInput::OrderInput(const std::shared_ptr<SpcialTestObject> &specialTestObject)
{
    return OHOS::ERR_OK;
}

ErrCode WatchCrownInput::RandomInput()
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

    int x = screenWidth / ONE_HALF;
    int y = screenHeight / ONE_HALF;
    int eventIndex = util->Generate(ONE_HALF);
    if (eventIndex == 0) {
        INFO_LOG("Start simulating the rotating crown.");
        // 旋转表冠
        int rotateCount = util->Generate(CHARGE_STRIDE);
        result = multiInput->WatchCrownRotateSimulate(x, y, rotateCount);
    } else {
        INFO_LOG("Start simulating watch face switching.");
        // 切换表盘
        multiInput->PowerKeySimulate();
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
        multiInput->LongTouchSimulate(x, y);
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
        bool randomFlag = util->NextBool();
        std::string position1 = util->GetPositionStr(x, y);
        std::string position2 =
            util->GetPositionStr(randomFlag ? x - DEFAULT_SWIPE_MOVE_LENGTH : x + DEFAULT_SWIPE_MOVE_LENGTH, y);
        multiInput->WatchSwipeSimulate(position1, position2, SWIPE_PAUSE_TIME);
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
        multiInput->TouchSimulate(x, y);
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
        multiInput->PowerKeySimulate();
    }

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType WatchCrownInput::GetInputInfo()
{
    return INPUTTYPE_CROWNINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

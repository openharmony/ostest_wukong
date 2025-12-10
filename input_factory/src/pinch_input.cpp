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

#include "pinch_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
PinchInput::PinchInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_PINCHINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

PinchInput::~PinchInput()
{
}

ErrCode PinchInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    return OHOS::ERR_OK;
}

ErrCode PinchInput::RandomInput()
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    int touchX1 = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 1 / 10, screenWidth * 4 / 10);
    int touchY1 = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 1 / 10, screenHeight * 4 / 10);
    int touchX2 = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 6 / 10, screenWidth * 9 / 10);
    int touchY2 = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 6 / 10, screenHeight * 9 / 10);

    // 往中间捏或者往外放大
    bool isIntoCenter = WuKongUtil::GetInstance()->NextBool();

    std::string position1 = WuKongUtil::GetInstance()->GetPositionStr(touchX1, touchY1);
    std::string position2 = WuKongUtil::GetInstance()->GetPositionStr(touchX2, touchY2);
    result = MultimodeManager::GetInstance()->PinchSimulate(position1, position2, isIntoCenter, this->arg_interval);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    INFO_LOG_STR("double fingers pinch event: (%d, %d, %d, %d), to %s",
        touchX1,
        touchY1,
        touchX2,
        touchY2,
        isIntoCenter ? "center" : "border");
    return result;
}

InputType PinchInput::GetInputInfo()
{
    return INPUTTYPE_PINCHINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

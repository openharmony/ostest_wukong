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

#include "rotate_input.h"

#include "input_manager.h"
#include "screen.h"
#include "screen_manager.h"
#include "wukong_define.h"
#include "report.h"
namespace OHOS {
namespace WuKong {
const int ONE = 1;
const int TWO = 2;
const int THREE = 3;
const int FOUR = 4;
RotateInput::RotateInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_ROTATEINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

RotateInput::~RotateInput()
{
}

ErrCode RotateInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<sptr<Rosen::Screen>> screens;
    Rosen::ScreenManager::GetInstance().GetAllScreens(screens);

    uint32_t orientation = static_cast<uint32_t>((rand() % FOUR) + ONE);
    switch (orientation) {
        case ONE:
            INFO_LOG("Rotate orientation is VERTICAL");
            break;
        case TWO:
            INFO_LOG("Rotate orientation is HORIZONTAL");
            break;
        case THREE:
            INFO_LOG("Rotate orientation is REVERSE_VERTICAL");
            break;
        case FOUR:
            INFO_LOG("Rotate orientation is REVERSE_HORIZONTAL");
            break;
        
        default:
            break;
    }
    if (screens[0] == nullptr) {
        ERROR_LOG("Failed to get home screen data");
        return OHOS::ERR_INVALID_VALUE;
    }
    screens[0]->SetOrientation(static_cast<Rosen::Orientation>(orientation));
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode RotateInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<sptr<Rosen::Screen>> screens;
    Rosen::ScreenManager::GetInstance().GetAllScreens(screens);

    uint32_t orientation = static_cast<uint32_t>((rand() % FOUR) + ONE);
    switch (orientation) {
        case ONE:
            INFO_LOG("Rotate orientation is VERTICAL");
            break;
        case TWO:
            INFO_LOG("Rotate orientation is HORIZONTAL");
            break;
        case THREE:
            INFO_LOG("Rotate orientation is REVERSE_VERTICAL");
            break;
        case FOUR:
            INFO_LOG("Rotate orientation is REVERSE_HORIZONTAL");
            break;
        
        default:
            break;
    }
    if (screens.size() == 0) {
        ERROR_LOG("Failed to get home screen data");
        return OHOS::ERR_INVALID_VALUE;
    }
    screens[0]->SetOrientation(static_cast<Rosen::Orientation>(orientation));
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType RotateInput::GetInputInfo()
{
    return INPUTTYPE_ROTATEINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

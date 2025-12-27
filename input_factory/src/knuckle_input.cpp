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
#include <map>
#include <random>

#include "knuckle_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
KnuckleInput::KnuckleInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_KNUCKLEINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

KnuckleInput::~KnuckleInput()
{
}

ErrCode KnuckleInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    return OHOS::ERR_OK;
}

ErrCode KnuckleInput::RandomInput()
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist1(1, ONE_HALF);
    std::uniform_int_distribution<> dist2(0, PERCENT_ONE_HUNDRED - 1);

    // 默认不带-f参数，随机生成单指或者多指事件
    if (!this->enableFingerArg) {
        int finger = dist1(gen);
        return KnuckleAction(finger, screenWidth, screenHeight);
    } else {
        // 带了-f参数，根据配置的比例，生成事件
        if (fingersMap_.size() == 0 || fingerVector_.size() == 0) {
            ERROR_LOG("fingersMap_ or fingerVector_ is empty.");
            return OHOS::ERR_INVALID_VALUE;
        }

        int index = dist2(gen);
        return KnuckleAction(fingerVector_[index], screenWidth, screenHeight);
    }
}

ErrCode KnuckleInput::KnuckleAction(int fingerNumber, int screenWidth, int screenHeight)
{
    switch (fingerNumber) {
        case F_ONE:
            return KnuckleInput().SingleFingerKnuckle(screenWidth, screenHeight);
            break;
        case F_TWO:
            return KnuckleInput().DoubleFingerKnuckle(screenWidth, screenHeight);
            break;
        default:
            ERROR_LOG_STR("invalid finger %d", fingerNumber);
            break;
    }

    return OHOS::ERR_OK;
}

ErrCode KnuckleInput::SingleFingerKnuckle(int screenWidth, int screenHeight)
{
    int touchX = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 1 / 10, screenWidth * 9 / 10);
    int touchY = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 1 / 10, screenHeight * 9 / 10);
    auto multiInput = MultimodeManager::GetInstance();
    ErrCode result = multiInput->KnuckleSimulate(touchX, touchY);

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode KnuckleInput::DoubleFingerKnuckle(int screenWidth, int screenHeight)
{
    int touchX1 = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 1 / 10, screenWidth * 9 / 10);
    int touchY1 = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 1 / 10, screenHeight * 9 / 10);
    int touchX2 = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 1 / 10, screenWidth * 9 / 10);
    int touchY2 = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 1 / 10, screenHeight * 9 / 10);

    auto multiInput = MultimodeManager::GetInstance();
    ErrCode result = multiInput->DoubleKnuckleSimulate(touchX1, touchY1, touchX2, touchY2);

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType KnuckleInput::GetInputInfo()
{
    return INPUTTYPE_KNUCKLEINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

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

#include "swap_input.h"

#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"
namespace OHOS {
namespace WuKong {
SwapInput::SwapInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_SWAPINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

SwapInput::~SwapInput()
{
}

ErrCode SwapInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    ErrCode result;
    static bool isBack = true;
    SwapParam* swapPtr = static_cast<SwapParam*>(specialTestObject.get());
    if (swapPtr == nullptr) {
        return OHOS::ERR_INVALID_VALUE;
    }
    int xSrcPosition = swapPtr->startX_;
    int ySrcPosition = swapPtr->startY_;
    int xDstPosition = swapPtr->endX_;
    int yDstPosition = swapPtr->endY_;
    if (swapPtr->isGoBack_) {
        isBack = !isBack;
        swapPtr->isBack_ = isBack;
    }
    if (isBack) {
        xSrcPosition = swapPtr->endX_;
        ySrcPosition = swapPtr->endY_;
        xDstPosition = swapPtr->startX_;
        yDstPosition = swapPtr->startY_;
    }
    result = MultimodeManager::GetInstance()->IntervalSwap(xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
    return result;
}

ErrCode SwapInput::RandomInput()
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    int xSrcPosition = rand() % screenWidth;
    int ySrcPosition = rand() % screenHeight;
    int xDstPosition = rand() % screenWidth;
    int yDstPosition = rand() % screenHeight;
    INFO_LOG_STR("Swap: (%d, %d) -> (%d, %d)", xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
    result = MultimodeManager::GetInstance()->IntervalSwap(xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode SwapInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

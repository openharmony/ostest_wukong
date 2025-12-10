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
#include <random>

#include "swap_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
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

ErrCode SwapInput::OrderInput(const std::shared_ptr<SpcialTestObject> &specialTestObject)
{
    ErrCode result;
    static bool isBack = true;
    SwapParam* swapPtr = static_cast<SwapParam *>(specialTestObject.get());
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

    // 不带-f参数的，默认单指滑动
    if (!this->enableFingerArg) {
        INFO_LOG("SwapInput::RandomInput(), start without param -f.");
        result = SingleFingerSwipe(screenWidth, screenHeight);
    } else {
        // 带-f,-D,-o参数的，根据配置的比例，随便选取滑动指数和方向、是否停顿
        INFO_LOG("SwapInput::RandomInput(), start with param -f.");
        srand(time(0));
        int index = rand() % PERCENT_ONE_HUNDRED + 1;
        int finger = fingerVector_[index];
        // 如果没有指定方向，则从4个方向里随机取一个
        SWIPE_DIRECTION randomDirection =
            WuKongUtil::GetInstance()->GetRandomEnumValue<SWIPE_DIRECTION, DIRECTION_LEN>({UP, DOWN, LEFT, RIGHT});
        char direction = directionVector_.size() == 0 ? static_cast<char>(randomDirection) : directionVector_[index];
        if (DIRECTION.find(direction) != std::string::npos) {
            result = SwipeAction(finger, screenWidth, screenHeight, this->enablePause, direction);
        } else {
            ERROR_LOG("invalid direction.");
            result = OHOS::ERR_INVALID_VALUE;
        }
    }

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode SwapInput::SwipeAction(int fingerNumber, int screenWidth, int screenHeight, bool enablePause, char &direction)
{
    std::string deviceType;
    ErrCode result = OHOS::ERR_OK;

    switch (fingerNumber) {
        case F_ONE:
        case F_TWO:
            result = SingleFingerSwipe(screenWidth, screenHeight);
            break;
        case F_THREE:
        case F_FOUR:
            deviceType = WuKongUtil::GetInstance()->GetDeviceType();
            if (deviceType == "phone") {
                INFO_LOG("phone does not support this operation.");
            } else {
                result = MultimodeManager::GetInstance()->SwipeForFingersSimulate(
                    screenWidth / ONE_HALF, screenHeight / ONE_HALF, fingerNumber, enablePause, direction);
            }
            break;
        default:
            ERROR_LOG_STR("current support finger range: [1-4], invalid finger: %d", fingerNumber);
            break;
    }

    return result;
}

ErrCode SwapInput::SingleFingerSwipe(int screenWidth, int screenHeight)
{
    int xSrcPosition = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 1 / 10, screenWidth * 9 / 10);
    int ySrcPosition = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 1 / 10, screenHeight * 9 / 10);
    int xDstPosition = WuKongUtil::GetInstance()->RangeGenerate(screenWidth * 1 / 10, screenWidth * 9 / 10);
    int yDstPosition = WuKongUtil::GetInstance()->RangeGenerate(screenHeight * 1 / 10, screenHeight * 9 / 10);
    INFO_LOG_STR("Swap: (%d, %d) -> (%d, %d)", xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
    return MultimodeManager::GetInstance()->IntervalSwap(xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
}

InputType SwapInput::GetInputInfo()
{
    return INPUTTYPE_SWAPINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

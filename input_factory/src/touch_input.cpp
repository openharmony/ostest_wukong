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

#include "touch_input.h"

#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"
namespace OHOS {
namespace WuKong {
TouchInput::TouchInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_TOUCHINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

TouchInput::~TouchInput()
{
}

ErrCode TouchInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    TouchParam* touchPtr = static_cast<TouchParam*>(specialTestObject.get());
    if (touchPtr == nullptr) {
        return OHOS::ERR_INVALID_VALUE;
    }
    int touchX = touchPtr->x_;
    int touchY = touchPtr->y_;
    auto multiinput = MultimodeManager::GetInstance();
    ErrCode result = multiinput->PointerInput(touchX, touchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                              MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    result = multiinput->PointerInput(touchX, touchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_UP);
    INFO_LOG_STR("Touch: (%d, %d)", touchX, touchY);
    return result;
}

ErrCode TouchInput::RandomInput()
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    int touchX = rand() % screenWidth;
    int touchY = rand() % screenHeight;
    auto multiinput = MultimodeManager::GetInstance();
    result = multiinput->PointerInput(touchX, touchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    result = multiinput->PointerInput(touchX, touchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_UP);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    INFO_LOG_STR("Touch: (%d, %d)", touchX, touchY);
    return result;
}

ErrCode TouchInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

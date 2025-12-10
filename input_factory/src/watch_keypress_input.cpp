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

#include "watch_keypress_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
WatchKeyPressInput::WatchKeyPressInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_KEYPRESSINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

WatchKeyPressInput::~WatchKeyPressInput()
{
}

ErrCode WatchKeyPressInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    return OHOS::ERR_OK;
}

ErrCode WatchKeyPressInput::RandomInput()
{
    auto util = WuKongUtil::GetInstance();
    auto multiInput = MultimodeManager::GetInstance();
    int eventIndex = util->Generate(DIRECTION_LEN);
    ErrCode result = OHOS::ERR_OK;
    switch (eventIndex) {
        case F_ONE - 1:
            // 电源键
            INFO_LOG("start press the power button.");
            result = multiInput->PowerKeySimulate();
            break;
        case F_ONE:
            // 长按Dagger键，需要2秒以上
            INFO_LOG("start press the dagger button.");
            result = multiInput->PowerKeySimulate();
            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
            result = multiInput->LongPressKeySimulate(
                MMI::KeyEvent::KEYCODE_DAGGER_CLICK, DEFAULT_DRAG_TOTAL_TIME_MS * ONE_HALF);
            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
            result = multiInput->PowerKeySimulate();
            break;
        case F_TWO:
            // 进多任务中心
            INFO_LOG("start enter the multitask center");
            result = multiInput->DoublePowerKeySimulate();
            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
            result = multiInput->PowerKeySimulate();
            break;
        case F_THREE:
            // 长按电源键，需要4秒以上
            INFO_LOG("start press the power button for long time.");
            result = multiInput->LongPressKeySimulate(
                MMI::KeyEvent::KEYCODE_POWER, DEFAULT_DRAG_TOTAL_TIME_MS * ONE_HALF * ONE_HALF);
            std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_KNUCKLE_INTERVAL_TIME_MS));
            result = multiInput->PowerKeySimulate();
            break;
        default:
            ERROR_LOG("invalid event index.");
            break;
    }

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType WatchKeyPressInput::GetInputInfo()
{
    return INPUTTYPE_KEYPRESSINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

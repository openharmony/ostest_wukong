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

#include "float_split_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
FloatAndSplitInput::FloatAndSplitInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_FLOATSPLITINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

FloatAndSplitInput::~FloatAndSplitInput()
{}

ErrCode FloatAndSplitInput::RandomInput()
{
    auto util = WuKongUtil::GetInstance();
    if (util->GetDeviceType() != "phone") {
        INFO_LOG("this event just only supported by phone.");
        return OHOS::ERR_OK;
    }

    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = util->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    int eventIndex = util->Generate(ONE_HALF);
    std::string srcPosition = util->GetPositionStr(screenWidth / 2, screenHeight - 50);
    std::string dstPosition;
    if (eventIndex == 0) {
        // 悬浮窗
        INFO_LOG("Start simulating floating window.");
        dstPosition = util->GetPositionStr(screenWidth - DEFAULT_SWIPE_MOVE_LENGTH, DEFAULT_SWIPE_MOVE_LENGTH);
    } else {
        // 应用分屏
        INFO_LOG("Start simulating app split-screen.");
        dstPosition = util->GetPositionStr(DEFAULT_SWIPE_MOVE_LENGTH, DEFAULT_SWIPE_MOVE_LENGTH);
    }

    result =
        MultimodeManager::GetInstance()->FloatOrSplitSimulate(srcPosition, dstPosition, DEFAULT_DRAG_TOTAL_TIME_MS);

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType FloatAndSplitInput::GetInputInfo()
{
    return INPUTTYPE_FLOATSPLITINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

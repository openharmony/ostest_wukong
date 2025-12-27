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

#include "watch_idle_input.h"
#include "input_manager.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
WatchIdleInput::WatchIdleInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_IDLEINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

WatchIdleInput::~WatchIdleInput()
{
}

ErrCode WatchIdleInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    return OHOS::ERR_OK;
}

ErrCode WatchIdleInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    INFO_LOG("start simulating watch idle action.");
    auto multiInput = MultimodeManager::GetInstance();
    multiInput->PowerSuspendSimulate();
    std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
    multiInput->PowerWakeupSimulate();
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType WatchIdleInput::GetInputInfo()
{
    return INPUTTYPE_IDLEINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

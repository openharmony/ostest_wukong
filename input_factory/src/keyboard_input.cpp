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

#include "keyboard_input.h"
#include "multimode_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
const int singleCodePercent = 30;
const int oneHundredPercent = 100;
const int downtime = 100;
}  // namespace
KeyboardInput::KeyboardInput() : InputAction() {}
KeyboardInput::~KeyboardInput() {}
ErrCode KeyboardInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<int> keycodelist;
    int keyCodePercent = rand() % oneHundredPercent;
    MultimodeManager::GetInstance()->GetKeycodeList(keycodelist);
    if (keycodelist.size() > 0) {
        if (keyCodePercent < singleCodePercent) {
            int keycode = keycodelist[rand() % keycodelist.size()];
            result = MultimodeManager::GetInstance()->SingleKeyCodeInput(keycode, downtime);
        } else {
            result = MultimodeManager::GetInstance()->MultiKeyCodeInput(downtime);
        }
    } else {
        return OHOS::ERR_NO_INIT;
    }
    return result;
}

ErrCode KeyboardInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

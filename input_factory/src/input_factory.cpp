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

#include "input_factory.h"

#include "appswitch_input.h"
#include "component_input.h"
#include "keyboard_input.h"
#include "swap_input.h"
#include "mouse_input.h"
#include "hardkey_input.h"
#include "touch_input.h"
#include "record_input.h"

namespace OHOS {
namespace WuKong {
std::shared_ptr<InputAction> InputFactory::GetInputAction(InputType type)
{
    std::shared_ptr<InputAction> input_action = nullptr;
    switch (type) {
        case INPUTTYPE_TOUCHINPUT: {
            input_action = std::make_shared<TouchInput>();
            break;
        }
        case INPUTTYPE_SWAPINPUT: {
            input_action = std::make_shared<SwapInput>();
            break;
        }
        case INPUTTYPE_MOUSEINPUT: {
            input_action = std::make_shared<MouseInput>();
            break;
        }
        case INPUTTYPE_KEYBOARDINPUT: {
            input_action = std::make_shared<KeyboardInput>();
            break;
        }
        case INPUTTYPE_APPSWITCHINPUT: {
            input_action = std::make_shared<AppswitchInput>();
            break;
        }
        case INPUTTYPE_ELEMENTINPUT: {
            input_action = std::make_shared<ComponentInput>();
            break;
        }
        case INPUTTYPE_HARDKEYINPUT: {
            input_action = std::make_shared<HardkeyInput>();
            break;
        }
        case INPUTTYPE_RECORDINPUT: {
            input_action = std::make_shared<RecordInput>();
            break;
        }
        case INPUTTYPE_REPPLAYINPUT: {
            input_action = std::make_shared<RecordInput>();
            break;
        }
        default: {
            break;
        }
    }
    return input_action;
}
}  // namespace WuKong
}  // namespace OHOS

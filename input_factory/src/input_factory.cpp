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
#include "rotate_input.h"
#include "knuckle_input.h"
#include "pinch_input.h"
#include "watch_crown_input.h"
#include "watch_gestures_input.h"
#include "watch_idle_input.h"
#include "watch_keypress_input.h"
#include "float_split_input.h"
#include "collapse_input.h"
#include "browser_input.h"

namespace OHOS {
namespace WuKong {

template <typename T>
std::shared_ptr<InputAction> CreateInputAction()
{
    return std::make_shared<T>();
}

std::shared_ptr<InputAction> InputFactory::GetInputAction(InputType type)
{
    static const std::unordered_map<InputType, std::function<std::shared_ptr<InputAction>()>> inputActionMap = {
        {INPUTTYPE_TOUCHINPUT, CreateInputAction<TouchInput>},
        {INPUTTYPE_SWAPINPUT, CreateInputAction<SwapInput>},
        {INPUTTYPE_MOUSEINPUT, CreateInputAction<MouseInput>},
        {INPUTTYPE_KEYBOARDINPUT, CreateInputAction<KeyboardInput>},
        {INPUTTYPE_APPSWITCHINPUT, CreateInputAction<AppswitchInput>},
        {INPUTTYPE_ELEMENTINPUT, CreateInputAction<ComponentInput>},
        {INPUTTYPE_HARDKEYINPUT, CreateInputAction<HardkeyInput>},
        {INPUTTYPE_RECORDINPUT, CreateInputAction<RecordInput>},
        {INPUTTYPE_REPPLAYINPUT, CreateInputAction<RecordInput>},
        {INPUTTYPE_ROTATEINPUT, CreateInputAction<RotateInput>},
        {INPUTTYPE_KNUCKLEINPUT, CreateInputAction<KnuckleInput>},
        {INPUTTYPE_PINCHINPUT, CreateInputAction<PinchInput>},
        {INPUTTYPE_CROWNINPUT, CreateInputAction<WatchCrownInput>},
        {INPUTTYPE_IDLEINPUT, CreateInputAction<WatchIdleInput>},
        {INPUTTYPE_GESTURESINPUT, CreateInputAction<WatchGesturesInput>},
        {INPUTTYPE_KEYPRESSINPUT, CreateInputAction<WatchKeyPressInput>},
        {INPUTTYPE_FLOATSPLITINPUT, CreateInputAction<FloatAndSplitInput>},
        {INPUTTYPE_COLLAPSEINPUT, CreateInputAction<CollapseInput>},
        {INPUTTYPE_BROWSERINPUT, CreateInputAction<BrowserInput>}};

    auto it = inputActionMap.find(type);
    if (it != inputActionMap.end()) {
        return it->second();
    }

    return nullptr;
}
}  // namespace WuKong
}  // namespace OHOS

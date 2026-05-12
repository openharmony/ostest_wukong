/*
 *Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "input_info.h"

namespace OHOS {
namespace WuKong {

std::string InputInfo::InputTypeToString()
{
    std::string inputString = "";
    switch (inputType_) {
        case INPUTTYPE_TOUCHINPUT:
            inputString = "touch";
            break;
        case INPUTTYPE_SWAPINPUT:
            inputString = "swap";
            break;
        case INPUTTYPE_MOUSEINPUT:
            inputString = "mouse";
            break;
        case INPUTTYPE_KEYBOARDINPUT:
            inputString = "keyboard";
            break;
        case INPUTTYPE_APPSWITCHINPUT:
            inputString = "appswitch";
            break;
        case INPUTTYPE_HARDKEYINPUT:
            inputString = "hardkey";
            break;
        case INPUTTYPE_ROTATEINPUT:
            inputString = "rotate";
            break;
        case INPUTTYPE_KNUCKLEINPUT:
            inputString = "knuckle";
            break;
        case INPUTTYPE_PINCHINPUT:
            inputString = "pinch";
            break;
        case INPUTTYPE_CROWNINPUT:
            inputString = "watch_crown";
            break;
        case INPUTTYPE_IDLEINPUT:
            inputString = "watch_idle";
            break;
        case INPUTTYPE_GESTURESINPUT:
            inputString = "watch_gestures";
            break;
        case INPUTTYPE_KEYPRESSINPUT:
            inputString = "watch_keypress";
            break;
        case INPUTTYPE_FLOATSPLITINPUT:
            inputString = "floatOrsplit";
            break;
        case INPUTTYPE_COLLAPSEINPUT:
            inputString = "collapse";
            break;
        case INPUTTYPE_BROWSERINPUT:
            inputString = "browser";
            break;
        default:
            break;
    }
    return inputString;
}
}  // namespace WuKong
}  // namespace OHOS

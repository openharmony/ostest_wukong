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
namespace {
const std::map<InputType, std::string> INPUT_TYPE_STRING_MAP = {
    {INPUTTYPE_TOUCHINPUT, "touch"},
    {INPUTTYPE_SWAPINPUT, "swap"},
    {INPUTTYPE_MOUSEINPUT, "mouse"},
    {INPUTTYPE_KEYBOARDINPUT, "keyboard"},
    {INPUTTYPE_APPSWITCHINPUT, "appswitch"},
    {INPUTTYPE_HARDKEYINPUT, "hardkey"},
    {INPUTTYPE_ROTATEINPUT, "rotate"},
    {INPUTTYPE_KNUCKLEINPUT, "knuckle"},
    {INPUTTYPE_PINCHINPUT, "pinch"},
    {INPUTTYPE_CROWNINPUT, "watch_crown"},
    {INPUTTYPE_IDLEINPUT, "watch_idle"},
    {INPUTTYPE_GESTURESINPUT, "watch_gestures"},
    {INPUTTYPE_KEYPRESSINPUT, "watch_keypress"},
    {INPUTTYPE_FLOATSPLITINPUT, "floatOrsplit"},
    {INPUTTYPE_COLLAPSEINPUT, "collapse"},
    {INPUTTYPE_BROWSERINPUT, "browser"},
};
}  // namespace

std::string InputInfo::InputTypeToString()
{
    auto it = INPUT_TYPE_STRING_MAP.find(inputType_);
    if (it != INPUT_TYPE_STRING_MAP.end()) {
        return it->second;
    }
    return "";
}
}  // namespace WuKong
}  // namespace OHOS

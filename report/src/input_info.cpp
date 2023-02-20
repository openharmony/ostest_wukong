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
            /* code */
            inputString = "touch";
            break;
        case INPUTTYPE_SWAPINPUT:
            /* code */
            inputString = "swap";
            break;
        case INPUTTYPE_MOUSEINPUT:
            /* code */
            inputString = "mouse";
            break;
        case INPUTTYPE_KEYBOARDINPUT:
            /* code */
            inputString = "keyboard";
            break;
        case INPUTTYPE_APPSWITCHINPUT:
            /* code */
            inputString = "appswitch";
            break;
        case INPUTTYPE_HARDKEYINPUT:
            /* code */
            inputString = "hardkey";
            break;
        default:
            break;
    }
    return inputString;
}
}  // namespace WuKong
}  // namespace OHOS

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

#include "input_info.h"

namespace OHOS {
namespace WuKong {
InputInfo::InputInfo()
{
    reset();
}
InputInfo::~InputInfo()
{
}
void InputInfo::SetBundleName(const std::string &bundleName)
{
    bundleName_ = bundleName;
}

std::string InputInfo::GetBundleName()
{
    return bundleName_;
}

void InputInfo::SetAbilityName(const std::string &abilityName)
{
    abilityName_ = abilityName;
}

std::string InputInfo::GetAbilityName()
{
    return abilityName_;
}

void InputInfo::SetInputType(InputType inputType)
{
    inputType_ = inputType;
}

void InputInfo::SetComponmentName(const std::string &componmentName)
{
    componmentName_ = componmentName;
}

std::string InputInfo::GetComponmentName()
{
    return componmentName_;
}

void InputInfo::SetComponmentInputedTimes(int componmentInputedTimes)
{
    componmentInputedTimes_ = componmentInputedTimes;
}
std::string InputInfo::GetComponmentInputedTimes()
{
    std::string componmentInputedTimes = std::to_string(componmentInputedTimes_);
    return componmentInputedTimes;
}
void InputInfo::SetComponmentTotals(int componmentTotals)
{
    componmentTotals_ = componmentTotals;
}
std::string InputInfo::GetComponmentTotals()
{
    std::string componmentTotals = std::to_string(componmentTotals_);
    return componmentTotals;
}
void InputInfo::reset()
{
    bundleName_ = "unknow";
    abilityName_ = "unknow";
    inputType_ = INPUTTYPE_INVALIDINPUT;
    componmentName_ = "";
    componmentInputedTimes_ = 0;
    componmentTotals_ = 0;
}
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
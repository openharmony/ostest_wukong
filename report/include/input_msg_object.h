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

#ifndef TEST_WUKONG_INPUF_MSG_OBJECT_H
#define TEST_WUKONG_INPUF_MSG_OBJECT_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "input_action.h"
#include "singleton.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
enum inputedMode {
    multimodeInput = 0,
    componmentInput,
    invalidIput
};
class InputedMsgObject {
public:
    InputedMsgObject(inputedMode inputedMode)
    {
        inputedMode_ = inputedMode;
    }

    inputedMode GetInputedMode()
    {
        return inputedMode_;
    }
    virtual ~InputedMsgObject() = default;

private:
    inputedMode inputedMode_ = invalidIput;
};

class MultimodeInputMsg : public InputedMsgObject {
public:
    MultimodeInputMsg() : InputedMsgObject(multimodeInput)
    {
    }
    std::string GetInputType()
    {
        std::string inputString = "";
        DEBUG_LOG_STR("inputType{%d}", inputType_);
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
    virtual ~MultimodeInputMsg() = default;
    InputType inputType_ = INPUTTYPE_INVALIDINPUT;
};

class ComponmentInputMsg : public InputedMsgObject {
public:
    ComponmentInputMsg() : InputedMsgObject(componmentInput)
    {
    }
    virtual ~ComponmentInputMsg() = default;
    std::string componmentType_ = "";
    uint32_t pageId_ = 0;
    std::vector<std::string> pageComponments;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

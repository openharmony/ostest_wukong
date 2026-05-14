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
        static const std::map<InputType, std::string> inputTypeMap = {
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
            {INPUTTYPE_BROWSERINPUT, "browser"}
        };
        DEBUG_LOG_STR("inputType{%d}", inputType_);
        auto it = inputTypeMap.find(inputType_);
        return (it != inputTypeMap.end()) ? it->second : "";
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
    std::string pagePath_;
    std::string content_;
    int32_t startX_ = 0;
    int32_t startY_ = 0;
    int32_t endX_ = 0;
    int32_t endY_ = 0;
    int64_t pssTotal_ = 0;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

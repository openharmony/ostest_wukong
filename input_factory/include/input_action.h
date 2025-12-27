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

#ifndef TEST_WUKONG_INPUT_ACTION_H
#define TEST_WUKONG_INPUT_ACTION_H

#include <string>
#include <unistd.h>
#include <map>

#include "app_manager.h"
#include "input_manager.h"
#include "special_test_object.h"
#include "wukong_define.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
enum InputType {
    INPUTTYPE_TOUCHINPUT,      // input touch event
    INPUTTYPE_SWAPINPUT,       // input swap event
    INPUTTYPE_MOUSEINPUT,      // input mouse event
    INPUTTYPE_KEYBOARDINPUT,   // input keyboard event
    INPUTTYPE_ELEMENTINPUT,    // input element event
    INPUTTYPE_APPSWITCHINPUT,  // input appswitch event
    INPUTTYPE_HARDKEYINPUT,    // input power event
    INPUTTYPE_ROTATEINPUT,     // input rotate event
    INPUTTYPE_KNUCKLEINPUT,
    INPUTTYPE_PINCHINPUT,
    INPUTTYPE_CROWNINPUT,
    INPUTTYPE_IDLEINPUT,
    INPUTTYPE_GESTURESINPUT,
    INPUTTYPE_KEYPRESSINPUT,
    INPUTTYPE_FLOATSPLITINPUT,
    INPUTTYPE_COLLAPSEINPUT,
    INPUTTYPE_BROWSERINPUT,
    INPUTTYPE_INVALIDINPUT,  // input invaild event
    INPUTTYPE_RECORDINPUT,   // input record event
    INPUTTYPE_REPPLAYINPUT   // input replay event
};

enum FingerNumber { F_ONE = 1, F_TWO, F_THREE, F_FOUR };
const std::string DIRECTION = "udlr";
const std::string MOUSE_ACTION_STR = "bcgm";
const int PERCENT_ONE_HUNDRED = 100;
enum CollapseStatus { C_EXPANDED = 1, C_HALF_FOLDED, C_FOLDED, C_UNKNOWN };
// 创建一个映射，将枚举值映射到字符串
const std::map<int, std::string> StatusToDesc = {
    {CollapseStatus::C_EXPANDED, "EXPANDED"},
    {CollapseStatus::C_HALF_FOLDED, "HALF_FOLD"},
    {CollapseStatus::C_FOLDED, "FOLD"},
    {CollapseStatus::C_UNKNOWN, "UNKNOWN"}};

/**
 * @brief Input base class, provide the "Random, Next, Report" function for input action.
 */
class InputAction {
public:
    int arg_interval;
    bool enableFingerArg = false;
    std::map<int, float> fingersMap_;
    std::vector<int> fingerVector_;
    bool enablePause = false;
    std::map<char, float> directionMap_;
    std::vector<char> directionVector_;

    InputAction()
    {
    }
    virtual ~InputAction()
    {
    }
    virtual ErrCode OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
    {
        return OHOS::ERR_INVALID_VALUE;
    }
    /**
     * @brief input a touch event in random test.
     * @return Return ERR_OK on success， others on failure.
     */
    virtual ErrCode RandomInput()
    {
        return OHOS::ERR_INVALID_VALUE;
    }
    /**
     * @brief input a touch event in random test for uri and uriType.
     * @return Return ERR_OK on success， others on failure.
     */
    virtual ErrCode RandomInput(const std::string &uri, const std::string &uriType)
    {
        return OHOS::ERR_INVALID_VALUE;
    }
    /**
     * @brief input a touch event in random test for uri and abilityname.
     * @return Return ERR_OK on success， others on failure.
     */
    virtual ErrCode RandomInput(const std::string &uri, const std::vector<std::string> &abilityName)
    {
        return OHOS::ERR_INVALID_VALUE;
    }

    /**
     * @brief input a touch event in focus test for a certain times.
     * @return Return ERR_OK on success， others on failure.
     */
    virtual ErrCode FocusInput(bool shouldScreenCap)
    {
        return OHOS::ERR_INVALID_VALUE;
    }

    virtual InputType GetInputInfo()
    {
        return INPUTTYPE_INVALIDINPUT;
    }
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_INPUT_ACTION_H

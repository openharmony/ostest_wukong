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
    INPUTTYPE_RECORDINPUT,     // input record event
    INPUTTYPE_REPPLAYINPUT,    // input replay event
    INPUTTYPE_INVALIDINPUT     // input invaild event
};

/**
 * @brief Input base class, provide the "Random, Next, Report" function for input action.
 */
class InputAction {
public:
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

    virtual ErrCode GetInputInfo()
    {
        return OHOS::ERR_INVALID_VALUE;
    }
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_INPUT_ACTION_H

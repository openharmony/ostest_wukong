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

#ifndef TEST_WUKONG_ELEMENT_INPUT_H
#define TEST_WUKONG_ELEMENT_INPUT_H

#include <string>

#include "input_action.h"

namespace OHOS {
namespace WuKong {
class ElementInput : public InputAction {
public:
    ElementInput();
    ~ElementInput();
    ErrCode OrderInput(std::shared_ptr<SpcialTestObject>& specialTestObject);
    ErrCode RandomInput();
    InputType GetInputInfo();
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_ELEMENT_INPUT_H
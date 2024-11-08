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

#ifndef TEST_WUKONG_COMPONENT_INPUT_H
#define TEST_WUKONG_COMPONENT_INPUT_H

#include <string>

#include "accessibility_element_info.h"
#include "component_tree.h"
#include "input_action.h"
#include "special_test_object.h"
#include "tree_manager.h"

namespace OHOS {
namespace WuKong {
class ComponentInput : public InputAction {
public:
    ComponentInput();
    ~ComponentInput();
    ErrCode OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject);
    ErrCode RandomInput();
    ErrCode FocusInput(bool shouldScreenCap);
    InputType GetInputInfo();

private:
    int JudgeComponentType(OHOS::Accessibility::AccessibilityElementInfo& elementInfo);
    uint32_t ChooseRightComponentIndex(
        std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> &componentInfos,
        std::vector<std::shared_ptr<ComponentTree>> &wComponentInfos, bool shouldScreenCap);
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_COMPONENT_INPUT_H

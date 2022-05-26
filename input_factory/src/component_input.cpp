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

#include "component_input.h"

#include "component_manager.h"
#include "input_manager.h"
#include "scene_delegate.h"
#include "tree_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
ComponentInput::ComponentInput() : InputAction()
{
}

ComponentInput::~ComponentInput()
{
}

ErrCode ComponentInput::OrderInput(std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    ErrCode result = OHOS::ERR_OK;
    auto treemanager = TreeManager::GetInstance();
    result = treemanager->UpdateComponentInfo();
    DEBUG_LOG_STR("update componentinfo result (%d)", result);
    if (result == OHOS::ERR_OK) {
        SceneDelegate delegate;
        delegate.ChooseScene(false);
        if (delegate.IsBackToPrePage()) {
            result = ComponentManager::GetInstance()->BackToPrePage();
        } else {
            auto elementInfo = treemanager->GetElementInfoByOrder();
            OHOS::Accessibility::ActionType actionType = JudgeComponentType(*(elementInfo.get()));
            if (actionType == Accessibility::ACCESSIBILITY_ACTION_INVALID) {
                actionType = OHOS::Accessibility::ACCESSIBILITY_ACTION_CLICK;
            }
            result = ComponentManager::GetInstance()->ComponentEventInput(*(elementInfo.get()), actionType);
            if (result == OHOS::ERR_OK) {
                treemanager->SetInputcomponentIndex(actionType);
            }
        }
    }
    DEBUG_LOG_STR("component order input result (%d)", result);
    return result;
}

ErrCode ComponentInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    auto treemanager = TreeManager::GetInstance();
    result = treemanager->UpdateComponentInfo();
    DEBUG_LOG_STR("update componentinfo result (%d)", result);
    if (result == OHOS::ERR_OK) {
        SceneDelegate delegate;
        delegate.ChooseScene(true);
        auto componentInfos = treemanager->GetActiveElementInfos();
        int index = -1;
        DEBUG_LOG_STR("component list size (%d)", componentInfos.size());
        DEBUG_LOG_STR("back: %d", delegate.IsBackToPrePage());
        if (delegate.IsBackToPrePage()) {
            result = ComponentManager::GetInstance()->BackToPrePage();
        } else if (componentInfos.size() > 0) {
            index = rand() % componentInfos.size();
            DEBUG_LOG_STR("component input index (%d)", index);
            OHOS::Accessibility::ActionType actionType = JudgeComponentType(*(componentInfos[index].get()));
            if (actionType == Accessibility::ACCESSIBILITY_ACTION_INVALID) {
                actionType = OHOS::Accessibility::ACCESSIBILITY_ACTION_CLICK;
            }
            result = ComponentManager::GetInstance()->ComponentEventInput(*(componentInfos[index].get()), actionType);
            if (result == OHOS::ERR_OK) {
                treemanager->SetInputcomponentIndex(actionType, index);
            }
        }
    }
    DEBUG_LOG_STR("component random input result (%d)", result);
    return result;
}

ErrCode ComponentInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}

OHOS::Accessibility::ActionType ComponentInput::JudgeComponentType(
    OHOS::Accessibility::AccessibilityElementInfo& elementInfo)
{
    OHOS::Accessibility::ActionType actionType = Accessibility::ACCESSIBILITY_ACTION_INVALID;
    std::vector<OHOS::Accessibility::AccessibleAction> actionlist = elementInfo.GetActionList();
    if (actionlist.empty()) {
        std::string componentType = elementInfo.GetComponentType();
        TRACK_LOG_STR("component type: %s", componentType.c_str());
        if (componentType == "TextInput" || componentType == "TextArea") {
            actionType = Accessibility::ACCESSIBILITY_ACTION_SET_TEXT;
        } else {
            actionType = Accessibility::ACCESSIBILITY_ACTION_CLICK;
        }
    } else {
        auto it = actionlist[rand() % actionlist.size()];
        actionType = it.GetActionType();
    }
    TRACK_LOG_STR("action type: %d", actionType);
    return actionType;
}
}  // namespace WuKong
}  // namespace OHOS

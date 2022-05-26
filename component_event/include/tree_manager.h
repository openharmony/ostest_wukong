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

#ifndef TEST_WUKONG_TREE_MANAGER_H
#define TEST_WUKONG_TREE_MANAGER_H

#include "ability_tree.h"
#include "accessibility_element_info.h"
#include "component_tree.h"
#include "page_tree.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint32_t INVALIDED_INPUT_INDEX = 0xFFFFFFFF;
}
/**
 * @brief Generate component tree, get AccessibilityElementInfo for current active components.
 */
class TreeManager : public DelayedSingleton<TreeManager> {
    /**
     * for test flow to get and set test element info.
     */
public:
    /**
     * @brief update wukong tree by AccessibilityUITestAbility.
     * @return An AccessibilityElementInfo
     */
    ErrCode UpdateComponentInfo();

    /**
     * @brief get AccessibilityElementInfo for the Preorder Traversal Algorithms.
     * @return An AccessibilityElementInfo
     */
    const std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>& GetElementInfoByOrder()
    {
        return inputElementInfo_;
    }

    /**
     * @brief get AccessibilityElementInfo list of active component.
     * @return input AccessibilityElementInfo list
     */
    const std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>>& GetActiveElementInfos()
    {
        return inputElementInfoList_;
    }

    /**
     * @brief set input event component, and input type.
     * @param index index of active element info list.
     * @param actionType component input type.
     */
    void SetInputcomponentIndex(OHOS::Accessibility::ActionType actionType, uint32_t index = INVALIDED_INPUT_INDEX);

    /**
     * for scene update tree.
     */
public:
    /**
     * @brief set ComponentTree list of active component.
     * @param inputComponentList ComponentTree list.
     */
    void SetActiveComponent(const std::vector<std::shared_ptr<ComponentTree>>& inputComponentList);

    /**
     * @brief set a ComponentTree of active component.
     * @param inputComponent a ComponentTree.
     */
    void SetActiveComponent(const std::shared_ptr<ComponentTree>& inputComponent);

    /**
     * @brief get current component tree.
     * @return A ComponentTree
     */
    std::shared_ptr<ComponentTree> GetCurrentComponents()
    {
        return currentComponentNode_;
    }
    /**
     * @brief get new component tree
     * @return A ComponentTree
     */
    std::shared_ptr<ComponentTree> GetNewComponents()
    {
        return newComponentNode_;
    }

    /**
     * @brief get current page node.
     * @return A ComponentTree
     */
    std::shared_ptr<PageTree> GetCurrentPage()
    {
        return currentPageNode_;
    }

    /**
     * @brief get new page node
     * @return A ComponentTree
     */
    std::shared_ptr<PageTree> GetNewPage()
    {
        return newPageNode_;
    }
    /**
     * @brief add current page as a new page
     * @return add new page result
     */
    bool AddPage();
    /**
     * @brief remove new page.
     * @return remove new page result
     */
    bool SamePage();

    /**
     * @brief back and goto existed page
     * @param layer 0 update current page, < 0 update parent page, > 0 update child page
     * @param index child index
     * @return update page result
     */
    bool UpdatePage(int layer, int index = -1);

    const std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> GetNewElementInfoList(uint32_t index)
    {
        if (index < newElementInfoList_.size()) {
            return newElementInfoList_[index];
        } else {
            return {};
        }
    }
    const std::shared_ptr<ComponentTree> GetComponents(uint32_t index)
    {
        if (index < componentTreeList_.size()) {
            return componentTreeList_[index];
        } else {
            return {};
        }
    }
    DECLARE_DELAYED_SINGLETON(TreeManager);

private:
    bool RecursGetChildElementInfo(std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>& parent,
                                   std::shared_ptr<WuKongTree> componentParent);
    bool RecursComponent(std::shared_ptr<ComponentTree> componentTree);
    bool FindAbility(const std::shared_ptr<AbilityTree>& abilityNode);
    ErrCode MakeAndCheckNewAbility();
    bool UpdateCurrentPage(bool isAdd = false);
    bool RemovePage();

    std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> inputElementInfo_;
    std::shared_ptr<ComponentTree> inputComponent_;
    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> inputElementInfoList_;
    std::vector<std::shared_ptr<ComponentTree>> inputComponentList_;

    std::shared_ptr<ComponentTree> currentComponentNode_;
    std::shared_ptr<ComponentTree> newComponentNode_;

    std::shared_ptr<AbilityTree> currentAbilityNode_;
    std::shared_ptr<AbilityTree> newAbilityNode_;

    std::shared_ptr<PageTree> currentPageNode_;
    std::shared_ptr<PageTree> newPageNode_;

    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> newElementInfoList_;

    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> elementInfoList_;
    std::vector<std::shared_ptr<ComponentTree>> componentTreeList_;

    std::vector<std::shared_ptr<AbilityTree>> abilityTreeList_;
    std::vector<std::shared_ptr<PageTree>> pageTreeList_;

    bool isUpdateComponentFinished_;
    bool isNewAbility_;
    std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> invaledElementInfo_;
    std::shared_ptr<ComponentTree> invaledComponent_;
    std::shared_ptr<PageTree> invaledPage_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif
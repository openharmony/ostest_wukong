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
     * @brief init and clear container.
     */
    void InitContainer()
    {
        elementInfoList_.clear();
        abilityTreeList_.clear();
        pageTreeList_.clear();
        componentTreeList_.clear();
    }
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
    void SetInputcomponentIndex(int actionType, uint32_t index = INVALIDED_INPUT_INDEX);

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
    const std::shared_ptr<ComponentTree>& GetCurrentComponents()
    {
        return currentComponentNode_;
    }
    /**
     * @brief get new component tree
     * @return A ComponentTree
     */
    const std::shared_ptr<ComponentTree>& GetNewComponents()
    {
        return newComponentNode_;
    }

    /**
     * @brief get current page node.
     * @return A ComponentTree
     */
    const std::shared_ptr<PageTree>& GetCurrentPage()
    {
        return currentPageNode_;
    }

    /**
     * @brief get new page node
     * @return A ComponentTree
     */
    const std::shared_ptr<PageTree>& GetNewPage()
    {
        return newPageNode_;
    }

    /**
     * @brief get current ability node.
     * @return A AblilityTree
     */
    const std::shared_ptr<AbilityTree>& GetCurrentAbility()
    {
        return currentAbilityNode_;
    }

    /**
     * @brief get all app bundle tree.
     * @return A AblilityTree list
     */
    const std::vector<std::shared_ptr<AbilityTree>>& GetBundleList()
    {
        return abilityTreeList_;
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
    bool UpdatePage(int layer, uint32_t index = INVALIDED_INPUT_INDEX);

    const std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> GetNewElementInfoList(uint32_t index)
    {
        if (index < newElementInfoList_.size()) {
            return newElementInfoList_[index];
        } else {
            return {};
        }
    }

    bool RecursComponent(const std::shared_ptr<ComponentTree>& componentTree);
    DECLARE_DELAYED_SINGLETON(TreeManager);

private:
    bool RecursGetChildElementInfo(const std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>& parent,
                                   const std::shared_ptr<ComponentTree>& componentParent);
    bool FindAbility(const std::shared_ptr<AbilityTree>& abilityNode);
    ErrCode MakeAndCheckNewAbility();
    bool UpdateCurrentPage(bool isAdd = false);
    bool RemovePage();

    std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo> inputElementInfo_;
    std::shared_ptr<ComponentTree> inputComponent_;
    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> inputElementInfoList_;
    std::vector<std::shared_ptr<ComponentTree>> inputComponentList_;

    std::shared_ptr<ComponentTree> currentComponentNode_ = std::make_shared<ComponentTree>();
    std::shared_ptr<ComponentTree> newComponentNode_ = std::make_shared<ComponentTree>();

    std::shared_ptr<AbilityTree> currentAbilityNode_ = std::make_shared<AbilityTree>();
    std::shared_ptr<AbilityTree> newAbilityNode_ = std::make_shared<AbilityTree>();

    std::shared_ptr<PageTree> currentPageNode_ = std::make_shared<PageTree>();
    std::shared_ptr<PageTree> newPageNode_ = std::make_shared<PageTree>();

    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> newElementInfoList_;

    std::vector<std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>> elementInfoList_;

    std::vector<std::shared_ptr<AbilityTree>> abilityTreeList_;
    std::map<std::uint32_t, std::shared_ptr<PageTree>> pageTreeList_;
    std::map<std::uint32_t, std::shared_ptr<ComponentTree>> componentTreeList_;

    bool isUpdateComponentFinished_ = false;
    bool isNewAbility_ = false;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

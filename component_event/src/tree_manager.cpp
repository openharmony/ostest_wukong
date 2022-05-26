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
#include "tree_manager.h"

#include "accessibility_ui_test_ability.h"
#include "component_manager.h"
#include "touch_input.h"

namespace OHOS {
namespace WuKong {
namespace {
class ComponentManagerMonitor : public ComponentManagerListener {
    virtual void OnStatusUpdated(ComponentStatus stutus) override
    {
    }
    virtual void OnScreenUpdated() override
    {
    }
    virtual void OnPermissionScreenShown() override
    {
    }
};
}  // namespace

TreeManager::TreeManager()
    : isUpdateComponentFinished_(false),
      invaledElementInfo_(nullptr),
      invaledComponent_(nullptr),
      invaledPage_(nullptr)
{
}
TreeManager::~TreeManager()
{
}

bool TreeManager::RecursGetChildElementInfo(std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>& parent,
                                            std::shared_ptr<WuKongTree> componentParent)
{
    bool res = true;
    if (componentParent == nullptr) {
        ERROR_LOG("tree parent is null!");
        return false;
    }
    for (int32_t i = 0; i < parent->GetChildCount(); i++) {
        auto elementChild = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
        // Get child AccessibilityElementInfo from Accessibility.
        res = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetChildElementInfo(
            i, *(parent.get()), *(elementChild.get()));
        if (!res) {
            ERROR_LOG("GetChildElementInfo failed!");
            return res;
        }
        TRACK_LOG_STR("GetChildElementInfo child ID (%d), child count (%d), Type (%s)",
                      elementChild->GetAccessibilityId(), elementChild->GetChildCount(),
                      elementChild->GetComponentType().c_str());

        // save child AccessibilityElementInfo.
        newElementInfoList_.push_back(elementChild);

        // Generate ComponentTree.
        std::shared_ptr<ComponentTree> componentChild = std::make_shared<ComponentTree>();
        componentChild->SetIndex(newElementInfoList_.size() - 1);
        // set ComponentTree parent
        componentChild->SetParent(componentParent);
        componentParent->AddChild(componentChild);
        // Recurs get child AccessibilityElementInfo.
        res = RecursGetChildElementInfo(elementChild, componentChild);
        if (!res) {
            return res;
        }
    }

    componentParent->SetNodeId();
    return res;
}
bool TreeManager::RecursComponent(std::shared_ptr<ComponentTree> componentTree)
{
    if (componentTree == nullptr) {
        return false;
    }
    auto children = componentTree->GetChildren();

    auto elementInfo = GetNewElementInfoList(componentTree->GetIndex());
    TRACK_LOG_STR("Component Node Indxe: (%d), Child Count: (%d), Component Tree child ID (%d), Type (%s)",
                  componentTree->GetIndex(), children.size(), elementInfo->GetAccessibilityId(),
                  elementInfo->GetComponentType().c_str());
    for (auto tree : children) {
        RecursComponent(std::static_pointer_cast<ComponentTree>(tree));
    }
    return true;
}

bool TreeManager::FindAbility(const std::shared_ptr<AbilityTree>& abilityNode)
{
    if (newAbilityNode_->IsEqual(abilityNode)) {
        DEBUG_LOG("Found same old ability");
        currentAbilityNode_ = abilityNode;
        return true;
    } else {
        for (auto child : abilityNode->GetChildren()) {
            if (FindAbility(std::static_pointer_cast<AbilityTree>(child))) {
                return true;
            }
        }
        return false;
    }
}

ErrCode TreeManager::MakeAndCheckNewAbility()
{
    isNewAbility_ = false;
    // Check ability state
    newAbilityNode_ = std::make_shared<AbilityTree>();
    newAbilityNode_->SetNodeId();

    // check same abiliby as current ability
    if (currentAbilityNode_ != nullptr) {
        if (newAbilityNode_->IsEqual(currentAbilityNode_)) {
            DEBUG_LOG("Ability not change");
            return OHOS::ERR_OK;
        }
    }

    DEBUG_LOG("Ability changed");
    bool isNewBundle = true;
    bool isNewAbility = false;
    for (auto abilityTree : abilityTreeList_) {
        TRACK_LOG_STR("NewAbility Bundle: (%s), Bundle List Name: (%s)", newAbilityNode_->bundleName_.c_str(),
                      abilityTree->bundleName_.c_str());
        // check is new bundle.
        if (abilityTree->bundleName_ == newAbilityNode_->bundleName_) {
            isNewBundle = false;
            // find new ability in ability tree.
            if (!FindAbility(abilityTree)) {
                currentAbilityNode_->AddChild(newAbilityNode_);
                newAbilityNode_->SetParent(currentAbilityNode_);
                currentAbilityNode_ = newAbilityNode_;
                isNewAbility = true;
            }
        }
    }

    // save new bundle for launch multi-application
    if (isNewBundle) {
        abilityTreeList_.push_back(newAbilityNode_);
        currentAbilityNode_ = newAbilityNode_;
    }

    // clear current screen data when it is new ability.
    if (isNewBundle || isNewAbility) {
        currentComponentNode_ = nullptr;
        currentPageNode_ = nullptr;
        isNewAbility_ = true;
    } else if (!isNewBundle && !isNewAbility) {
        // set old screen to current screen data when it is old ability.
        if (currentAbilityNode_->GetIndex() >= pageTreeList_.size()) {
            ERROR_LOG_STR("ability index (%d) more than pageTreeList count (%d)", currentAbilityNode_->GetIndex(),
                          pageTreeList_.size());
            return OHOS::ERR_INVALID_OPERATION;
        }
        currentPageNode_ = pageTreeList_[currentAbilityNode_->GetIndex()];
        if (currentPageNode_->GetIndex() >= componentTreeList_.size()) {
            ERROR_LOG_STR("page index (%d) more than componentTreeList count (%d)", currentPageNode_->GetIndex(),
                          componentTreeList_.size());
            return OHOS::ERR_INVALID_OPERATION;
        }
        currentComponentNode_ = componentTreeList_[currentPageNode_->GetIndex()];
    }
    return OHOS::ERR_OK;
}

ErrCode TreeManager::UpdateComponentInfo()
{
    TRACK_LOG_STD();
    ErrCode result = OHOS::ERR_OK;
    // start update component tree.
    isUpdateComponentFinished_ = false;
    newElementInfoList_.clear();

    auto root = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
    auto aacPtr = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance();

    // Get root AccessibilityElementInfo from Accessibility,
    bool bResult = aacPtr->GetRoot(*(root.get()));
    if (!bResult) {
        ERROR_LOG("Accessibility Ability get root element info failed!");
        return OHOS::ERR_INVALID_OPERATION;
    } else {
        // save root AccessibilityElementInfo.
        newElementInfoList_.push_back(root);
        uint32_t count = newElementInfoList_.size();
        DEBUG_LOG_STR("New Element Info count (%d), Type (%s)", root->GetChildCount(),
                      root->GetComponentType().c_str());

        // Generate new ComponentTree.
        newComponentNode_ = std::make_shared<ComponentTree>();
        newComponentNode_->SetIndex(count - 1);

        // Recurs get all children AccessibilityElementInfo.
        bResult = RecursGetChildElementInfo(root, newComponentNode_);
        if (!bResult) {
            return OHOS::ERR_INVALID_OPERATION;
        }
        if (LOG_LEVEL_DEBUG >= WuKongLogger::GetInstance()->GetLogLevel()) {
            RecursComponent(newComponentNode_);
        }
    }
    // Generate Page Node
    newPageNode_ = std::make_shared<PageTree>();
    newPageNode_->SetNodeId();

    // Generate Ability Node
    result = MakeAndCheckNewAbility();

    TRACK_LOG_END();
    return result;
}

void TreeManager::SetInputcomponentIndex(OHOS::Accessibility::ActionType actionType, uint32_t index)
{
    DEBUG_LOG_STR("Input: (%d), Input Type: (%d)", index, actionType);
    if (currentPageNode_ == nullptr) {
        ERROR_LOG("current page is null!");
        return;
    }

    // component input count statistics
    if (index != INVALIDED_INPUT_INDEX) {
        if (inputComponentList_.size() <= 0) {
            ERROR_LOG("input compoent list is null!");
            return;
        }
        if (index >= inputComponentList_.size()) {
            ERROR_LOG("index argument is invalided");
            return;
        }
        inputComponentList_[index]->AddInputCount();
        inputComponentList_[index]->AddTypeInputCount(actionType);
    } else {
        if (inputComponent_ == nullptr) {
            ERROR_LOG("inputComponent_ is nullptr");
            return;
        }
        inputComponent_->AddInputCount();
        inputComponent_->AddTypeInputCount(actionType);
        DEBUG_LOG_STR("inputComponent_: GetNodeId (0x%016llX)", inputComponent_->GetNodeId());
    }

    // ability input count statistics
    currentAbilityNode_->AddInputCount();

    // page input count statistics
    currentPageNode_->AddInputCount();
}

void TreeManager::SetActiveComponent(const std::vector<std::shared_ptr<ComponentTree>>& inputComponentList)
{
    DEBUG_LOG_STR("Active component list: size (%d)", inputComponentList.size());
    // Clear old input data
    inputElementInfoList_.clear();
    inputComponentList_.clear();
    uint32_t count = elementInfoList_.size();
    for (auto componentNode : inputComponentList) {
        if (count > componentNode->GetIndex()) {
            // Save input pointer list
            inputElementInfoList_.push_back(elementInfoList_[componentNode->GetIndex()]);
            inputComponentList_.push_back(componentNode);
        } else {
            ERROR_LOG_STR("component index (%d) more than elementInfoList count (%d)", componentNode->GetIndex(),
                          count);
            break;
        }
    }
}

void TreeManager::SetActiveComponent(const std::shared_ptr<ComponentTree>& inputComponent)
{
    DEBUG_LOG_STR("Active component: GetNodeId (0x%016llX)", inputComponent->GetNodeId());
    // Save one input pointer.
    if (inputComponent == nullptr) {
        ERROR_LOG("argument failed inputComponent is nullptr");
        return;
    }
    inputComponent_ = inputComponent;
    if (elementInfoList_.size() > inputComponent->GetIndex()) {
        inputElementInfo_ = elementInfoList_[inputComponent->GetIndex()];
    } else {
        ERROR_LOG_STR("component index (%d) more than elementInfoList count (%d)", inputComponent->GetIndex(),
                      elementInfoList_.size());
    }
}

bool TreeManager::AddPage()
{
    TRACK_LOG_STD();
    // save new component tree, and change current conmponent tree.
    UpdateCurrentPage(true);

    // page tree growth
    if (newPageNode_ == nullptr) {
        ERROR_LOG("the new Page Node is null");
        return false;
    }
    if (currentPageNode_ != nullptr) {
        newPageNode_->SetParent(currentPageNode_);
        currentPageNode_->AddChild(newPageNode_);
    }
    newPageNode_->SetIndex(componentTreeList_.size() - 1);
    currentPageNode_ = newPageNode_;

    // ability tree growth
    if (isNewAbility_) {
        pageTreeList_.push_back(currentPageNode_);
        currentAbilityNode_->SetIndex(pageTreeList_.size() - 1);
    }
    TRACK_LOG_END();
    return SamePage();
}

bool TreeManager::SamePage()
{
    TRACK_LOG_STD();
    isUpdateComponentFinished_ = true;
    newElementInfoList_.clear();
    newComponentNode_.reset();
    newPageNode_.reset();
    newAbilityNode_.reset();
    TRACK_LOG_END();
    return true;
}

bool TreeManager::UpdatePage(int layer, int index)
{
    TRACK_LOG_STD();
    DEBUG_LOG_STR("UpdatePage: layer (%d), index (%d)", layer, index);
    std::shared_ptr<WuKongTree> pageNode = currentPageNode_;
    if (layer > 0) {
        if (pageNode->GetChildren().size() <= index) {
            ERROR_LOG_STR("UpdatePage child index (%d) more than elementInfoList_ GetChildren() size (%d)", index,
                          pageNode->GetChildren().size());
            return false;
        }
        currentPageNode_ = std::static_pointer_cast<PageTree>(pageNode->GetChildren()[index]);
    } else {
        while (layer >= 0) {
            layer++;
            pageNode = pageNode->GetParent();
            if (pageNode == nullptr) {
                ERROR_LOG_STR("UpdatePage back layer (%d) more than currentPageNode_ parent (%p)", layer,
                              pageNode.get());
                return false;
            }
        }
        currentPageNode_ = std::static_pointer_cast<PageTree>(pageNode);
    }

    if (!RemovePage()) {
        return false;
    }

    if (!UpdateCurrentPage()) {
        return false;
    }

    TRACK_LOG_END();
    return SamePage();
}

bool TreeManager::RemovePage()
{
    TRACK_LOG_STD();
    uint32_t startIndex = currentPageNode_->GetIndex();
    uint32_t endIndex = startIndex + currentPageNode_->count_ - 1;
    uint32_t componentTreeListCount = componentTreeList_.size();
    if (startIndex >= componentTreeListCount || endIndex >= componentTreeListCount) {
        ERROR_LOG_STR("currentPageNode StartIndex (%d) EndIndex (%d) more than componentTreeList_ size (%d)",
                      startIndex, endIndex, componentTreeListCount);
        return false;
    }
    uint32_t elementInfoListCount = elementInfoList_.size();
    if (startIndex >= elementInfoListCount || endIndex >= elementInfoListCount) {
        ERROR_LOG_STR("currentPageNode StartIndex (%d) EndIndex (%d) more than elementInfoList_ size (%d)", startIndex,
                      endIndex, elementInfoListCount);
        return false;
    }

    componentTreeList_.erase(componentTreeList_.begin() + startIndex, componentTreeList_.begin() + endIndex);
    elementInfoList_.erase(elementInfoList_.begin() + startIndex, elementInfoList_.begin() + endIndex);

    TRACK_LOG_END();
    return true;
}

bool TreeManager::UpdateCurrentPage(bool isAdd)
{
    TRACK_LOG_STD();
    if (newComponentNode_ == nullptr || newPageNode_ == nullptr) {
        ERROR_LOG_STR("new list is nullptr: (%p), (%p)", newComponentNode_.get(), newPageNode_.get());
        return false;
    }

    // push new element info to list for check old screen specification info.
    uint32_t count = elementInfoList_.size();
    DEBUG_LOG_STR("elementInfoList_: %d", count);
    for (auto elementInfo : newElementInfoList_) {
        elementInfoList_.push_back(elementInfo);
    }

    // update component tree index
    newComponentNode_->RecursUpdateNodeIndex(count);
    if (isAdd) {
        componentTreeList_.push_back(newComponentNode_);
    } else {
        newComponentNode_->RecursUpdateInfo(currentComponentNode_);
    }

    // set current sreen componentNode to new screen
    currentComponentNode_ = newComponentNode_;
    if (!isAdd) {
        if (currentPageNode_->GetIndex() >= componentTreeList_.size()) {
            ERROR_LOG_STR("UpdatePage currentPageNode_ index (%d) more than componentTreeList_ szie (%d)",
                          currentPageNode_->GetIndex(), componentTreeList_.size());
            return false;
        }
        // update component tree data.
        componentTreeList_[currentPageNode_->GetIndex()] = currentComponentNode_;
    }
    TRACK_LOG_END();
    return true;
}
}  // namespace WuKong
}  // namespace OHOS

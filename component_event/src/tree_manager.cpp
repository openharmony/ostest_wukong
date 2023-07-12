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
    void OnStatusUpdated(ComponentStatus status) override
    {
    }
    void OnScreenUpdated() override
    {
    }
    void OnPermissionScreenShown() override
    {
    }
};
}  // namespace

TreeManager::TreeManager() : isUpdateComponentFinished_(false), isNewAbility_(false)
{
}
TreeManager::~TreeManager()
{
    TRACK_LOG_STD();
}

bool TreeManager::RecursGetChildElementInfo(
    const std::shared_ptr<OHOS::Accessibility::AccessibilityElementInfo>& parent,
    const std::shared_ptr<ComponentTree>& componentParent)
{
    auto res = Accessibility::RET_OK;
    if (componentParent == nullptr) {
        ERROR_LOG("tree parent is null!");
        return false;
    }
    for (int32_t i = 0; i < parent->GetChildCount(); i++) {
        auto elementChild = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
        // Get child AccessibilityElementInfo from Accessibility.
        res = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance()->GetChildElementInfo(
            i, *(parent.get()), *(elementChild.get()));
        if (res != Accessibility::RET_OK) {
            ERROR_LOG("GetChildElementInfo failed!");
            return false;
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
        auto result = RecursGetChildElementInfo(elementChild, componentChild);
        if (!result) {
            return false;
        }
    }

    componentParent->SetNodeId();
    return res == Accessibility::RET_OK ? true : false;
}
bool TreeManager::RecursComponent(const std::shared_ptr<ComponentTree>& componentTree)
{
    if (LOG_LEVEL_TRACK < WuKongLogger::GetInstance()->GetLogLevel()) {
        return false;
    }
    if (componentTree == nullptr) {
        return false;
    }
    auto children = componentTree->GetChildren();

    auto elementInfo = GetNewElementInfoList(componentTree->GetIndex());
    if (elementInfo != nullptr) {
        TRACK_LOG_STR("Component Node Indxe:(%d), NodeId:(0x%016llX), input count:(%u), Element ID(%d), Type(%s)",
                      componentTree->GetIndex(), componentTree->GetNodeId(), componentTree->GetInputCount(),
                      elementInfo->GetAccessibilityId(), elementInfo->GetComponentType().c_str());
    } else {
        TRACK_LOG_STR("Component Node Indxe:(%d), NodeId:(0x%016llX), input count:(%u)", componentTree->GetIndex(),
                      componentTree->GetNodeId(), componentTree->GetInputCount());
    }

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
    // Check ability state
    newAbilityNode_ = std::make_shared<AbilityTree>();
    newAbilityNode_->SetNodeId();
    // check same abiliby as current ability
    if (currentAbilityNode_ != nullptr) {
        if (newAbilityNode_->IsEqual(currentAbilityNode_)) {
            DEBUG_LOG("Ability not change");
            return OHOS::ERR_OK;
        }
    } else if (currentAbilityNode_ == nullptr) {
        return OHOS::ERR_INVALID_OPERATION;
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
        InitContainer();
        abilityTreeList_.push_back(newAbilityNode_);
        currentAbilityNode_ = newAbilityNode_;
    }
    // clear current screen data when it is new ability.
    if (isNewBundle || isNewAbility) {
        currentComponentNode_ = nullptr;
        currentPageNode_ = nullptr;
        isNewAbility_ = true;
    } else {
        // set old screen to current screen data when it is old ability.
        if (pageTreeList_.find(currentAbilityNode_->GetIndex()) == pageTreeList_.end()) {
            ERROR_LOG_STR("ability index (%d) more than pageTreeList count (%d)", currentAbilityNode_->GetIndex(),
                          pageTreeList_.size());
            return OHOS::ERR_INVALID_OPERATION;
        }
        currentPageNode_ = pageTreeList_[currentAbilityNode_->GetIndex()];
        if (componentTreeList_.find(currentPageNode_->GetIndex()) == componentTreeList_.end()) {
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
    isNewAbility_ = false;
    newElementInfoList_.clear();
    if (WuKongLogger::GetInstance()->GetLogLevel() == LOG_LEVEL_TRACK) {
        DEBUG_LOG_STR("CompoentNode shared  new (%p) count = (%ld) unique (%d)", newComponentNode_.get(),
                      newComponentNode_.use_count(), newComponentNode_.unique());
        DEBUG_LOG_STR("CompoentNode shared (%p) count = (%ld) unique (%d)", currentComponentNode_.get(),
                      currentComponentNode_.use_count(), currentComponentNode_.unique());
    }
    // Generate Ability Node
    MakeAndCheckNewAbility();

    auto root = std::make_shared<OHOS::Accessibility::AccessibilityElementInfo>();
    auto aacPtr = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance();

    // Get root AccessibilityElementInfo from Accessibility,
    auto bResult = aacPtr->GetRoot(*(root.get()));
    if (bResult != Accessibility::RET_OK) {
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
        auto cResult = RecursGetChildElementInfo(root, newComponentNode_);
        if (!cResult) {
            return OHOS::ERR_INVALID_OPERATION;
        }
        RecursComponent(newComponentNode_);
    }
    // Generate Page Node
    newPageNode_ = std::make_shared<PageTree>();
    newPageNode_->SetNodeId();

    TRACK_LOG_END();
    return result;
}

void TreeManager::SetInputcomponentIndex(int actionType, uint32_t index)
{
    DEBUG_LOG_STR("Input: (%d), Input Type: (%d)", index, actionType);
    if (currentPageNode_ == nullptr) {
        ERROR_LOG("current page is null!");
        return;
    }

    // component input count statistics
    if (index != INVALIDED_INPUT_INDEX) {
        if (inputComponentList_.size() <= 0) {
            ERROR_LOG("input component list is null!");
            return;
        }
        if (index >= inputComponentList_.size()) {
            ERROR_LOG("index argument is invalided");
            return;
        }
        inputComponentList_[index]->AddInputCount();
        inputComponentList_[index]->AddTypeInputCount(actionType);
        DEBUG_LOG_STR("inputComponent: GetNodeId (0x%016llX)", inputComponentList_[index]->GetNodeId());
    } else {
        if (inputComponent_ == nullptr) {
            ERROR_LOG("inputComponent_ is nullptr");
            return;
        }
        inputComponent_->AddInputCount();
        inputComponent_->AddTypeInputCount(actionType);
        DEBUG_LOG_STR("inputComponent: GetNodeId (0x%016llX)", inputComponent_->GetNodeId());
    }
    RecursComponent(currentComponentNode_);
    // ability input count statistics
    currentAbilityNode_->AddInputCount();

    // page input count statistics
    currentPageNode_->AddInputCount();
    if (index != INVALIDED_INPUT_INDEX) {
        currentPageNode_->SetCurrentComponentNode(inputComponentList_[index]);
    } else {
        currentPageNode_->SetCurrentComponentNode(inputComponent_);
    }
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
    // Save one input pointer.
    if (inputComponent == nullptr) {
        ERROR_LOG("argument failed inputComponent is nullptr");
        return;
    }
    DEBUG_LOG_STR("Active component: GetNodeId (0x%016llX)", inputComponent->GetNodeId());
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

    uint32_t key = componentTreeList_.size();
    componentTreeList_[key] = currentComponentNode_;
    newPageNode_->SetIndex(key);
    if (!isNewAbility_) {
        if (currentPageNode_ != nullptr) {
            newPageNode_->SetParent(currentPageNode_);
            currentPageNode_->AddChild(newPageNode_);
        }
    } else {
        // ability tree growth
        key = pageTreeList_.size();
        pageTreeList_[key] = newPageNode_;
        currentAbilityNode_->SetIndex(key);
    }
    currentPageNode_ = newPageNode_;
    currentAbilityNode_->AddAllComponentCount(currentPageNode_->GetAllComponentCount());

    TRACK_LOG_END();
    return SamePage();
}

bool TreeManager::SamePage()
{
    TRACK_LOG_STD();
    isUpdateComponentFinished_ = true;
    newElementInfoList_.clear();
    if (WuKongLogger::GetInstance()->GetLogLevel() == LOG_LEVEL_TRACK) {
        DEBUG_LOG_STR("CompoentNode shared  new (%p) count = (%ld) unique (%d)", newComponentNode_.get(),
                      newComponentNode_.use_count(), newComponentNode_.unique());
    }
    newComponentNode_.reset();
    newPageNode_.reset();
    newAbilityNode_.reset();
    TRACK_LOG_END();
    return true;
}

bool TreeManager::UpdatePage(int layer, uint32_t index)
{
    TRACK_LOG_STD();
    DEBUG_LOG_STR("UpdatePage: layer (%d), index (%u)", layer, index);
    std::shared_ptr<WuKongTree> pageNode = currentPageNode_;
    if (layer > 0) {
        if (pageNode->GetChildren().size() <= index) {
            ERROR_LOG_STR("UpdatePage child index (%d) more than elementInfoList_ GetChildren() size (%u)", index,
                          pageNode->GetChildren().size());
            return false;
        }
        currentPageNode_ = std::static_pointer_cast<PageTree>(pageNode->GetChildren()[index]);
    } else {
        while (layer < 0) {
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

    if (componentTreeList_.find(currentPageNode_->GetIndex()) == componentTreeList_.end()) {
        ERROR_LOG_STR("currentPageNode_ index (%u) more than componentTreeList_ size (%u)",
                      currentPageNode_->GetIndex(), componentTreeList_.size());
        return false;
    }

    TRACK_LOG_STR("currentPageNode_->GetIndex(): %d", currentPageNode_->GetIndex());
    currentComponentNode_ = componentTreeList_[currentPageNode_->GetIndex()];

    if (!UpdateCurrentPage()) {
        return false;
    }
    TRACK_LOG_END();
    return SamePage();
}

bool TreeManager::RemovePage()
{
    TRACK_LOG_STD();
    uint32_t componentNodeIndex = currentPageNode_->GetIndex();
    uint32_t componentTreeListCount = componentTreeList_.size();
    if (componentNodeIndex >= componentTreeListCount) {
        ERROR_LOG_STR("currentPageNode index (%u) more than componentTreeList_ size (%u)", componentNodeIndex,
                      componentTreeListCount);
        return false;
    }
    if (WuKongLogger::GetInstance()->GetLogLevel() == LOG_LEVEL_TRACK) {
        DEBUG_LOG_STR("CompoentNode shared (%p) count = (%ld) unique (%d)",
                      componentTreeList_[componentNodeIndex].get(), componentTreeList_[componentNodeIndex].use_count(),
                      componentTreeList_[componentNodeIndex].unique());
    }
    auto componentNode = componentTreeList_[componentNodeIndex];
    if (componentNode == nullptr) {
        ERROR_LOG("componentNode point is nullptr of currentPageNode");
        return false;
    }
    uint32_t startIndex = componentNode->GetIndex();
    componentTreeList_[componentNodeIndex].reset();
    uint32_t endIndex = startIndex + currentPageNode_->count_ - 1;
    uint32_t elementInfoListCount = elementInfoList_.size();
    DEBUG_LOG_STR(
        "currentPageNode StartIndex (%u) EndIndex (%u) componentTreeList_ size (%u) elementInfoList_ size (%u)",
        startIndex, endIndex, componentTreeListCount, elementInfoListCount);
    if (startIndex >= elementInfoListCount || endIndex >= elementInfoListCount) {
        ERROR_LOG_STR("currentPageNode StartIndex (%u) EndIndex (%u) more than elementInfoList_ size (%u)", startIndex,
                      endIndex, elementInfoListCount);
        return false;
    }
    elementInfoList_.erase(elementInfoList_.begin() + startIndex, elementInfoList_.begin() + endIndex);
    TRACK_LOG_END();
    return true;
}

bool TreeManager::UpdateCurrentPage(bool isAdd)
{
    TRACK_LOG_STD();
    uint32_t count = elementInfoList_.size();
    DEBUG_LOG_STR("elementInfoList_: %d", count);
    for (auto elementInfo : newElementInfoList_) {
        elementInfoList_.push_back(elementInfo);
    }
    if (WuKongLogger::GetInstance()->GetLogLevel() == LOG_LEVEL_TRACK) {
        DEBUG_LOG_STR("CompoentNode shared  new (%p) count = (%ld) unique (%d)", newComponentNode_.get(),
                      newComponentNode_.use_count(), newComponentNode_.unique());
        DEBUG_LOG_STR("CompoentNode shared (%p) count = (%ld) unique (%d)", currentComponentNode_.get(),
                      currentComponentNode_.use_count(), currentComponentNode_.unique());
    }
    // update component tree index
    newComponentNode_->RecursUpdateNodeIndex(count);
    if (!isAdd) {
        newComponentNode_->RecursUpdateInfo(currentComponentNode_);
    }
    // set current sreen componentNode to new screen
    currentComponentNode_ = newComponentNode_;
    if (WuKongLogger::GetInstance()->GetLogLevel() == LOG_LEVEL_TRACK) {
        DEBUG_LOG_STR("CompoentNode shared (%p) count = (%ld) unique (%d)", currentComponentNode_.get(),
                      currentComponentNode_.use_count(), currentComponentNode_.unique());
        if (currentPageNode_ != nullptr) {
            DEBUG_LOG_STR("CompoentNode shared (%p) index (%u) count = (%ld) unique (%d)",
                          componentTreeList_[currentPageNode_->GetIndex()].get(), currentPageNode_->GetIndex(),
                          componentTreeList_[currentPageNode_->GetIndex()].use_count(),
                          componentTreeList_[currentPageNode_->GetIndex()].unique());
        }
    }

    if (!isAdd) {
        componentTreeList_[currentPageNode_->GetIndex()] = currentComponentNode_;
        currentAbilityNode_->allComponentCount_ -= currentPageNode_->GetAllComponentCount();
        currentAbilityNode_->allComponentCount_ += newPageNode_->GetAllComponentCount();
        currentPageNode_->nodeId_ = newPageNode_->nodeId_;
        currentPageNode_->allComponentCount_ = newPageNode_->allComponentCount_;
    }
    TRACK_LOG_END();
    return true;
}
}  // namespace WuKong
}  // namespace OHOS

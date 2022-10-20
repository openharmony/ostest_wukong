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

#include "scene_delegate.h"

#include "normal_scene.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
const float SAMEPERCENT = 0.8;
const int ONELAYER = 1;
const int ZEROLAYER = 0;
const float MINCOVERAGE = 0.9;
uint8_t LISTITEM_COUNT = 0;
uint8_t GRID_COUNT = 0;
uint8_t NUMBER_ZERO = 0;
uint8_t NUMBER_FOUR = 4;
uint8_t NUMBER_FIVE = 5;
}  // namespace
SceneDelegate::SceneDelegate()
{
}
SceneDelegate::~SceneDelegate()
{
}

ErrCode SceneDelegate::GetCurrentComponentInfo(std::shared_ptr<ComponentTree> componentinfo,
                                               std::vector<std::shared_ptr<ComponentTree>> &componentlist)
{
    ErrCode result = OHOS::ERR_OK;
    if (componentinfo == nullptr) {
        return OHOS::ERR_NO_INIT;
    }
    auto componentinfos = componentinfo->GetChildren();
    if (componentinfos.size() > 0) {
        bool isListItem = false;
        for (auto it : componentinfos) {
            auto componenttree = std::static_pointer_cast<ComponentTree>(it);
            if (componenttree->GetType() == "GridContainer") {
                componentlist.push_back(componenttree);
                componentType_.push_back("GridContainer");
                GRID_COUNT++;
                TRACK_LOG_STR("GridContainer count %u", GRID_COUNT);
            }
            if (componenttree->GetType() == "List") {
                LISTITEM_COUNT = NUMBER_ZERO;
            }
            if (componenttree->GetType() == "ListItem") {
                isListItem = true;
                LISTITEM_COUNT++;
            }
            if (isListItem && LISTITEM_COUNT > NUMBER_FOUR) {
                componenttree = std::static_pointer_cast<ComponentTree>(componentinfos[componentinfos.size() - 1]);
            }
            GetCurrentComponentInfo(componenttree, componentlist);
            if (isListItem && LISTITEM_COUNT >= NUMBER_FIVE) {
                break;
            }
        }
    } else if (GRID_COUNT <= componentlist.size() &&
               std::static_pointer_cast<ComponentTree>(componentinfo)->IsVisible() &&
               IsComponentInScreen(std::static_pointer_cast<ComponentTree>(componentinfo))) {
        componentlist.emplace(componentlist.end() - GRID_COUNT, componentinfo);
        componentType_.push_back(std::static_pointer_cast<ComponentTree>(componentinfo)->GetType());
    }
    return result;
}

ErrCode SceneDelegate::ChooseScene(bool isRandom)
{
    ErrCode result;
    GRID_COUNT = 0;
    componentType_.clear();
    auto treemanager = TreeManager::GetInstance();
    auto newpage = treemanager->GetNewPage();
    if (newpage == nullptr) {
        ERROR_LOG("newpage is nullptr");
        return OHOS::ERR_NO_INIT;
    }
    auto newcomponents = treemanager->GetNewComponents();
    if (newcomponents == nullptr) {
        ERROR_LOG("newcomponents is nullptr");
        return OHOS::ERR_NO_INIT;
    }
    std::vector<std::shared_ptr<ComponentTree>> allcomponentlist;
    // get current component list
    GetCurrentComponentInfo(newcomponents, allcomponentlist);
    // set all component counts of new page
    newpage->SetAllComponentCount(allcomponentlist.size());
    // set valid component counts of new page
    newpage->SetValidComponentCount(allcomponentlist.size());
    // get current page node
    std::shared_ptr<WuKongTree> currentpage = treemanager->GetCurrentPage();
    if (currentpage == nullptr) {
        DEBUG_LOG("first page");
        treemanager->AddPage();
        // set available component list of current page
        result = SetAvailableComponentList(newcomponents, isRandom);
        TRACK_LOG_STR("new component Node Id: %016llX", newcomponents->GetNodeId());
        return result;
    }
    DEBUG_LOG_STR("new ID: %016llX ,old ID: %016llX", newpage->GetNodeId(), currentpage->GetNodeId());
    auto currentcomponents = treemanager->GetCurrentComponents();
    if (currentcomponents == nullptr) {
        ERROR_LOG("currentcomponents is nullptr");
        return OHOS::ERR_NO_INIT;
    }
    if (newpage->IsEqual(currentpage)) {
        treemanager->SamePage();
        DEBUG_LOG("at same page");
        result = SetAvailableComponentList(currentcomponents, isRandom);
        return result;
    } else {
        bool isFoundParent = false;
        // find the same page in parent list
        result = FindSamePageInParent(isFoundParent, isRandom);
        if (result != OHOS::ERR_OK || isFoundParent) {
            return result;
        }
        bool isFoundChildren = false;
        // find the same page in chidren list
        result = FindSamePageInChildren(isFoundChildren, isRandom);
        if (result != OHOS::ERR_OK) {
            return result;
        }
        if (!isFoundChildren) {
            auto currentcomponentinfo = treemanager->GetCurrentComponents();
            if (currentcomponentinfo == nullptr) {
                ERROR_LOG("currentcomponentinfo is nullptr");
                return OHOS::ERR_NO_INIT;
            }
            // compare new component tree and current component tree
            CompareComponentInfos(newcomponents, currentcomponentinfo, isRandom);
        }
    }
    return result;
}

ErrCode SceneDelegate::CompareComponentInfos(std::shared_ptr<ComponentTree> &newcomponentinfo,
                                             std::shared_ptr<ComponentTree> &oldcomponentinfo, bool isRandom)
{
    ErrCode result;
    DEBUG_LOG("compare page");
    GRID_COUNT = 0;
    componentType_.clear();
    std::vector<std::shared_ptr<ComponentTree>> newChildList;
    GetCurrentComponentInfo(newcomponentinfo, newChildList);
    GRID_COUNT = 0;
    componentType_.clear();
    std::vector<std::shared_ptr<ComponentTree>> currentChildList;
    GetCurrentComponentInfo(oldcomponentinfo, currentChildList);
    auto treemanager = TreeManager::GetInstance();
    DEBUG_LOG_STR("childlist size %d", currentChildList.size());
    float samePercent = 0.0;
    // get the same count in new component list and current component list
    uint32_t samecount = FindSame(newChildList, currentChildList);
    if (newChildList.size() > currentChildList.size()) {
        samePercent = (float)samecount / (float)currentChildList.size();
    } else {
        samePercent = (float)samecount / (float)newChildList.size();
    }

    DEBUG_LOG_STR("same percent: %2f", samePercent);
    if (samePercent > SAMEPERCENT) {
        if (!treemanager->UpdatePage(ZEROLAYER)) {
            DEBUG_LOG("update failed");
            return OHOS::ERR_NO_INIT;
        }
        auto currentComponentinfo = treemanager->GetCurrentComponents();
        if (currentComponentinfo == nullptr) {
            ERROR_LOG("current component is nullptr");
            return OHOS::ERR_NO_INIT;
        }
        result = SetAvailableComponentList(currentComponentinfo, isRandom);
    } else {
        auto newcomponent = treemanager->GetNewComponents();
        DEBUG_LOG("add new page");
        treemanager->AddPage();
        result = SetAvailableComponentList(newcomponent, isRandom);
    }
    return result;
}

ErrCode SceneDelegate::SetAvailableComponentList(std::shared_ptr<ComponentTree> componentinfo, bool isRandom)
{
    GRID_COUNT = 0;
    componentType_.clear();
    ErrCode result = OHOS::ERR_OK;
    NormalScene normalscene;
    std::vector<std::shared_ptr<ComponentTree>> componentlist;
    std::shared_ptr<ComponentTree> inputcomponent = nullptr;
    auto treemanager = TreeManager::GetInstance();
    GetCurrentComponentInfo(componentinfo, componentlist);
    if (isRandom) {
        // get valid components from scene
        normalscene.SetInputComponentList(componentlist);
        isBack_ = normalscene.IsBackToPrePage();
        TRACK_LOG_STR("is random back: %d", isBack_);
        // set valid components to tree manager
        treemanager->SetActiveComponent(componentlist);
    } else {
        // get valid component from scene
        normalscene.SetInputComponent(componentlist, inputcomponent);
        isBack_ = normalscene.IsBackToPrePage();
        TRACK_LOG_STR("is special back: %d", isBack_);
        if (inputcomponent != nullptr) {
            // set valid component to tree manager
            treemanager->SetActiveComponent(inputcomponent);
        }
    }
    return result;
}

uint32_t SceneDelegate::FindSame(const std::vector<std::shared_ptr<ComponentTree>> &newcomponentlist,
                                 const std::vector<std::shared_ptr<ComponentTree>> &oldcomponentlist)
{
    uint32_t count = 0;
    for (auto newIt : newcomponentlist) {
        for (auto oldIt : oldcomponentlist) {
            if (newIt->IsEqual(oldIt)) {
                count++;
            }
        }
    }
    return count;
}

ErrCode SceneDelegate::FindSamePageInChildren(bool &isFound, bool isRandom)
{
    ErrCode result = OHOS::ERR_OK;
    auto treemanager = TreeManager::GetInstance();
    auto newpage = treemanager->GetNewPage();
    std::shared_ptr<WuKongTree> currentpage = treemanager->GetCurrentPage();
    auto pagechild = currentpage->GetChildren();
    if (pagechild.empty()) {
        return result;
    }
    int childIndex = -1;
    for (auto it : pagechild) {
        TRACK_LOG_STR("current child ID: %016llX ", it->GetNodeId());
        childIndex++;
        if (newpage->IsEqual(it)) {
            DEBUG_LOG("go to same page");
            if (!treemanager->UpdatePage(ONELAYER, childIndex)) {
                DEBUG_LOG("update failed");
                return OHOS::ERR_NO_INIT;
            }
            auto gotocurrent = treemanager->GetCurrentComponents();
            if (gotocurrent == nullptr) {
                ERROR_LOG("goto current is nullptr");
                return OHOS::ERR_NO_INIT;
            }
            result = SetAvailableComponentList(gotocurrent, isRandom);
            isFound = true;
            return result;
        }
    }
    return result;
}

ErrCode SceneDelegate::FindSamePageInParent(bool &isFound, bool isRandom)
{
    ErrCode result = OHOS::ERR_OK;
    auto treemanager = TreeManager::GetInstance();
    auto newpage = treemanager->GetNewPage();
    std::shared_ptr<WuKongTree> currentpage = treemanager->GetCurrentPage();
    int layer = 0;
    auto parentpage = currentpage->GetParent();
    while (parentpage != nullptr) {
        TRACK_LOG_STR("current parent ID: %016llX ", parentpage->GetNodeId());
        layer--;
        if (newpage->IsEqual(parentpage)) {
            auto oldpage = treemanager->GetCurrentPage();
            if (oldpage == nullptr) {
                ERROR_LOG("old page is nullptr");
                return OHOS::ERR_NO_INIT;
            }
            float coverage = (float)oldpage->GetInputCount() / (float)oldpage->GetValidComponentCount();

            TRACK_LOG_STR("layer: (%d)", layer);
            if (!treemanager->UpdatePage(layer)) {
                DEBUG_LOG("update failed");
                return OHOS::ERR_NO_INIT;
            }
            if (coverage < MINCOVERAGE && !isRandom) {
                DEBUG_LOG("continue to same page");
                treemanager->SetActiveComponent(
                    std::static_pointer_cast<PageTree>(parentpage)->GetCurrentComponentNode());
            } else {
                DEBUG_LOG("back to same page");
                result = SetAvailableComponentList(treemanager->GetCurrentComponents(), isRandom);
            }
            isFound = true;
            return result;
        }
        parentpage = parentpage->GetParent();
    }
    return result;
}

bool SceneDelegate::IsComponentInScreen(const std::shared_ptr<ComponentTree> componentinfo)
{
    int32_t width = -1;
    int32_t height = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(width, height);
    if (result != OHOS::ERR_OK) {
        ERROR_LOG("failed to determine component position");
        return false;
    }
    auto rect = componentinfo->GetPosition();
    if ((rect.GetRightBottomXScreenPostion() <= width) && (rect.GetLeftTopXScreenPostion() <= width) &&
        (rect.GetRightBottomYScreenPostion() <= height) && (rect.GetLeftTopYScreenPostion() <= height) &&
        (rect.GetRightBottomXScreenPostion() > 0) && (rect.GetLeftTopXScreenPostion() > 0) &&
        (rect.GetRightBottomYScreenPostion() > 0) && (rect.GetLeftTopYScreenPostion() > 0)) {
        return true;
    }
    return false;
}
}  // namespace WuKong
}  // namespace OHOS

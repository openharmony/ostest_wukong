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

namespace OHOS {
namespace WuKong {
namespace {
const float SAMEPERCENT = 0.8;
const int ONELAYER = 1;
const int ZEROLAYER = 0;
const std::vector<std::string> componentType = {"Row", "input", "Column", "ListItem", "TextInput", "Toggle", "Button"};
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
        for (auto it : componentinfos) {
            auto componenttree = std::static_pointer_cast<ComponentTree>(it);
            GetCurrentComponentInfo(componenttree, componentlist);
        }
    } else {
        componentlist.push_back(componentinfo);
    }
    return result;
}

ErrCode SceneDelegate::ChooseScene(bool isRandom)
{
    ErrCode result = OHOS::ERR_OK;
    int layer = 0;
    auto treemanager = TreeManager::GetInstance();
    auto newpage = treemanager->GetNewPage();
    std::shared_ptr<WuKongTree> currentpage = treemanager->GetCurrentPage();
    auto newcomponents = treemanager->GetNewComponents();
    if (currentpage == nullptr) {
        DEBUG_LOG("first page");
        treemanager->AddPage();
        result = SetAvailableComponentList(newcomponents, isRandom);
        return result;
    }
    DEBUG_LOG_STR("new ID: %lld ,old ID: %lld", newpage->GetNodeId(), currentpage->GetNodeId());
    auto currentcomponents = treemanager->GetCurrentComponents();
    if (newpage->IsEqual(currentpage)) {
        treemanager->SamePage();
        DEBUG_LOG("at same page");
        result = SetAvailableComponentList(currentcomponents, isRandom);
        return result;
    } else {
        while (currentpage->GetParent() != nullptr) {
            currentpage = currentpage->GetParent();
            layer--;
            if (newpage->IsEqual(currentpage)) {
                DEBUG_LOG_STR("layer: (%d)", layer);
                treemanager->UpdatePage(layer);
                DEBUG_LOG("back to same page");
                auto exsitcomponent = treemanager->GetCurrentComponents();
                result = SetAvailableComponentList(exsitcomponent, isRandom);
                return result;
            }
        }
        std::vector<std::shared_ptr<WuKongTree>> pagechild;
        if (!pagechild.empty()) {
            int childIndex = 0;
            for (auto it : pagechild) {
                childIndex++;
                if (newpage->IsEqual(it)) {
                    auto currentComponentinfo = treemanager->GetCurrentComponents();
                    DEBUG_LOG("go to same page");
                    treemanager->UpdatePage(ONELAYER, childIndex);
                    result = SetAvailableComponentList(currentComponentinfo, isRandom);
                    return result;
                }
            }
        }
        CompareComponentInfos(newcomponents, currentcomponents, isRandom);
    }
    return result;
}

ErrCode SceneDelegate::CompareComponentInfos(std::shared_ptr<ComponentTree> newcomponentinfo,
                                             std::shared_ptr<ComponentTree> oldcomponentinfo, bool isRandom)
{
    ErrCode result = OHOS::ERR_OK;
    DEBUG_LOG("compare page");
    std::vector<std::shared_ptr<ComponentTree>> newChildList;
    GetCurrentComponentInfo(newcomponentinfo, newChildList);
    std::vector<std::shared_ptr<ComponentTree>> currentChildList;
    GetCurrentComponentInfo(oldcomponentinfo, currentChildList);
    auto treemanager = TreeManager::GetInstance();
    if (newChildList.size() < 0 || currentChildList.size() < 0) {
        return OHOS::ERR_NO_INIT;
    }
    DEBUG_LOG_STR("childlist size %d", currentChildList.size());
    float samePercent = (float)((FindSame(newChildList, currentChildList)) / (float)(currentChildList.size()));

    DEBUG_LOG_STR("same percent: %2f", samePercent);
    if (samePercent > SAMEPERCENT) {
        auto currentComponentinfo = treemanager->GetCurrentComponents();
        treemanager->UpdatePage(ZEROLAYER);
        result = SetAvailableComponentList(currentComponentinfo, isRandom);
    } else {
        DEBUG_LOG("add new page");
        auto newComponentinfo = treemanager->GetNewComponents();
        treemanager->AddPage();
        result = SetAvailableComponentList(newComponentinfo, isRandom);
    }
    return result;
}

ErrCode SceneDelegate::SetAvailableComponentList(std::shared_ptr<ComponentTree> componentinfo, bool isRandom)
{
    ErrCode result = OHOS::ERR_OK;
    NormalScene normalsecen;
    std::vector<std::shared_ptr<ComponentTree>> componentlist;
    std::shared_ptr<ComponentTree> inputcomponent = nullptr;
    auto treemanager = TreeManager::GetInstance();
    GetCurrentComponentInfo(componentinfo, componentlist);
    if (isRandom) {
        normalsecen.SetInputComponentList(componentlist);
        isBack_ = normalsecen.IsBackToPrePage();
        TRACK_LOG_STR("is random back: %d", isBack_);
        treemanager->SetActiveComponent(componentlist);
    } else {
        normalsecen.SetInputComponent(componentlist, inputcomponent);
        isBack_ = normalsecen.IsBackToPrePage();
        TRACK_LOG_STR("is special back: %d", isBack_);
        if (inputcomponent != nullptr) {
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
}  // namespace WuKong
}  // namespace OHOS

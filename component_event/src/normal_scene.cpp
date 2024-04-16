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

#include "normal_scene.h"

#include "tree_manager.h"

namespace OHOS {
namespace WuKong {
namespace {
const int MAXINPUTNUM = 3;
const int ONEHUNDRED = 100;
const int NEWPERCENT = 75;
const int OLDPERCENT = 20;
}  // namespace
NormalScene::NormalScene()
{
}

NormalScene::~NormalScene()
{
}
ErrCode NormalScene::SetInputComponentList(std::vector<std::shared_ptr<ComponentTree>> &componentList)
{
    ErrCode result = OHOS::ERR_OK;
    int randomNumber = rand() % ONEHUNDRED;
    uint32_t count = 0;
    DEBUG_LOG_STR("randomNumber: %d", randomNumber);
    std::vector<uint32_t> indexList;
    auto util = WuKongUtil::GetInstance();
    std::vector<std::string> compIdBlock = util->GetCompIdBlockList();
    std::vector<std::string> compTypeBlock = util->GetCompTypeBlockList();
    if (randomNumber < NEWPERCENT) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            TRACK_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
            if ((*it)->GetInputCount() > MAXINPUTNUM || IsComponentBlock((*it), compIdBlock, compTypeBlock)) {
                indexList.push_back((*it)->GetIndex());
                TRACK_LOG_STR("index0: %d", distance(componentList.begin(), it));
            }
        }
    } else if (randomNumber < (NEWPERCENT + OLDPERCENT)) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            TRACK_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
            if ((*it)->GetInputCount() <= MAXINPUTNUM || IsComponentBlock((*it), compIdBlock, compTypeBlock)) {
                count++;
                TRACK_LOG_STR("inputed count: %d, componentList size: %d", count, componentList.size());
                indexList.push_back((*it)->GetIndex());
                TRACK_LOG_STR("index: %d", distance(componentList.begin(), it));
            }
        }
    }
    if (count >= componentList.size()) {
        if ((componentList.size() == 0) || (randomNumber < ONEHUNDRED && randomNumber >= (NEWPERCENT + OLDPERCENT))) {
            isBack_ = true;
        } else {
            SubtractComponent(componentList, indexList);
        }
        indexList.clear();
        return OHOS::ERR_OK;
    }
    TRACK_LOG_STR("componentList size: %d", componentList.size());
    TRACK_LOG_STR("indexList size: %d", indexList.size());
    SubtractComponent(componentList, indexList);
    if ((componentList.size() == 0) || (randomNumber < ONEHUNDRED && randomNumber >= (NEWPERCENT + OLDPERCENT))) {
        isBack_ = true;
    }
    indexList.clear();
    return result;
}

ErrCode NormalScene::SetInputComponent(std::vector<std::shared_ptr<ComponentTree>> &componentList,
                                       std::shared_ptr<ComponentTree> &componentinfo)
{
    auto util = WuKongUtil::GetInstance();
    std::vector<std::string> compIdBlock = util->GetCompIdBlockList();
    std::vector<std::string> compTypeBlock = util->GetCompTypeBlockList();
    ErrCode result = OHOS::ERR_OK;
    if (!componentList.empty()) {
        auto treemanager = TreeManager::GetInstance();
        bool hasDialog = treemanager->HasDialog();
        bool meetDialog = false;
        for (auto it : componentList) {
            if (it->GetType() == "Dialog") {
                meetDialog = true;
            }
            if (hasDialog && !meetDialog) {
                continue;
            }
            DEBUG_LOG_STR("component inputcount: %d, node id: %016llX", it->GetInputCount(), it->GetNodeId());
            if (it->GetInputCount() < 1 && !IsComponentBlock(it, compIdBlock, compTypeBlock)) {
                componentinfo = it;
                break;
            }
        }
    }
    if (componentinfo == nullptr) {
        isBack_ = true;
    }
    return result;
}

ErrCode NormalScene::SetInputComponentListForFocusInput(std::vector<std::shared_ptr<ComponentTree>> &componentList)
{
    ErrCode result = OHOS::ERR_OK;
    uint32_t count = 0;
    std::vector<uint32_t> indexList;
    auto treemanager = TreeManager::GetInstance();
    uint32_t focusNum = treemanager->GetFocusNum();
    auto util = WuKongUtil::GetInstance();
    std::vector<std::string> compIdBlock = util->GetCompIdBlockList();
    std::vector<std::string> compTypeBlock = util->GetCompTypeBlockList();
    for (auto it = componentList.begin(); it != componentList.end(); it++) {
        TRACK_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
        std::string type = (*it)->GetType();
        bool needFocus = treemanager->NeedFocus(type);
        uint32_t limit = needFocus ? focusNum : MAXINPUTNUM;
        if ((*it)->GetInputCount() >= limit || IsComponentBlock((*it), compIdBlock, compTypeBlock)) {
            indexList.push_back((*it)->GetIndex());
            count++;
            TRACK_LOG_STR("index0: %d", distance(componentList.begin(), it));
        }
    }
    if (count >= componentList.size()) {
        isBack_ = true;
        indexList.clear();
        return OHOS::ERR_OK;
    }
    // just determine back or not, can not remove component because of locating component after
    if (componentList.size() == 0) {
        isBack_ = true;
    }
    indexList.clear();
    return result;
}

void NormalScene::SubtractComponent(std::vector<std::shared_ptr<ComponentTree>> &componentList,
    std::vector<uint32_t> &indexList)
{
    for (auto index : indexList) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            if ((*it)->GetIndex() == index) {
                componentList.erase(it);
                it--;
            }
        }
    }
}

bool NormalScene::IsComponentBlock(std::shared_ptr<ComponentTree> &comp, std::vector<std::string> &compIdBlock,
    std::vector<std::string> &compTypeBlock)
{
    std::string id = comp->GetInspectorKey();
    std::string type = comp->GetType();
    auto util = WuKongUtil::GetInstance();
    if (util->ContainsElement(compIdBlock, id)) {
        DEBUG_LOG_STR("Block Id: %s", id.c_str());
        return true;
    }
    if (util->ContainsElement(compTypeBlock, type)) {
        DEBUG_LOG_STR("Block type: %s", type.c_str());
        return true;
    }
    DEBUG_LOG_STR("UnBlock Id: %s, type: %s", id.c_str(), type.c_str());
    return false;
}
}  // namespace WuKong
}  // namespace OHOS

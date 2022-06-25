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

namespace OHOS {
namespace WuKong {
namespace {
const int MAXINPUTNUM = 2;
const int ONEHUNDRED = 100;
const int NEWPERCENT = 70;
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
    if (randomNumber < NEWPERCENT) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            DEBUG_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
            if ((*it)->GetInputCount() > MAXINPUTNUM) {
                indexList.push_back((*it)->GetIndex());
                TRACK_LOG_STR("index0: %d", distance(componentList.begin(), it));
            }
        }
    } else if (randomNumber < (NEWPERCENT + OLDPERCENT)) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            DEBUG_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
            if ((*it)->GetInputCount() <= MAXINPUTNUM) {
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
        }
        indexList.clear();
        return OHOS::ERR_OK;
    }
    TRACK_LOG_STR("componentList size: %d", componentList.size());
    TRACK_LOG_STR("indexList size: %d", indexList.size());
    for (auto index : indexList) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            if ((*it)->GetIndex() == index) {
                componentList.erase(it);
                it--;
            }
        }
    }
    if ((componentList.size() == 0) || (randomNumber < ONEHUNDRED && randomNumber >= (NEWPERCENT + OLDPERCENT))) {
        isBack_ = true;
    }
    indexList.clear();
    return result;
}

ErrCode NormalScene::SetInputComponent(std::vector<std::shared_ptr<ComponentTree>> &componentList,
                                       std::shared_ptr<ComponentTree> &componentinfo)
{
    ErrCode result = OHOS::ERR_OK;
    if (!componentList.empty()) {
        for (auto it : componentList) {
            DEBUG_LOG_STR("component inputcount: %d, node id: %016llX", it->GetInputCount(), it->GetNodeId());
            if (it->GetInputCount() < 1) {
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
}  // namespace WuKong
}  // namespace OHOS

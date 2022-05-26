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
    DEBUG_LOG_STR("randomNumber: %d", randomNumber);
    if (randomNumber < NEWPERCENT) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            DEBUG_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
            if ((*it)->GetInputCount() > MAXINPUTNUM) {
                componentList.erase(it);
                it--;
            }
        }
    } else if (randomNumber < (NEWPERCENT + OLDPERCENT)) {
        for (auto it = componentList.begin(); it != componentList.end(); it++) {
            DEBUG_LOG_STR("component inputcount: %d", (*it)->GetInputCount());
            if ((*it)->GetInputCount() <= MAXINPUTNUM) {
                componentList.erase(it);
                it--;
            }
        }
    } else if ((componentList.size() == 0) || randomNumber < ONEHUNDRED) {
        isBack_ = true;
    }
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

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
#include <map>
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

#include "collapse_input.h"
#include "input_action.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
CollapseInput::CollapseInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_COLLAPSEINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

CollapseInput::~CollapseInput()
{}

ErrCode CollapseInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    std::string command = "hidumper -s 4607 -a '-a' | grep FoldStatus | cut -d''";
    // 先获取当前状态
    std::string currentStatus = WuKongUtil::GetInstance()->ExecuteCmd(command);
    if (currentStatus == StatusToDesc.at(CollapseStatus::C_UNKNOWN)) {
        ERROR_LOG("Only foldable products support this command.");
        return OHOS::ERR_OK;
    }

    // 获取所有键
    std::vector<int> keys;
    for (const auto &pair : StatusToDesc) {
        keys.push_back(pair.first);
    }

    // 随机打乱顺序
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(keys.begin(), keys.end(), gen);

    // 选择第一个元素
    int randomKey = keys[0];
    std::string dstStatus = StatusToDesc.at(randomKey);
    if (dstStatus == StatusToDesc.at(CollapseStatus::C_UNKNOWN)) {
        ERROR_LOG("Does not support switch to status : UNKNOWN.");
        return OHOS::ERR_OK;
    }

    if (currentStatus == dstStatus) {
        INFO_LOG("The source state and target state are the same; no state transition is required.");
        return OHOS::ERR_OK;
    }

    std::string foldScreenType = WuKongUtil::GetInstance()->GetFoldScreenType();
    result = MultimodeManager::GetInstance()->CollapseSimulate(currentStatus, dstStatus, foldScreenType);

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

InputType CollapseInput::GetInputInfo()
{
    return INPUTTYPE_COLLAPSEINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

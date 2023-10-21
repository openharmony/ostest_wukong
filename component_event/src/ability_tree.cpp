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

#include "ability_tree.h"

#include "ability_manager_client.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint8_t ABILITY_BUNDLE_NAME_SIZE_POSION = 56;
const uint8_t ABILITY_BUNDLE_NAME_POSION = 32;
const uint8_t ABILITY_ABILITY_NAME_SIZE_POSION = 24;
const uint8_t ABILITY_ABILITY_NAME_POSION = 0;
}  // namespace

/**
 * @brief Ability Node Id specification format
 * @details
 * |----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
 * |BUN SIZ  |BUN STR                      |ABI SIZ  |ABI STR                      |
 */
bool AbilityTree::SetNodeId()
{
    auto amcPtr = AAFwk::AbilityManagerClient::GetInstance();
    if (amcPtr == nullptr) {
        ERROR_LOG("AbilityManagerClient is nullptr");
        return false;
    }
    auto elementName = amcPtr->GetTopAbility();
    if (elementName.GetBundleName().empty()) {
        ERROR_LOG("GetTopAbility GetBundleName is nullptr");
        return false;
    }
    const uint8_t nameCount = 3;
    // make bundle name value
    bundleName_ = elementName.GetBundleName();
    uint64_t nameSize = bundleName_.size();
    nodeId_ |= nameSize << ABILITY_BUNDLE_NAME_SIZE_POSION;
    nodeId_ |= GetSubName(bundleName_, nameCount) << ABILITY_BUNDLE_NAME_POSION;

    // make ability name value
    abilityName_ = elementName.GetAbilityName();
    nameSize = abilityName_.size();
    nodeId_ |= nameSize << ABILITY_ABILITY_NAME_SIZE_POSION;
    nodeId_ |= GetSubName(abilityName_, nameCount) << ABILITY_ABILITY_NAME_POSION;

    TRACK_LOG_STR("Ability Node ID: (0x%016llX)", nodeId_);
    return true;
}
}  // namespace WuKong
}  // namespace OHOS

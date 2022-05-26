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
const uint8_t ABILITY_BUNDLE_NAME_POSION = 48;
const uint8_t ABILITY_ABILITY_NAME_SIZE_POSION = 40;
const uint8_t ABILITY_ABILITY_NAME_POSION = 32;
const uint8_t ABILITY_MODULE_NAME_SIZE_POSION = 24;
const uint8_t ABILITY_MODULE_NAME_POSION = 16;
const uint8_t ABILITY_DEVICE_ID_NAME_SIZE_POSION = 8;
const uint8_t ABILITY_DEVICE_ID_NAME_POSION = 0;
uint64_t GetSubName(std::string name)
{
    TRACK_LOG_STR("name %s", name.c_str());
    uint64_t subName = 0;
    uint32_t nameSize = name.size();
    if (nameSize > 0) {
        subName = name[nameSize - 1];
    } else {
        subName = 0;
    }
    return subName;
}
}  // namespace

/**
 * @brief Ability Node Id specification format
 * @details
 * |----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
 * |BUN SIZ  |BUN STR  |ABI SIZ  |ABI STR  |MOD SIZ  |MOD STR  |DEV SIZ  |DEV STR  |
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
    // make bundle name value
    bundleName_ = elementName.GetBundleName();
    uint64_t nameSize = bundleName_.size();
    nodeId_ |= nameSize << ABILITY_BUNDLE_NAME_SIZE_POSION;
    nodeId_ |= GetSubName(bundleName_) << ABILITY_BUNDLE_NAME_POSION;

    // make ability name value
    abilityName_ = elementName.GetAbilityName();
    nameSize = abilityName_.size();
    nodeId_ |= nameSize << ABILITY_ABILITY_NAME_SIZE_POSION;
    nodeId_ |= GetSubName(abilityName_) << ABILITY_ABILITY_NAME_POSION;

    // make ability name value
    moduleName_ = elementName.GetModuleName();
    nameSize = moduleName_.size();
    nodeId_ |= nameSize << ABILITY_MODULE_NAME_SIZE_POSION;
    nodeId_ |= GetSubName(moduleName_) << ABILITY_MODULE_NAME_POSION;

    // make device id name value
    deviceId_ = elementName.GetDeviceID();
    nameSize = deviceId_.size();
    nodeId_ |= nameSize << ABILITY_DEVICE_ID_NAME_SIZE_POSION;
    nodeId_ |= GetSubName(deviceId_) << ABILITY_DEVICE_ID_NAME_POSION;

    TRACK_LOG_STR("Ability Node ID: (0x%016llX)", nodeId_);
    return true;
}
}  // namespace WuKong
}  // namespace OHOS

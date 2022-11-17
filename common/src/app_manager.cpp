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

#include "app_manager.h"

#include "ability_manager_client.h"
#include "element_name.h"
#include "wukong_define.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string HELP_MSG_NO_ABILITY_NAME_OPTION = "error: -a <ability-name> is expected";
const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION = "error: -b <bundle-name> is expected";

const std::string STRING_START_ABILITY_OK = "start ability successfully.";
const std::string STRING_START_ABILITY_NG = "error: failed to start ability.";
}  // namespace

bool AppManager::BlockAbilityController::AllowAbilityStart(const AAFwk::Want &want, const std::string &bundleName)
{
    TRACK_LOG_STD();
    std::vector<std::string> blocklist;
    std::vector<std::string> tempAllowList;
    bool orderFlag;
    auto util = WuKongUtil::GetInstance();
    
    tempAllowList = util->GetTempAllowList();
    // if bundleName in the tempAllow list to allow ability start.
    auto it = find(tempAllowList.begin(), tempAllowList.end(), bundleName);
    orderFlag = util->GetOrderFlag();
    if (orderFlag && tempAllowList.size() != 0) {
        if (it != tempAllowList.end()) {
            DEBUG_LOG("bundle start allow");
            return true;
        } else {
            return false;
        }
    }

    util->GetBlockList(blocklist);
    DEBUG_LOG_STR("BundleName: %s", bundleName.c_str());

    // if bundleName in the block list to unallow ability start.
    it = find(blocklist.begin(), blocklist.end(), bundleName);
    if (it == blocklist.end()) {
        DEBUG_LOG("bundle start prohibition");
        return true;
    }

    TRACK_LOG_END();
    return false;
}

// turn to background
bool AppManager::BlockAbilityController::AllowAbilityBackground(const std::string &bundleName)
{
    if (WuKongUtil::GetInstance()->GetOrderFlag()) {
        return false;
    } else {
        return true;
    }
}

ErrCode AppManager::StartAbilityByBundleInfo(std::string abilityName, std::string bundleName)
{
    TRACK_LOG_STD();
    AAFwk::Want want;
    int result;
    std::string output;
    if (abilityName.size() == 0 || bundleName.size() == 0) {
        if (abilityName.size() == 0) {
            output.append(HELP_MSG_NO_ABILITY_NAME_OPTION + "\n");
        }
        if (bundleName.size() == 0) {
            output.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
        }
        result = OHOS::ERR_INVALID_VALUE;
    } else {
        AppExecFwk::ElementName element("", bundleName, abilityName);
        want.SetElement(element);
        result = OHOS::AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
        if (result == OHOS::ERR_OK) {
            output = STRING_START_ABILITY_OK;
        } else {
            output = STRING_START_ABILITY_NG;
        }
    }
    DEBUG_LOG_STR("%s", output.c_str());
    TRACK_LOG_STR("result %s", std::to_string(result).c_str());
    return result;
}

void AppManager::SetAbilityController()
{
    if (abilityController_ == nullptr) {
        abilityController_ = new (std::nothrow) BlockAbilityController();
    }
    OHOS::AAFwk::AbilityManagerClient::GetInstance()->SetAbilityController(abilityController_, true);
}
}  // namespace WuKong
}  // namespace OHOS

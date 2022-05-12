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

#include "appswitch_input.h"
#include "input_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
AppswitchInput::AppswitchInput() : InputAction() {}

AppswitchInput::~AppswitchInput() {}

ErrCode AppswitchInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<std::string> bundleList;
    std::vector<std::string> abilityList;
    int index = -1;
    Util::GetInstance()->GetBundleList(bundleList, abilityList);
    index = GetAbilityIndex(bundleList);
    // start ability through bundle information
    result = AppManager::GetInstance()->StartAbilityByBundleInfo(abilityList[index], bundleList[index]);
    // print the result of start event
    PrintResultOfStartAbility(result, index);

    return result;
}

ErrCode AppswitchInput::PrintResultOfStartAbility(const ErrCode result, int index)
{
    std::vector<std::string> bundleList;
    std::vector<std::string> abilityList;
    Util::GetInstance()->GetBundleList(bundleList, abilityList);
    if (result == OHOS::ERR_OK) {
        INFO_LOG_STR("Bundle Name: (%s) Startup SUCCESSFUL", bundleList[index].c_str());
    } else {
        INFO_LOG_STR("Bundle Name: (%s) Startup FAILED", bundleList[index].c_str());
    }
    return OHOS::ERR_OK;
}

ErrCode AppswitchInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}

int AppswitchInput::GetAbilityIndex(std::vector<std::string> &bundlelist)
{
    int index = -1;
    std::vector<std::string> whitelist;
    std::vector<std::string> validlist;
    Util::GetInstance()->GetWhiteList(whitelist);
    Util::GetInstance()->GetValidBundleList(validlist);
    if (bundlelist.size() > 0) {
        if (whitelist.size() > 0) {
            index = Util::GetInstance()->FindElement(bundlelist, whitelist.at(rand() % whitelist.size()));
        } else if (validlist.size() > 0) {
            index = Util::GetInstance()->FindElement(bundlelist, validlist.at(rand() % validlist.size()));
        } else {
            index = rand() % bundlelist.size();
        }
    }
    return index;
}
}  // namespace WuKong
}  // namespace OHOS

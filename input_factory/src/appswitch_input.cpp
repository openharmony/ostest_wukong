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
#include "report.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
AppswitchInput::AppswitchInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_APPSWITCHINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}

AppswitchInput::~AppswitchInput()
{
}

ErrCode AppswitchInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    AppSwitchParam* appSwitchPtr = static_cast<AppSwitchParam*>(specialTestObject.get());
    if (appSwitchPtr == nullptr) {
        return OHOS::ERR_INVALID_VALUE;
    }
    std::string bundlename = appSwitchPtr->bundlename_;
    std::vector<std::string> bundleList(0);
    std::vector<std::string> abilityList(0);
    auto util = WuKongUtil::GetInstance();
    util->GetBundleList(bundleList, abilityList);
    if (bundleList.size() == 0 || abilityList.size() == 0) {
        ERROR_LOG_STR("bundleList (%u) or abilityList (%u) is 0", bundleList.size(), abilityList.size());
        return OHOS::ERR_INVALID_VALUE;
    }
    uint32_t index = util->FindElement(bundleList, bundlename);
    if (index == INVALIDVALUE) {
        ERROR_LOG("not found bundle");
        return OHOS::ERR_INVALID_VALUE;
    }

    // start ability through bundle information
    ErrCode result = AppManager::GetInstance()->StartAbilityByBundleInfo(abilityList[index], bundleList[index]);
    // print the result of start event
    PrintResultOfStartAbility(result, index);
    usleep(WAIT_TIME);
    return result;
}

ErrCode AppswitchInput::RandomInput()
{
    std::vector<std::string> bundleList(0);
    std::vector<std::string> abilityList(0);
    WuKongUtil::GetInstance()->GetBundleList(bundleList, abilityList);
    if (bundleList.size() == 0 || abilityList.size() == 0) {
        ERROR_LOG_STR("bundleList (%u) or abilityList (%u) is 0", bundleList.size(), abilityList.size());
        return OHOS::ERR_INVALID_VALUE;
    }
    uint32_t index = GetAbilityIndex(bundleList);
    if (index == INVALIDVALUE) {
        ERROR_LOG("not found bundle");
        return OHOS::ERR_INVALID_VALUE;
    }
    // start ability through bundle information
    ErrCode result = AppManager::GetInstance()->StartAbilityByBundleInfo(abilityList[index], bundleList[index]);
    // print the result of start event
    PrintResultOfStartAbility(result, index);
    TRACK_LOG_STR("bundle index: %d", index);
    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode AppswitchInput::PrintResultOfStartAbility(const ErrCode result, uint32_t index)
{
    std::vector<std::string> bundleList;
    std::vector<std::string> abilityList;
    WuKongUtil::GetInstance()->GetBundleList(bundleList, abilityList);
    if (result == OHOS::ERR_OK) {
        INFO_LOG_STR("Bundle Name: (%s) startup successful", bundleList[index].c_str());
    } else {
        INFO_LOG_STR("Bundle Name: (%s) startup failed", bundleList[index].c_str());
    }
    return OHOS::ERR_OK;
}

ErrCode AppswitchInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}

uint32_t AppswitchInput::GetAbilityIndex(std::vector<std::string>& bundlelist)
{
    uint32_t index = INVALIDVALUE;
    std::vector<std::string> allowlist;
    std::vector<std::string> validlist;
    WuKongUtil::GetInstance()->GetAllowList(allowlist);
    WuKongUtil::GetInstance()->GetValidBundleList(validlist);
    if (bundlelist.size() > 0) {
        if (allowlist.size() > 0) {
            index = WuKongUtil::GetInstance()->FindElement(bundlelist, allowlist.at(rand() % allowlist.size()));
        } else if (validlist.size() > 0) {
            index = WuKongUtil::GetInstance()->FindElement(bundlelist, validlist.at(rand() % validlist.size()));
        } else {
            index = rand() % bundlelist.size();
        }
    }
    return index;
}
}  // namespace WuKong
}  // namespace OHOS

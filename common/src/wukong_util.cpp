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

#include "wukong_util.h"

#include "accessibility_ui_test_ability.h"
#include "display_manager.h"
#include "event_monitor.h"
#include "iservice_registry.h"
#include "launcher_service.h"
#include "string_ex.h"

namespace OHOS {
namespace WuKong {
using namespace OHOS::Accessibility;
using namespace std;
const int userId = 100;

Util::Util()
{
    TRACK_LOG_STD();
    const int timeBufsize = 32;
    char fileNameBuf[timeBufsize] = {0};
    time_t currentTime = (time_t)time(0);

    int res = strftime(fileNameBuf, timeBufsize, "%Y%m%d_%H%M%S", localtime(&currentTime));
    if (res > 0) {
        startRunTime_ = std::string(fileNameBuf);
    } else {
        startRunTime_ = "unvalid_time";
    }
    DEBUG_LOG_STR("%s", startRunTime_.c_str());
    TRACK_LOG_END();
}

/**
 * @brief: release util
 */
Util::~Util()
{
    TRACK_LOG_STD();
}

ErrCode Util::GetAllAppInfo()
{
    AppExecFwk::LauncherService launcherservice;
    std::vector<AppExecFwk::LauncherAbilityInfo> launcherAbilityInfos;
    launcherservice.GetAllLauncherAbilityInfos(userId, launcherAbilityInfos);
    for (auto item : launcherAbilityInfos) {
        std::string bundleName = item.elementName.GetBundleName();
        // store the list of all bundle names
        bundleList_.push_back(bundleName);
        abilityList_.push_back(item.elementName.GetAbilityName());
        int isInBlackList = FindElement(blackList_, bundleName);
        if (isInBlackList != -1) {
            continue;
        }
        // store the list of bundle names except for block list
        validBundleList_.push_back(bundleName);
        validAbilityList_.push_back(item.elementName.GetAbilityName());
    }
    return OHOS::ERR_OK;
}

void Util::GetBundleList(std::vector<std::string> &bundlelist, std::vector<std::string> &abilitylist)
{
    if (bundleList_.size() == 0) {
        GetAllAppInfo();
    }
    bundlelist = bundleList_;
    abilitylist = abilityList_;
}

int Util::FindElement(std::vector<std::string> bundleList, std::string key)
{
    auto it = find(bundleList.begin(), bundleList.end(), key);
    if (it != bundleList.end()) {
        return distance(bundleList.begin(), it);
    }
    return -1;
}

ErrCode Util::CheckBundleNameList()
{
    std::set<std::string> m(whiteList_.begin(), whiteList_.end());

    for (auto it = blackList_.begin(); it != blackList_.end(); it++) {
        if (m.find(*it) != m.end()) {
            ERROR_LOG("invalid param:please check params of '-p' and '-b'");
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

ErrCode Util::CheckArgumentList(std::vector<std::string> &arguments)
{
    ErrCode result = OHOS::ERR_OK;
    GetAllAppInfo();
    for (unsigned int i = 0; i < arguments.size(); i++) {
        int index = FindElement(bundleList_, arguments[i]);
        if (index == -1) {
            ERROR_LOG_STR("bundle name '%s' is not be included in all bundles", arguments[i].c_str());
            result = OHOS::ERR_INVALID_VALUE;
        }
    }
    return result;
}

ErrCode Util::SetWhiteList(const std::string &optarg)
{
    ErrCode result = OHOS::ERR_OK;
    SplitStr(optarg, ",", whiteList_);
    result = CheckArgumentList(whiteList_);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(whiteList_);
        if (whiteList_.size() > 0) {
            result = CheckBundleNameList();
        }
    }
    return result;
}

ErrCode Util::SetBlackList(const std::string &optarg)
{
    ErrCode result = OHOS::ERR_OK;
    SplitStr(optarg, ",", blackList_);
    result = CheckArgumentList(blackList_);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(blackList_);
        if (blackList_.size() > 0) {
            result = CheckBundleNameList();
        }
    }
    return result;
}

void Util::DelRepeatArguments(std::vector<std::string> &argumentlist)
{
    std::set<std::string> s(argumentlist.begin(), argumentlist.end());
    argumentlist.assign(s.begin(), s.end());
}

void Util::GetWhiteList(std::vector<std::string> &whiteList)
{
    whiteList = whiteList_;
}

void Util::GetBlackList(std::vector<std::string> &blackList)
{
    blackList = blackList_;
}

void Util::GetValidBundleList(std::vector<std::string> &validbundlelist)
{
    validbundlelist = validBundleList_;
}

void Util::SetAllAppInfo(std::vector<std::string> &bundleList, std::vector<std::string> &abilityList)
{
    bundleList_ = bundleList;
    abilityList_ = abilityList;
}

void Util::GetScreenSize(int32_t &width, int32_t &height)
{
    if (screenWidth_ == -1 || screenHeight_ == -1) {
        OHOS::Rosen::DisplayManager &displayMgr = OHOS::Rosen::DisplayManager::GetInstance();
        sptr<OHOS::Rosen::Display> display = displayMgr.GetDefaultDisplay();
        screenWidth_ = display->GetWidth();
        screenHeight_ = display->GetHeight();
    }
    width = screenWidth_;
    height = screenHeight_;
}
}  // namespace WuKong
}  // namespace OHOS

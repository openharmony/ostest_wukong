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

#include "display_manager.h"
#include "iservice_registry.h"
#include "launcher_service.h"
#include "string_ex.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
using namespace std;
const int userId = 100;

WuKongUtil::WuKongUtil()
{
    TRACK_LOG_STD();
    const int timeBufsize = 32;
    char fileNameBuf[timeBufsize] = {0};
    time_t currentTime = time(0);
    uint32_t res = 0;

    if (currentTime > 0) {
        tm* timePtr = localtime(&currentTime);
    res = strftime(fileNameBuf, timeBufsize, "%Y%m%d_%H%M%S", timePtr);
    }
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
WuKongUtil::~WuKongUtil()
{
    TRACK_LOG_STD();
}

ErrCode WuKongUtil::GetAllAppInfo()
{
    AppExecFwk::LauncherService launcherservice;
    std::vector<AppExecFwk::LauncherAbilityInfo> launcherAbilityInfos;
    launcherservice.GetAllLauncherAbilityInfos(userId, launcherAbilityInfos);
    for (auto item : launcherAbilityInfos) {
        iconPath_ = item.applicationInfo.iconPath;
        DEBUG_LOG_STR("iconPath: %s", item.applicationInfo.iconPath.c_str());
        DEBUG_LOG_STR("codePath: %s", item.applicationInfo.codePath.c_str());
        DEBUG_LOG_STR("dataDir: %s", item.applicationInfo.dataDir.c_str());
        DEBUG_LOG_STR("dataBaseDir: %s", item.applicationInfo.dataBaseDir.c_str());
        DEBUG_LOG_STR("cacheDir: %s", item.applicationInfo.cacheDir.c_str());
        DEBUG_LOG_STR("entryDir: %s", item.applicationInfo.entryDir.c_str());
        std::string bundleName = item.elementName.GetBundleName();
        // store the list of all bundle names
        bundleList_.push_back(bundleName);
        abilityList_.push_back(item.elementName.GetAbilityName());
        int isInBlockList = FindElement(blockList_, bundleName);
        if (isInBlockList != -1) {
            continue;
        }
        // store the list of bundle names except for block list
        validBundleList_.push_back(bundleName);
        validAbilityList_.push_back(item.elementName.GetAbilityName());
    }
    return OHOS::ERR_OK;
}

void WuKongUtil::GetBundleList(std::vector<std::string> &bundlelist, std::vector<std::string> &abilitylist)
{
    if (bundleList_.size() == 0) {
        GetAllAppInfo();
    }
    bundlelist = bundleList_;
    abilitylist = abilityList_;
}

int WuKongUtil::FindElement(std::vector<std::string> bundleList, std::string key)
{
    auto it = find(bundleList.begin(), bundleList.end(), key);
    if (it != bundleList.end()) {
        return distance(bundleList.begin(), it);
    }
    return -1;
}

ErrCode WuKongUtil::CheckBundleNameList()
{
    std::set<std::string> m(allowList_.begin(), allowList_.end());

    for (auto it = blockList_.begin(); it != blockList_.end(); it++) {
        if (m.find(*it) != m.end()) {
            ERROR_LOG("invalid param:please check params of '-p' and '-b'");
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

ErrCode WuKongUtil::CheckArgumentList(std::vector<std::string> &arguments)
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

ErrCode WuKongUtil::SetAllowList(const std::string &optarg)
{
    ErrCode result = OHOS::ERR_OK;
    SplitStr(optarg, ",", allowList_);
    result = CheckArgumentList(allowList_);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(allowList_);
        if (allowList_.size() > 0) {
            result = CheckBundleNameList();
        }
    }
    return result;
}

ErrCode WuKongUtil::SetBlockList(const std::string &optarg)
{
    ErrCode result = OHOS::ERR_OK;
    SplitStr(optarg, ",", blockList_);
    result = CheckArgumentList(blockList_);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(blockList_);
        if (blockList_.size() > 0) {
            result = CheckBundleNameList();
        }
    }
    return result;
}

void WuKongUtil::DelRepeatArguments(std::vector<std::string> &argumentlist)
{
    std::set<std::string> s(argumentlist.begin(), argumentlist.end());
    argumentlist.assign(s.begin(), s.end());
}

void WuKongUtil::GetAllowList(std::vector<std::string> &allowList)
{
    allowList = allowList_;
}

void WuKongUtil::GetBlockList(std::vector<std::string> &blockList)
{
    blockList = blockList_;
}

void WuKongUtil::GetValidBundleList(std::vector<std::string> &validbundlelist)
{
    validbundlelist = validBundleList_;
}

void WuKongUtil::SetAllAppInfo(std::vector<std::string> &bundleList, std::vector<std::string> &abilityList)
{
    bundleList_ = bundleList;
    abilityList_ = abilityList;
}

void WuKongUtil::GetScreenSize(int32_t &width, int32_t &height)
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

void WuKongUtil::GetIconPath(std::string &iconpath)
{
    iconpath = iconPath_;
}
}  // namespace WuKong
}  // namespace OHOS

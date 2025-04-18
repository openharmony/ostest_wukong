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

#include <climits>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <sstream>
#include <sys/stat.h>

#include "ability_manager_client.h"
#include "display_manager.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "launcher_service.h"
#include "png.h"
#include "string_ex.h"
#include "system_ability_definition.h"
#include "wukong_define.h"
#include "bundle_mgr_proxy.h"
#include "common.h"
#include "accessibility_ui_test_ability.h"
#include "component_manager.h"
#include "dump_usage.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string DEFAULT_DIR = "/data/local/tmp/wukong/report/";
const uint32_t LAP_HEIGHT = 200;
const uint32_t CHARGE_STRIDE = 11;
const uint32_t BLANK_THR = 30;
const uint32_t WHITE_THR = 225;
int g_bwCount = 0;
bool g_isBwScreen(std::shared_ptr<Media::PixelMap> pixelMap)
{
    auto width = static_cast<uint32_t>(pixelMap->GetWidth());
    auto height = static_cast<uint32_t>(pixelMap->GetHeight());
    auto data = pixelMap->GetPixels();
    auto stride = static_cast<uint32_t>(pixelMap->GetRowBytes());
    bool isUpper = true;
    bool isLower = true;
    uint32_t heightLimit = height - LAP_HEIGHT;
    for (uint32_t i = LAP_HEIGHT; i < heightLimit && (isUpper || isLower); i += CHARGE_STRIDE) {
        for (uint32_t j = 0; j < width; j += 1) {
            auto pixel = *(data + (i * stride) + (j * 4));
            if (pixel >= BLANK_THR) {
                isLower = false;
            }
            if (pixel <= WHITE_THR) {
                isUpper = false;
            }
            if (!isLower && !isUpper) {
                break;
            }
        }
    }
    return isLower || isUpper;
}

bool TakeWuKongScreenCap(const std::string &wkScreenPath, const bool checkBWScreen = false)
{
    // get PixelMap from DisplayManager API
    Rosen::DisplayManager &displayMgr = Rosen::DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> pixelMap = displayMgr.GetScreenshot(displayMgr.GetDefaultDisplayId());
    static constexpr int bitmapDepth = 8;
    if (pixelMap == nullptr) {
        DEBUG_LOG("Failed to get display pixelMap");
        return false;
    }
    auto width = static_cast<uint32_t>(pixelMap->GetWidth());
    auto height = static_cast<uint32_t>(pixelMap->GetHeight());
    auto data = pixelMap->GetPixels();
    auto stride = static_cast<uint32_t>(pixelMap->GetRowBytes());
    if (checkBWScreen) {
        bool result = g_isBwScreen(pixelMap);
        if (result) {
            g_bwCount++;
            INFO_LOG_STR("isBWScreen is true, BWScreen count : %d", g_bwCount);
        }
    }
    png_structp pngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (pngStruct == nullptr) {
        DEBUG_LOG("error: png_create_write_struct nullptr!");
        return false;
    }
    png_infop pngInfo = png_create_info_struct(pngStruct);
    if (pngInfo == nullptr) {
        DEBUG_LOG("error: png_create_info_struct error nullptr!");
        png_destroy_write_struct(&pngStruct, nullptr);
        return false;
    }
    FILE *fp = fopen(wkScreenPath.c_str(), "wb");
    if (fp == nullptr) {
        ERROR_LOG("error: open file error!");
        png_destroy_write_struct(&pngStruct, &pngInfo);
        return false;
    }
    png_init_io(pngStruct, fp);
    png_set_IHDR(pngStruct, pngInfo, width, height, bitmapDepth, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_set_packing(pngStruct);          // set packing info
    png_write_info(pngStruct, pngInfo);  // write to header
    for (uint32_t i = 0; i < height; i++) {
        png_write_row(pngStruct, data + (i * stride));
    }
    png_write_end(pngStruct, pngInfo);
    // free
    png_destroy_write_struct(&pngStruct, &pngInfo);
    (void)fclose(fp);
    return true;
}
}  // namespace
using namespace std;
using namespace OHOS::AppExecFwk;
const int USE_ID = 100;
WuKongUtil::WuKongUtil()
{
    TRACK_LOG_STD();
    const int timeBufsize = 32;
    char fileNameBuf[timeBufsize] = {0};
    time_t currentTime = time(0);
    uint32_t res = 0;

    if (currentTime > 0) {
        tm *timePtr = localtime(&currentTime);
        if (timePtr == nullptr) {
            ERROR_LOG("timePtr is nullptr");
            return;
        }
        res = strftime(fileNameBuf, timeBufsize, "%Y%m%d_%H%M%S", timePtr);
    }
    if (res > 0) {
        startRunTime_ = std::string(fileNameBuf);
    } else {
        startRunTime_ = "unvalid_time";
    }
    curDir_ = DEFAULT_DIR + startRunTime_ + "/";
    DEBUG_LOG_STR("common dir{%s}", curDir_.c_str());
    DIR *rootDir = nullptr;
    std::string dirStr = "/";
    std::vector<std::string> strs;
    OHOS::SplitStr(curDir_, "/", strs);
    for (auto str : strs) {
        dirStr.append(str);
        dirStr.append("/");
        if ((rootDir = opendir(dirStr.c_str())) == nullptr) {
            int ret = mkdir(dirStr.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
            if (ret != 0 && dirStr != "/data/" && dirStr != "/data/local/") {
                std::cerr << "failed to create dir: " << dirStr << std::endl;
                break;
            }
        } else {
            closedir(rootDir);
        }
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
    std::vector<AppExecFwk::LauncherAbilityInfo> launcherAbilityInfos(0);

    bool result = launcherservice.GetAllLauncherAbilityInfos(USE_ID, launcherAbilityInfos);
    DEBUG_LOG_STR("GetAllLauncherAbilityInfos: size (%u), result (%d)", launcherAbilityInfos.size(), result);
    if (launcherAbilityInfos.size() == 0) {
        ERROR_LOG("GetAllLauncherAbilityInfos size is 0");
        return OHOS::ERR_INVALID_VALUE;
    }
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
        uint32_t isInBlockList = FindElement(blockList_, bundleName);
        if (isInBlockList != INVALIDVALUE) {
            continue;
        }
        // store the list of bundle names except for block list
        validBundleList_.push_back(bundleName);
        validAbilityList_.push_back(item.elementName.GetAbilityName());
    }
    GetAllAbilities();
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

uint32_t WuKongUtil::FindElement(std::vector<std::string> &bundleList, std::string key)
{
    auto it = find(bundleList.begin(), bundleList.end(), key);
    if (it != bundleList.end()) {
        return distance(bundleList.begin(), it);
    }
    return INVALIDVALUE;
}

bool WuKongUtil::ContainsElement(std::vector<std::string> &bundleList, std::string key)
{
    return INVALIDVALUE != FindElement(bundleList, key);
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

ErrCode WuKongUtil::CheckArgumentList(std::vector<std::string> &arguments, bool isAddToList)
{
    ErrCode result = OHOS::ERR_OK;
    GetAllAppInfo();
    for (uint32_t i = 0; i < arguments.size(); i++) {
        uint32_t index = FindElement(bundleList_, arguments[i]);
        if (index == INVALIDVALUE) {
            uint32_t unLaunchedIndex = FindElement(unLaunchedBundleList_, arguments[i]);
            if (unLaunchedIndex == INVALIDVALUE) {
                ERROR_LOG_STR("bundle name '%s' is not be included in all bundles", arguments[i].c_str());
                result = OHOS::ERR_INVALID_VALUE;
            } else if (isAddToList) {
                bundleList_.push_back(arguments[i]);
                abilityList_.push_back(unLaunchedAbilityList_[unLaunchedIndex]);
            }
        }
    }
    return result;
}

ErrCode WuKongUtil::SetAllowList(const std::string &optarg)
{
    SplitStr(optarg, ",", allowList_);
    ErrCode result = CheckArgumentList(allowList_, true);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(allowList_);
        if (allowList_.size() > 0) {
            result = CheckBundleNameList();
        }
    }
    return result;
}

ErrCode WuKongUtil::CheckAbilityArgumentList(std::vector<std::string> &arguments)
{
    ErrCode result = OHOS::ERR_OK;
    GetAllAppInfo();
    for (uint32_t i = 0; i < arguments.size(); i++) {
        uint32_t index = FindElement(allAbilityList_, arguments[i]);
        if (index == INVALIDVALUE) {
            ERROR_LOG_STR("Ability name '%s' is not be included in all abilities", arguments[i].c_str());
            result = OHOS::ERR_INVALID_VALUE;
        }
    }
    return result;
}

ErrCode WuKongUtil::SetAllowAbilityList(const std::string &optarg)
{
    SplitStr(optarg, ",", allowAbilityList_);
    ErrCode result = CheckAbilityArgumentList(allowAbilityList_);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(allowAbilityList_);
        if (allowAbilityList_.size() > 0) {
            result = CheckAbilityNameList();
        }
    }
    return result;
}

ErrCode WuKongUtil::SetBlockAbilityList(const std::string &optarg)
{
    SplitStr(optarg, ",", blockAbilityList_);
    ErrCode result = CheckAbilityArgumentList(blockAbilityList_);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(blockAbilityList_);
        if (blockAbilityList_.size() > 0) {
            result = CheckAbilityNameList();
        }
    }
    return result;
}

ErrCode WuKongUtil::CheckAbilityNameList()
{
    std::set<std::string> m(allowAbilityList_.begin(), allowAbilityList_.end());

    for (auto it = blockAbilityList_.begin(); it != blockAbilityList_.end(); it++) {
        if (m.find(*it) != m.end()) {
            ERROR_LOG("invalid param:please check params of '-e' and '-E'");
            return OHOS::ERR_INVALID_VALUE;
        }
    }
    return OHOS::ERR_OK;
}

ErrCode WuKongUtil::SetBlockList(const std::string &optarg)
{
    SplitStr(optarg, ",", blockList_);
    ErrCode result = CheckArgumentList(blockList_, false);
    if (result == OHOS::ERR_OK) {
        // delete repeat argument
        DelRepeatArguments(blockList_);
        if (blockList_.size() > 0) {
            result = CheckBundleNameList();
        }
    }
    return result;
}

ErrCode WuKongUtil::SetBlockPageList(const std::string &optarg)
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<std::string> temp;
    SplitStr(optarg, ",", temp);
    blockPageList_.insert(blockPageList_.end(), temp.begin(), temp.end());
    // delete repeat argument
    DelRepeatArguments(blockPageList_);
    if (blockPageList_.size() > 0) {
        stringstream ss;
        for (const auto& str:blockPageList_) {
        ss << str << " ";
        }
        INFO_LOG_STR("Please confirm that the blocked page is %s", ss.str().c_str());
        result = OHOS::ERR_OK;
    }
    return result;
}

ErrCode WuKongUtil::SetComponentUri(const std::string &optarg)
{
    uri_ = optarg;
    ErrCode result;
    if (!uri_.empty()) {
        result = OHOS::ERR_OK;
        INFO_LOG_STR("Set component Uri is %s", uri_.c_str());
    } else {
        result = OHOS::ERR_INVALID_VALUE;
        ERROR_LOG("Set component Uri is failed, please check -U param");
    }
    return result;
}

ErrCode WuKongUtil::SetComponentUriType(const std::string &optarg)
{
    uriType_ = optarg;
    ErrCode result;
    if (!uriType_.empty()) {
        result = OHOS::ERR_OK;
        INFO_LOG_STR("Set component Uri Type is %s", uriType_.c_str());
    } else {
        result = OHOS::ERR_INVALID_VALUE;
        ERROR_LOG("Set component Uri Type is failed, please check -x param");
    }
    return result;
}

std::string WuKongUtil::GetComponentUri()
{
    return uri_;
}

std::string WuKongUtil::GetComponentUriType()
{
    return uriType_;
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

void WuKongUtil::GetAllowAbilityList(std::vector<std::string> &allowAbilityList)
{
    allowAbilityList = allowAbilityList_;
}

void WuKongUtil::GetBlockList(std::vector<std::string> &blockList)
{
    blockList = blockList_;
}

void WuKongUtil::GetBlockAbilityList(std::vector<std::string> &blockAbilityList)
{
    blockAbilityList = blockAbilityList_;
}

void WuKongUtil::GetBlockPageList(std::vector<std::string> &blockPageList)
{
    blockPageList = blockPageList_;
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

void WuKongUtil::SetTempAllowList(std::vector<std::string> tempAllowList)
{
    tempAllowList_ = tempAllowList;
}

std::vector<std::string> WuKongUtil::GetTempAllowList()
{
    return tempAllowList_;
}

void WuKongUtil::SetOrderFlag(bool orderFlag)
{
    orderFlag_ = orderFlag;
}

bool WuKongUtil::GetOrderFlag()
{
    return orderFlag_;
}

ErrCode WuKongUtil::GetScreenSize(int32_t &width, int32_t &height)
{
    ErrCode result = OHOS::ERR_OK;
    if (screenWidth_ == -1 || screenHeight_ == -1) {
        OHOS::Rosen::DisplayManager &displayMgr = OHOS::Rosen::DisplayManager::GetInstance();
        sptr<OHOS::Rosen::Display> display = displayMgr.GetDefaultDisplay();
        if (display == nullptr) {
            ERROR_LOG("get screen size failed");
            return OHOS::ERR_NO_INIT;
        }
        screenWidth_ = display->GetWidth();
        screenHeight_ = display->GetHeight();
    }
    width = screenWidth_;
    height = screenHeight_;
    return result;
}

void WuKongUtil::GetIconPath(std::string &iconpath)
{
    iconpath = iconPath_;
}

ErrCode WuKongUtil::WukongScreenCap(std::string &screenStorePath, bool gCommandUitest, bool g_commandCHECKBWSCREEN)
{
    using namespace std::chrono;
    ErrCode result = ERR_OK;
    auto wukongts = to_string(time_point_cast<milliseconds>(system_clock::now()).time_since_epoch().count());
    int fileExist_ = access((curDir_ + "screenshot").c_str(), F_OK);
    if (fileExist_ == 0) {
        DEBUG_LOG("File exist.");
    } else {
        const int wukongScreenShot = mkdir((curDir_ + "screenshot").c_str(), 0777);
        DEBUG_LOG("File create.");
        if (wukongScreenShot == -1) {
            DEBUG_LOG("Error creating directory!");
            result = ERR_NO_INIT;
        }
    }
    auto wkScreenPath = curDir_ + "screenshot/" + "/" + wukongts + ".png";
    DEBUG_LOG_STR("WukongScreenCap store path is  {%s}", wkScreenPath.c_str());
    if (gCommandUitest) {
        auto cm = ComponentManager::GetInstance();
        auto auita = Accessibility::AccessibilityUITestAbility::GetInstance();
        auita->Disconnect();
        std::string uitestCmd = "uitest dumpLayout -i -p " + curDir_ + "screenshot" + "/" + wukongts + ".json";
        std::string res = Common::runProcess(uitestCmd);
        DEBUG_LOG_STR("unitestCmd : %s", res.c_str());
        cm->Disconnect();
        if (!cm->Connect()) {
            ERROR_LOG("ComponentManager Connect failed");
            return OHOS::ERR_INVALID_OPERATION;
        }
    }
    bool isTakeScreen = TakeWuKongScreenCap(wkScreenPath, g_commandCHECKBWSCREEN);
    if (isTakeScreen == true) {
        screenStorePath = wkScreenPath;
        DEBUG_LOG("The snapshot has been created.");
    } else {
        DEBUG_LOG("This snapshot can not be created.");
    }
    return result;
}

sptr<IBundleMgr> WuKongUtil::GetBundleMgrProxy() const
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        ERROR_LOG("failed to get system ability mgr.");
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        ERROR_LOG("failed to get bundle manager proxy.");
        return nullptr;
    }

    return iface_cast<IBundleMgr>(remoteObject);
}

void WuKongUtil::GetAllAbilitiesByBundleName(std::string bundleName, std::vector<std::string> &abilities)
{
    TRACK_LOG_STD();
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    std::vector<BundleInfo> bundleInfos;
    if (!bundleMgrProxy) {
        ERROR_LOG("bundleMgrProxy is nullptr");
        return;
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos, USE_ID);
    if (!getInfoResult) {
        ERROR_LOG("GetBundleInfos ERR");
        return;
    }
    DEBUG_LOG_STR("bundles length{%d}", bundleInfos.size());
    for (const auto &bundleIter : bundleInfos) {
        TRACK_LOG_STR("bundleIter.name{%s}", bundleIter.name.c_str());
        BundleInfo bundleInfo;
        if (bundleIter.name == bundleName) {
            TRACK_LOG_STR("map bundleName{%s}", bundleIter.name.c_str());
            bool result =
                bundleMgrProxy->GetBundleInfo(bundleIter.name, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo, 100);
            if (!result) {
                ERROR_LOG_STR("WriteBundleInfo getBundleInfo result %d, bundleName: %s", result,
                    bundleIter.name.c_str());
                break;
            }
            for (auto &abilityIter : bundleInfo.abilityInfos) {
                TRACK_LOG_STR("bundleName{%s} container abilities item{%s}", bundleIter.name.c_str(),
                              (abilityIter.name).c_str());
                abilities.push_back(abilityIter.name);
            }
        }
    }
    TRACK_LOG_END();
}

ErrCode WuKongUtil::GetAllAbilities()
{
    TRACK_LOG_STD();
    ErrCode result = OHOS::ERR_INVALID_VALUE;
    sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
    std::vector<BundleInfo> bundleInfos;
    if (!bundleMgrProxy) {
        ERROR_LOG("bundleMgrProxy is nullptr");
        return result;
    }
    bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos, USE_ID);
    if (!getInfoResult) {
        ERROR_LOG("GetBundleInfos ERR");
        return result;
    }
    DEBUG_LOG_STR("bundles length{%d}", bundleInfos.size());
    for (const auto &bundleIter : bundleInfos) {
        std::string bundleName = bundleIter.name;
        uint32_t bundleListIndex = FindElement(bundleList_, bundleName);
        BundleInfo bundleInfo;
        bool getBundleResult =
            bundleMgrProxy->GetBundleInfo(bundleName, BundleFlag::GET_BUNDLE_WITH_ABILITIES, bundleInfo, 100);
        if (!getBundleResult) {
            ERROR_LOG_STR("WriteWuKongBundleInfo getBundleInfo result %d", getBundleResult);
            continue;
        }
        if (bundleListIndex != INVALIDVALUE) {
            for (auto &abilityIter : bundleInfo.abilityInfos) {
                allBundleList_.push_back(bundleName);
                allAbilityList_.push_back(abilityIter.name);
                DEBUG_LOG_STR("bundleName: %s, abilityName: %s", bundleName.c_str(), abilityIter.name.c_str());
            }
        } else {
            for (auto &abilityIter : bundleInfo.abilityInfos) {
                unLaunchedBundleList_.push_back(bundleName);
                unLaunchedAbilityList_.push_back(abilityIter.name);
                allBundleList_.push_back(bundleName);
                allAbilityList_.push_back(abilityIter.name);
                DEBUG_LOG_STR("bundleName: %s, abilityName: %s", bundleName.c_str(), abilityIter.name.c_str());
            }
        }
    }
    if (unLaunchedAbilityList_.size() > 0) {
        result = OHOS::ERR_OK;
    }
    TRACK_LOG_END();
    return result;
}

std::string WuKongUtil::GetCurrentTestDir()
{
    return curDir_;
}

bool WuKongUtil::CopyFile(std::string &targetFile, std::string &sourceDir, std::string &destDir)
{
    std::ifstream in;
    std::ofstream out;
    DEBUG_LOG_STR("targetFile{%s} sourceDir{%s} destDir{%s}", targetFile.c_str(), sourceDir.c_str(), destDir.c_str());
    char filepathSource[PATH_MAX] = {'\0'};
    std::string sourceFile = sourceDir + targetFile;
    char *realPathSource = realpath(sourceFile.c_str(), filepathSource);
    if (realPathSource == nullptr) {
        ERROR_LOG_STR("failed to get source file path (%s), errno: (%d)", sourceFile.c_str(), errno);
        return false;
    }
    in.open(filepathSource, std::ios::binary);
    if (in.fail()) {
        std::cout << "Error 1: Fail to open the source file." << std::endl;
        in.close();
        out.close();
        return false;
    }

    char filepathDest[PATH_MAX] = {'\0'};
    char *realPathDest = realpath(destDir.c_str(), filepathDest);
    if (realPathDest == nullptr) {
        ERROR_LOG_STR("failed to get dest dir path (%s), errno: (%d)", destDir.c_str(), errno);
        return false;
    }
    DEBUG_LOG_STR("destDir{%s}", filepathDest);
    std::string destFile = destDir + targetFile;
    out.open(destFile.c_str(), std::ios::binary);
    if (out.fail()) {
        std::cout << "Error 2: Fail to create the new file." << std::endl;
        out.close();
        in.close();
        return false;
    }
    out << in.rdbuf();
    out.close();
    in.close();
    return true;
}

bool WuKongUtil::DeleteFile(std::string targetDir)
{
    DIR *dirdp = nullptr;
    char filepathSource[PATH_MAX] = {'\0'};
    char *realPathSource = realpath(targetDir.c_str(), filepathSource);
    if (realPathSource != nullptr) {
        struct dirent *dp;
        dirdp = opendir(targetDir.c_str());
        while ((dp = readdir(dirdp)) != NULL) {
            std::string currentFileName(dp->d_name);
            std::string sourceFile = targetDir + currentFileName;
            char *realFileSource = realpath(sourceFile.c_str(), filepathSource);
            if (realFileSource != nullptr) {
                remove(sourceFile.c_str());
            }
        }
    } else {
        return false;
    }
    (void)closedir(dirdp);
    return true;
}

void WuKongUtil::SetCompIdBlockList(const std::string &optarg)
{
    SplitStr(optarg, ",", compIdBlockList_);
}

std::vector<std::string> WuKongUtil::GetCompIdBlockList()
{
    return compIdBlockList_;
}

void WuKongUtil::SetCompTypeBlockList(const std::string &optarg)
{
    SplitStr(optarg, ",", compTypeBlockList_);
}

std::vector<std::string> WuKongUtil::GetCompTypeBlockList()
{
    return compTypeBlockList_;
}

std::string WuKongUtil::runProcess(std::string cmd)
{
    const unsigned int NUMBER_SIZE = 1024;
    if (FILE* fp = popen(cmd.c_str(), "r")) {
        std::ostringstream ostrStream;
        char line[NUMBER_SIZE];
        while (fgets(line, NUMBER_SIZE, fp)) {
            ostrStream << line;
        }
        pclose(fp);
        return ostrStream.str();
    } else {
        ERROR_LOG("popen function failed");
    }
    return "";
}
void WuKongUtil::SetIsFirstStartAppFlag(bool isFirstStartApp)
{
    isFirstStartApp_ = isFirstStartApp;
}
bool WuKongUtil::GetIsFirstStartAppFlag()
{
    return isFirstStartApp_;
}
std::string WuKongUtil::GetBundlePid()
{
    auto elementName = OHOS::AAFwk::AbilityManagerClient::GetInstance()->GetTopAbility();
    std::string curBundleName = elementName.GetBundleName();
    std::string bufCmd = "pidof " + curBundleName;
    FILE* fp = nullptr;
    fp = popen(bufCmd.c_str(), "r");
    TRACK_LOG_STR("Run %s", bufCmd.c_str());
    if (fp == nullptr) {
        ERROR_LOG("popen function failed");
        return "";
    }
    const int bufferSize = 32;
    char pid[bufferSize] = {0};
    if (fgets(pid, bufferSize - 1, fp) != nullptr) {
        std::string pidStr(pid);
        pidStr = OHOS::ReplaceStr(pidStr, "\n", " ");
        pclose(fp);
        return pidStr;
    }
    pclose(fp);
    return "";
}

uint64_t WuKongUtil::GetBundlePssTotal()
{
    OHOS::HiviewDFX::DumpUsage dumpUsage;
    std::string pidStr = GetBundlePid();
    int pid = std::stoi(pidStr);
    OHOS::HiviewDFX::MemInfoData::MemInfo memInfo;
    bool success = dumpUsage.GetMemInfo(pid, memInfo);
    if (success) {
        uint64_t pss = dumpUsage.GetPss(pid);
        DEBUG_LOG_STR("Get bundle PssTotal is %d", pss);
        return pss;
    }
    return 0;
}

}  // namespace WuKong
}  // namespace OHOS

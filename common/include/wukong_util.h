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

#ifndef TEST_WUKONG_UTIL_H
#define TEST_WUKONG_UTIL_H

#include <dirent.h>
#include <string>
#include <unistd.h>
#include <vector>

#include "bundle_mgr_proxy.h"
#include "errors.h"
#include "semaphore_ex.h"
#include "singleton.h"

namespace OHOS {
namespace WuKong {
class WuKongUtil : public DelayedSingleton<WuKongUtil> {
public:
    /**
     * @brief Get the All App Info object
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetAllAppInfo();

    /**
     * @brief to find  the location of the key bundle in the bundle list.
     * @param bundleList the bundlelist of test
     * @param key the key subscript in the bundle list.
     * @return the location of the key bundle in the bundlelist.
     */
    uint32_t FindElement(std::vector<std::string> bundleList, std::string key);

    /**
     * @brief Set the Allow List.
     * @param optarg command string
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetAllowList(const std::string &optarg);

    /**
     * @brief Set the Block List.
     * @param optarg command string.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetBlockList(const std::string &optarg);

    /**
     * @brief Get the Allow List of the test.
     * @param bundleNameArgs the Allow list.
     */
    void GetAllowList(std::vector<std::string> &Allowlist);

    /**
     * @brief Get the block List of the test.
     * @param blockList the block list.
     */
    void GetBlockList(std::vector<std::string> &blockList);

    /**
     * @brief get the bundlelist and ability list.
     * @param bundleList the bundle list of the test.
     * @param abilityList the ability list of the test.
     */
    void SetAllAppInfo(std::vector<std::string> &bundleList, std::vector<std::string> &abilityList);

    /**
     * @brief Get the Bundle List of the test.
     * @param bundlelist
     * @param abilitylist
     */
    void GetBundleList(std::vector<std::string> &bundlelist, std::vector<std::string> &abilitylist);

    /**
     * @brief Get the list of the bundle list except for blockList.
     * @param validbundlelist  the list except for blockList
     */
    void GetValidBundleList(std::vector<std::string> &validbundlelist);

    /**
     * @brief get the size of screen.
     * @param width the width of screen.
     * @param height the height of screen.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode GetScreenSize(int32_t &width, int32_t &height);

    /**
     * @brief get the icon path of app.
     * @param iconpath icon path of app.
     */
    void GetIconPath(std::string &iconpath);

    /**
     * @brief get start run time string for report, log etc.
     * @return start run time string.
     */
    std::string GetStartRunTime()
    {
        return startRunTime_;
    }

    /**
     * @brief get screenshot.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode WukongScreenCap(std::string &screenStorePath);

    /**
     * @brief get all abilities by bundleName string.
     * @param bundleName bundle name.
     * @param abilities all abilities find by bundle name.
     */
    void GetAllAbilitiesByBundleName(std::string bundleName, std::vector<std::string> &abilities);

    /**
     * @brief Check if param is valid
     * @param argumentlist argument list.
     * @return Return ERR_OK argument is valid, others is invalid.
     */
    ErrCode CheckArgumentList(std::vector<std::string> &arguments);

    /**
     * @brief get current test wukong base dir.
     * @return dir string
     */
    std::string GetCurrentTestDir();

    /*
     * @brief copy file from soruce to dest
     * @param targetFile
     * @param sourceFile
     * @param destFile
     * @return bool
     */
    bool CopyFile(std::string &targetFile, std::string &sourceDir, std::string &destDir);

    /*
     * @brief deleted screenshots
     * @param targetDir
     * @return bool
     */
    bool DeleteFile(std::string targetDir);

    /*
     * @brief set the whitelist list
     * @param tempAllowList
     */
    void SetTempAllowList(std::vector<std::string> tempAllowList);

    /*
     * @brief get the whitelist list
     * @return tmpAllowList
     */
    std::vector<std::string> GetTempAllowList();

    /*
     * @brief set orderFlag
     * @param orderFlag
     */
    void SetOrderFlag(bool orderFlag);

    /*
     * @brief get orderFlag
     * @return bool
     */
    bool GetOrderFlag();

    DECLARE_DELAYED_SINGLETON(WuKongUtil);

private:
    /**
     * @brief delete repeat argument of argument list.
     * @param argumentlist argument list.
     */
    void DelRepeatArguments(std::vector<std::string> &argumentlist);

    /**
     * @brief Check if there is a conflict between the block and Allow lists
     * @return Return ERR_OK no conflict, others have conflict.
     */
    ErrCode CheckBundleNameList();

    /**
     * @brief get bundle manager proxy for get abilitys.
     * @return return bundleManager instance.
     */
    sptr<OHOS::AppExecFwk::IBundleMgr> GetBundleMgrProxy() const;

    std::string iconPath_;
    std::vector<std::string> bundleList_;
    std::vector<std::string> abilityList_;
    std::vector<std::string> validBundleList_;
    std::vector<std::string> validAbilityList_;
    std::vector<std::string> allowList_;
    std::vector<std::string> blockList_ = {"com.ohos.devicemanagerui", "com.ohos.screenlock",
                                           "com.ohos.permissionmanager"};
    int32_t screenWidth_ = -1;
    int32_t screenHeight_ = -1;

    std::string startRunTime_;
    std::string curDir_;

    bool orderFlag_ = false;
    std::vector<std::string> tempAllowList_;
};

class WukongSemaphore {
public:
    WukongSemaphore(const std::string &name, size_t size) : name_(name)
    {
        maxCount_ = 0;
    }
    ~WukongSemaphore()
    {
    }

    bool Create()
    {
        return true;
    }
    bool Unlink()
    {
        return true;
    }

    bool Open()
    {
        return true;
    }
    bool Close()
    {
        return true;
    }

    bool Wait()
    {
        return true;
    }
    bool TryWait()
    {
        return true;
    }
    bool TimedWait(const struct timespec &ts)
    {
        return true;
    }
    bool Post()
    {
        return true;
    }

    int GetValue() const
    {
        return maxCount_;
    }

private:
    std::string name_;
    int maxCount_;
};

typedef WukongSemaphore NamedSemaphore;
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_UTIL_H

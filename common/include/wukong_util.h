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

#include <cstdio>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <vector>
#include <climits>
#include <fstream>
#include <iostream>
#include <memory.h>
#include <sstream>
#include <sys/stat.h>
#include <openssl/rand.h>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <sys/time.h>
#include <random>

#include "bundle_mgr_proxy.h"
#include "errors.h"
#include "semaphore_ex.h"
#include "singleton.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
enum SWIPE_DIRECTION { UP = 'u', DOWN = 'd', LEFT = 'l', RIGHT = 'r' };
enum MOUSE_ACTION_ENUM { CLICK = 'c', DOUBLE_CLICK = 'b', DRAG = 'g', MOVE = 'm' };
const int DIRECTION_LEN = 4;
const uint32_t CHARGE_STRIDE = 11;
const int STEP = 2;
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
    uint32_t FindElement(std::vector<std::string> &bundleList, std::string key);

    /**
     * @brief to find if the key bundle is in the bundle list.
     * @param bundleList the bundlelist of test
     * @param key the key subscript in the bundle list.
     * @return if the key bundle in the bundlelist.
     */
    bool ContainsElement(std::vector<std::string> &bundleList, std::string key);

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
     * @brief Set the BlockPage List.
     * @param optarg command string.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetBlockPageList(const std::string &optarg);

    /**
     * @brief Set the BlockAbility List.
     * @param optarg command string.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetBlockAbilityList(const std::string &optarg);

    /**
     * @brief Check if AbilityName is valid
     * @return Return ERR_OK argument is valid, others is invalid.
     */
    ErrCode CheckAbilityNameList();

    /**
     * @brief Get the Allow Ability List of the test.
     * @param bundleNameArgs the Allow Ability list.
     */
    void GetAllowAbilityList(std::vector<std::string> &allowAbilityList);

    /**
     * @brief Check the list of allowed abilities to cannibalize
     * @return Return ERR_OK is ok, others have not ok.
     */
    ErrCode CheckAbilityArgumentList(std::vector<std::string> &allowAbilityList);

    /**
     * @brief Set Allow Ability List.
     * @param optarg the Ability list of the test.
     */
    ErrCode SetAllowAbilityList(const std::string &optarg);

    /**
     * @brief Get the Block Ability List of the test.
     * @param blockAbilityList the block Ability List.
     */
    void GetBlockAbilityList(std::vector<std::string> &blockAbilityList);

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
     * @brief Get the block page List of the test.
     * @param blockList the block page list.
     */
    void GetBlockPageList(std::vector<std::string> &blockPageList);

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
    ErrCode WukongScreenCap(std::string &screenStorePath, bool gCommandUitest, bool g_commandCheckBWScreen = false);

    /**
     * @brief get all abilities by bundleName string.
     * @param bundleName bundle name.
     * @param abilities all abilities find by bundle name.
     */
    void GetAllAbilitiesByBundleName(std::string bundleName, std::vector<std::string> &abilities);

    /**
     * @brief Check if param is valid
     * @param argumentlist argument list.
     * @param isAddToList should add check item to bundleList.
     * @return Return ERR_OK argument is valid, others is invalid.
     */
    ErrCode CheckArgumentList(std::vector<std::string> &arguments, bool isAddToList);

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

    /*
     * @brief set ComponentUri
     * @param optarg
     */
    ErrCode SetComponentUri(const std::string &optarg);

    /*
     * @brief get ComponentUri
     * @return string
     */
    std::string GetComponentUri();

    /*
     * @brief set ComponentUriType
     * @param optarg
     */
    ErrCode SetComponentUriType(const std::string &optarg);

    /*
     * @brief get ComponentUriType
     * @return string
     */
    std::string GetComponentUriType();

    /**
     * @brief get all ability infos
     * @return Return ERR_OK if get sth, others not.
     */
    ErrCode GetAllAbilities();

    /*
     * @brief set the component id block list
     * @param compIdBlockList
     */
    void SetCompIdBlockList(const std::string &optarg);

    /*
     * @brief get the component id block list
     * @return compIdBlockList
     */
    std::vector<std::string> GetCompIdBlockList();

    /*
     * @brief set the component Type block list
     * @param compTypeBlockList
     */
    void SetCompTypeBlockList(const std::string &optarg);

    /*
     * @brief get the component Type block list
     * @return compTypeBlockList
     */
    std::vector<std::string> GetCompTypeBlockList();

    /*
     * @brief cmd calls
     * @param cmd
     * @return cmd result
     */
    std::string runProcess(std::string cmd);

    /*
     * @brief set IsFirstStartAppFlag
     * @param isFirstStartApp
     */
    void SetIsFirstStartAppFlag(bool isFirstStartApp);

    /*
     * @brief get isFirstStartApp
     * @return bool
     */
    bool GetIsFirstStartAppFlag();

    /*
     * @brief get Bundle Pid
     * @return string
     */
    std::string GetBundlePid();

    /*
     * @brief get Bundle PssTotal
     * @return uint64_t
     */
    uint64_t GetBundlePssTotal();

    std::string GetDeviceType();
    std::string ExecuteCmd(std::string cmd);
    unsigned int Generate(unsigned int upperBound);
    int RangeGenerate(int lowerBound, int upperBound);
    bool NextBool();
    std::string GetPositionStr(int32_t x, int32_t y);
    void SetSwipeEnablePause(bool enablePause);
    bool GetSwipeEnablePause();
    int64_t GetSysClockTime();
    int64_t GetSysClockTimeMs();

    template <typename K>
    std::map<K, float> ConvertStringToMap(const std::string &input)
    {
        std::istringstream iss(input);
        std::string token;
        std::vector<std::string> tokens;
        float sum = 0.0f;
        std::map<K, float> optargMap;

        // 按逗号分割字符串
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() % STEP != 0) {
            std::cout << "Invalid input string format." << std::endl;
            return {};
        }

        // 配对键值
        for (size_t i = 0; i < tokens.size(); i += STEP) {
            if (i + 1 >= tokens.size()) {
                return {};
            }

            try {
                // 根据K的类型选择不同的转换方式
                K key = ParseKey<K>(tokens[i]);
                float value = std::stof(tokens[i + 1]);
                optargMap[key] = value;
                sum += value;
            } catch (const std::exception &e) {
                return {};
            }
        }

        // 检查百分比总和是否为1
        if (!(std::abs(sum - 1.0f) < 1e-6f)) {
            return {};
        }

        return optargMap;
    }

    template <typename K>
    K ParseKey(const std::string &token)
    {
        if constexpr (std::is_same_v<K, char>) {
            // 如果K是char类型，取字符串的第一个字符
            if (token.size() != 1) {
                throw std::invalid_argument("Invalid char key format.");
            }
            return token[0];
        } else {
            // 如果K是int类型，将字符串转换为整数
            return std::stoi(token);
        }
    }

    template <typename K>
    void ParseMapToVector(std::map<K, float> &srcMap, std::vector<K> &dstVector)
    {
        int totalPortions = 100;
        dstVector.clear();
        dstVector.resize(totalPortions);
        int currentIndex = 0;
        for (const auto &pair : srcMap) {
            int portions = static_cast<int>(totalPortions * pair.second);
            for (int i = 0; i < portions; ++i) {
                dstVector[currentIndex++] = pair.first;
            }
        }
    }

    template <typename T, std::size_t COUNT>
    T GetRandomEnumValue(const T (&enums)[COUNT])
    {
        // 创建随机数生成器
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, COUNT - 1);

        return enums[dis(gen)];
    }

    std::string GetFoldScreenType();

    bool StartWith(const std::string &str, const std::string &prefix)
    {
        return str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix;
    }

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
    std::vector<std::string> allowAbilityList_;
    std::vector<std::string> validBundleList_;
    std::vector<std::string> validAbilityList_;
    std::vector<std::string> allowList_;
    std::vector<std::string> blockList_ = {"com.ohos.devicemanagerui", "com.ohos.screenlock",
                                           "com.ohos.permissionmanager"};
    std::vector<std::string> blockPageList_ = {"pages/system"};
    std::vector<std::string> blockAbilityList_;
    std::vector<std::string> unLaunchedBundleList_;
    std::vector<std::string> unLaunchedAbilityList_;
    std::vector<std::string> allBundleList_;
    std::vector<std::string> allAbilityList_;
    std::string uri_;
    std::string uriType_;
    int32_t screenWidth_ = -1;
    int32_t screenHeight_ = -1;

    std::string startRunTime_;
    std::string curDir_;

    bool orderFlag_ = false;
    bool isFirstStartApp_ = true;
    bool swipeEnablePause = false;
    std::vector<std::string> tempAllowList_;
    std::vector<std::string> compIdBlockList_ = {};
    std::vector<std::string> compTypeBlockList_ = {"Divider"};
    std::string deviceType_ = "";
    std::string foldScreenType = "";
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

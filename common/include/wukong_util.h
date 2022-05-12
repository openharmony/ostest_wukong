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

#include <unistd.h>

#include "singleton.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class Util : public DelayedSingleton<Util> {
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
     * @return tthe location of the key bundle in the bundlelist.
     */
    int FindElement(std::vector<std::string> bundleList, std::string key);

    /**
     * @brief Set the White List.
     * @param optarg commmand string
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetWhiteList(const std::string &optarg);

    /**
     * @brief Set the Black List.
     * @param optarg command string.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SetBlackList(const std::string &optarg);

    /**
     * @brief Get the White List of the test.
     * @param bundleNameArgs the white list.
     */
    void GetWhiteList(std::vector<std::string> &whitelist);

    /**
     * @brief Get the block List of the test.
     * @param blockList the block list.
     */
    void GetBlackList(std::vector<std::string> & blockList);

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
     */
    void GetScreenSize(int32_t &width, int32_t &height);

    /**
     * @brief get start run time string for report, log etc.
     * @return start run time string.
     */
    std::string GetStartRunTime()
    {
        return startRunTime_;
    }

    DECLARE_DELAYED_SINGLETON(Util);

private:
    /**
     * @brief delete repeat argument of argument list.
     * @param argumentlist argument list.
     */
    void DelRepeatArguments(std::vector<std::string> &argumentlist);

    /**
     * @brief Check if there is a conflict between the black and white lists
     * @return Return ERR_OK no conflict, others have conflict.
     */
    ErrCode CheckBundleNameList();

    /**
     * @brief Check if param is valid
     * @param argumentlist argument list.
     * @return Return ERR_OK argument is valid, others is invalid.
     */
    ErrCode CheckArgumentList(std::vector<std::string> &arguments);

    std::vector<std::string> bundleList_;
    std::vector<std::string> abilityList_;
    std::vector<std::string> validBundleList_;
    std::vector<std::string> validAbilityList_;
    std::vector<std::string> whiteList_;
    std::vector<std::string> blackList_;
    int32_t screenWidth_ = -1;
    int32_t screenHeight_ = -1;
    std::string startRunTime_;
};

}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_UTIL_H

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

#include "wukong_command.h"
#include "wukong_app_manager.h"
#include "wukong_event_manager.h"
#include "wukong_exception_manager.h"
#include "bundle_mgr_proxy.h"

#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"

#include <iostream>
#include <getopt.h>
#include <random>
#include <chrono>
#include <thread>
#include <unistd.h>

using namespace OHOS::AAFwk;

namespace OHOS {
    namespace AppExecFwk {
        namespace {
            bool g_commandSEEDENABLE = false;
            bool g_commandCOUNTENABLE = false;
            bool g_commandBUNDLEENABLE = false;
            bool g_commandHELPENABLE = false;
            bool g_commandVERSIONENABLE = false;
            bool g_commandAPPSWITCHENABLE = false;
            bool g_commandTOUCHENABLE = false;
            bool g_commandINSOMNIAENABLE = false;
            bool g_commandMOTIONENABLE = false;
            bool g_commandXENABLE = false;
            bool g_commandYENABLE = false;
            bool g_commandTouch = false;


            const std::string SHORT_OPTIONS = "a:b:c:hi:s:t:vkl:x:y:m:n:";
            const struct option LONG_OPTIONS[] = {
                {"appswitch", required_argument, nullptr, 'a'},
                {"bundle", required_argument, nullptr, 'b'},
                {"count", required_argument, nullptr, 'c'},
                {"help", no_argument, nullptr, 'h'},
                {"interval", required_argument, nullptr, 'i'},
                {"seed", required_argument, nullptr, 's'},
                {"touch", required_argument, nullptr, 't'},
                {"version", no_argument, nullptr, 'v'},
                {"spec_insomnia", no_argument, nullptr, 'k'},
                {"motion", required_argument, nullptr, 'l'},
                {"xSrc", required_argument, nullptr, 'x'},
                {"ySrc", required_argument, nullptr, 'y'},
                {"xDst", required_argument, nullptr, 'm'},
                {"yDst", required_argument, nullptr, 'n'},
                {"touch_pos", no_argument, nullptr, 'u'},
            };
        }

        WuKongCommand::WuKongCommand(int argc, char *argv[]) : ShellCommand(argc, argv, WUKONG_TOOL_NAME)
        {}

        ErrCode WuKongCommand::init()
        {
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::CreateCommandMap()
        {
            commandMap_ = {
                    {"-v", std::bind(&WuKongCommand::GetWuKongVersion, this)},
                    {"help", std::bind(&WuKongCommand::RunAsHelpCommand, this)},
                    {"exec", std::bind(&WuKongCommand::RunAsParseCommand, this)},
                    {"appinfo", std::bind(&WuKongCommand::ShowAllAppInfo, this)},
            };
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::CreateMessageMap()
        {
            messageMap_ = {
                    { ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR, "error: wukong internal error.", },
            };
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::RunAsParseCommand()
        {
            APP_LOGI("%{public}s start", __func__);
            int result = OHOS::ERR_OK;
            
            int counter = 0;

            while (true) {
				int option = -1;
                counter++;
                option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

                APP_LOGI("option: %{public}d, optopt: %{public}d, optarg: %{public}s, optind: %{public}d",
                         option, optopt, optarg, optind);

                // process error
                if (optind < 0 || optind > argc_) {
                    return OHOS::ERR_INVALID_VALUE;
                }

                for (int i = 0; i < argc_; i++) {
                    APP_LOGI("argv_[%{public}d]: %{public}s", i, argv_[i]);
                }

                // process error
                if (option == -1) {
                    if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                        // 'wukong exec' with no option: wukong exec
                        // 'wukong exec' with a wrong argument: wukong exec xxx
                        APP_LOGE("'wukong exec' with no option.");
                        resultReceiver_.append(HELP_MSG_NO_OPTION + "\n");
                        result = OHOS::ERR_INVALID_VALUE;
                    }
                    break;
                }

                // process error
                if (option == '?') {
                    result = HandleUnknownOption(optopt);
                    break;
                }

                // process correct
                result = HandleNormalOption(option);
            }

            if (result == OHOS::ERR_OK) {
                result = RunAsExecCommand();
                resultReceiver_.append(STRING_WUKONG_INFO_OK);
            } else {
                resultReceiver_.append(STRING_WUKONG_INFO_NG);
            }
            APP_LOGI("%{public}s ends", __func__);
            return result;
        }

        ErrCode WuKongCommand::HandleUnknownOption(const char optopt)
        {
            APP_LOGI("%{public}s start", __func__);

            int result = OHOS::ERR_OK;
            switch (optopt) {
                case 'a':
                case 'b':
                case 'c':
                case 'i':
                case 's':
                case 't':
                case 'x':
                case 'y':
                case 'u':
                    resultReceiver_.append("error: option '");
                    resultReceiver_.append(argv_[optind - 1]);
                    resultReceiver_.append("' requires a value.\n");
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                case 'h': {
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
                default: {
                    // 'wukong exec' with a unknown option: wukong exec -x
                    std::cerr << "'wukong' with a unknown option"  << std::endl;
                    result = OHOS::ERR_INVALID_VALUE;
                    break;
                }
            }
            APP_LOGI("%{public}s ends", __func__);
            return result;
        }

        ErrCode WuKongCommand::HandleNormalOption(const int option)
        {
            APP_LOGI("%{public}s start", __func__);
            int result = OHOS::ERR_OK;
            switch (option) {
                case 'a': {
                    g_commandAPPSWITCHENABLE = true;
                    abilityPercent = std::stof(optarg);
                    break;
                }
                case 'b': {
                    bundleNameArgs = optarg;
                    g_commandBUNDLEENABLE = true;
                    break;
                }
                case 'c': {
                    countArgs = std::stoi(optarg);
                    g_commandCOUNTENABLE = true;
                    break;
                }
                case 'h': {
                    g_commandHELPENABLE = true;
                    break;
                }
                case 'i': {
                    intervalArgs = std::stoi(optarg);
                    break;
                }
                case 's': {
                    seedArgs = std::stoi(optarg);
                    g_commandSEEDENABLE = true;
                    break;
                }
                case 't': {
                    g_commandTOUCHENABLE = true;
                    touchPercent = std::stof(optarg);
                    break;
                }
                case 'v': {
                    g_commandVERSIONENABLE = true;
                    break;
                }
                case 'k': {
                    g_commandINSOMNIAENABLE = true;
                    break;
                }
                case 'l': {
                    g_commandMOTIONENABLE = true;
                    break;
                }
                case 'x': {
                    g_commandXENABLE = true;
                    xSrcPosi = std::stoi(optarg);
                    break;
                }
                case 'y': {
                    g_commandYENABLE = true;
                    ySrcPosi = std::stoi(optarg);
                    break;
                }
                case 'm': {
                    g_commandYENABLE = true;
                    xDstPosi = std::stoi(optarg);
                    break;
                }
                case 'n': {
                    g_commandYENABLE = true;
                    yDstPosi = std::stoi(optarg);
                    break;
                }
                case 'u':
                    g_commandTouch = true;
                    break;
            }
            APP_LOGI("%{public}s ends", __func__);
            return result;
        }

        ErrCode WuKongCommand::ShowAllAppInfo()
        {
            int result = GetAllAppInfo();
            if (result == OHOS::ERR_OK) {
                for (unsigned index = 0; index < bundleList.size(); index++) {
                    if (visibleList[index] == "true") {
                        std::cout << "BundleName:  " << bundleList[index] << std::endl;
                        std::cout << "AbilityName:  " << abilityList[index] << std::endl;
                    }
                }
            } else {
                result = OHOS::ERR_INVALID_VALUE;
            }
            return result;
        }

        ErrCode WuKongCommand::WriteWuKongBundleInfo(sptr<IBundleMgr> bundleMgrProxy,
                                                     const std::vector<BundleInfo> bundleInfos)
        {
            std::string visible = "";
            const int abilityIndex = 1000;
            for (auto &bundleIter : bundleInfos) {
                BundleInfo bundleInfo;
                bool result = bundleMgrProxy->GetBundleInfo(bundleIter.name, BundleFlag::GET_BUNDLE_WITH_ABILITIES,
                                                            bundleInfo, userId);
                if (!result) {
                    std::cout << "WriteWuKongBundleInfo getBundleInfo result " << result << std::endl;
                    break;
                }
                for (auto &abilityIter : bundleInfo.abilityInfos) {
                    if ((abilityIter.name.find(".MainAbility") > 0 &&
                        abilityIter.name.find(".MainAbility") < abilityIndex) ||
                        (abilityIter.name.find(".default") > 0 &&
                        abilityIter.name.find(".default") < abilityIndex)) {
                        visible = abilityIter.visible ? "true" : "false";
                        bundleList.push_back(abilityIter.bundleName);
                        abilityList.push_back(abilityIter.name);
                        visibleList.push_back(visible);
                    }
                }
            }
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::GetAllAppInfo()
        {
            sptr<IBundleMgr> bundleMgrProxy = GetBundleMgrProxy();
            std::vector<BundleInfo> bundleInfos;
            bool getInfoResult = bundleMgrProxy->GetBundleInfos(BundleFlag::GET_BUNDLE_DEFAULT, bundleInfos, userId);
            if (!getInfoResult) {
                return OHOS::ERR_INVALID_VALUE;
            }
            ErrCode result = WriteWuKongBundleInfo(bundleMgrProxy, bundleInfos);

            return result;
        }

        ErrCode WuKongCommand::RunAsExecCommand()
        {
            const int windowsWidth = 720;
            const int windowsHeight = 1080;

            if (g_commandHELPENABLE) {
                int result = RunAsHelpCommand();
                return result;
            }

            if (g_commandVERSIONENABLE) {
                int result = GetWuKongVersion();
                return result;
            }

            if (g_commandSEEDENABLE) {
                srand(seedArgs);
            } else {
                srand((unsigned int)time(nullptr));
                seedArgs = (int)time(nullptr);
            }

            if (g_commandTouch) {
                checkPosition(windowsWidth, windowsHeight);
                int result = RunTouchCommand();
                return result;
            }
            int result = GetAllAppInfo();

            float randomPercent = -1.0;
            int index = -1;
            int touchX = -1;
            int touchY = -1;
            bool slideDirection = true;

            std::cout << "seed:" << seedArgs << std::endl;
            std::cout << "count:" << countArgs << std::endl;
            std::cout << "interval:" << intervalArgs << std::endl;

            int defaultMotionPressure = 5;
            int sleepAwakeChangeInterval = 2;
            int oneMs = 1000;
            bool singleAppFlag = false;

            bool startReuslt = WuKongExceptionManager::GetInstance()->StartCatching();
            if (!startReuslt) {
                std::cerr << "[Error] failed to start catching" << std::endl;
                return OHOS::ERR_NO_INIT;
            }
            for (int i = 1; i <= countArgs; i++) {
                if (g_commandINSOMNIAENABLE) {
                    result = WuKongEventManager::GetInstance()->PowerOnAndOffEvent();
                    usleep(intervalArgs * oneMs);
                    if (i % sleepAwakeChangeInterval == 1) {
                        std::cout << i << " sleep" << std::endl;
                    } else {
                        std::cout << i << " awake" << std::endl;
                    }
                } else if (g_commandXENABLE && g_commandYENABLE) {
                    if (!g_commandCOUNTENABLE) {
                        result = WuKongEventManager::GetInstance()->MotionEvent(xSrcPosi, ySrcPosi,
                                                                                xDstPosi, yDstPosi,
                                                                                defaultMotionPressure);
                        break;
                    }
                    if (slideDirection) {
                        std::cout << "slideDirection:" << " come" << std::endl;
                        result = WuKongEventManager::GetInstance()->MotionEvent(xSrcPosi, ySrcPosi,
                                                                                xDstPosi, yDstPosi,
                                                                                defaultMotionPressure);
                        slideDirection = false;
                    } else {
                        std::cout << "slideDirection:" << " go" << std::endl;
                        result = WuKongEventManager::GetInstance()->MotionEvent(xDstPosi, yDstPosi,
                                                                                xSrcPosi, ySrcPosi,
                                                                                defaultMotionPressure);
                        slideDirection = true;
                    }
                    usleep(intervalArgs * oneMs);
                } else {
                    int maxPercent = 100;
                    randomPercent = (float)(rand() % maxPercent) / maxPercent;
                    if ((randomPercent < touchPercent) || singleAppFlag) {
                        touchX = rand() % windowsWidth;
                        touchY = rand() % windowsHeight;
                        std::cout << "touch: (" << touchX << ", " << touchY << ")" << std::endl;
                        result = WuKongEventManager::GetInstance()->TouchEvent(touchX, touchY);
                        usleep(intervalArgs * oneMs);
                        continue;
                    }
                    singleAppFlag = g_commandBUNDLEENABLE;
                    index = getAbilityIndex();
                    if (index == -1) {
                        std::cout << "Not Found " << bundleNameArgs << ", please check bundle name!" << std::endl;
                        result = OHOS::ERR_INVALID_VALUE;
                        break;
                    }
                    result = WuKongAppManager::GetInstance()->StartAbilityByBundleInfo(abilityList[index],
                                                                                       bundleList[index]);
                    result = PrintResultOfStartAbility(result, index);
                    usleep(intervalArgs * oneMs);
                }
            }
            WuKongExceptionManager::GetInstance()->StopCatching();

            APP_LOGI("%{public}s ends", __func__);
            return result;
        }

        ErrCode WuKongCommand::PrintResultOfStartAbility(const ErrCode result, int index)
        {
            if (result == OHOS::ERR_OK) {
                std::cout << "bundleName: " << bundleList[index] << " startup succeed" << std::endl;
            } else {
                std::cout << "bundleName: " << bundleList[index] << " startup fail" << std::endl;
            }
            return OHOS::ERR_OK;
        }

        void WuKongCommand::checkPosition(int width, int height)
        {
            if (xSrcPosi < 0) {
                xSrcPosi = 0;
            }

            if (ySrcPosi < 0) {
                ySrcPosi = 0;
            }

            if (xSrcPosi > width) {
                xSrcPosi = width;
            }

            if (ySrcPosi > height) {
                ySrcPosi = height;
            }
        }

        int WuKongCommand::getAbilityIndex()
        {
            int index;
            if (g_commandBUNDLEENABLE) {
                index = FindElement(bundleList, bundleNameArgs);
            } else {
                index = rand() % bundleList.size();
                while (visibleList[index] == "false") {
                    index = rand() % bundleList.size();
                }
            }
            return index;
        }

        ErrCode WuKongCommand::RunTouchCommand()
        {
            int result = WuKongEventManager::GetInstance()->TouchEvent(xSrcPosi, ySrcPosi);
            return result;
        }

        ErrCode WuKongCommand::RunAsHelpCommand()
        {
            resultReceiver_ = WUKONG_HELP_MSG;
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::GetWuKongVersion()
        {
            resultReceiver_ = WUKONG_TOOL_VERSION;
            return OHOS::ERR_OK;
        }

        int WuKongCommand::FindElement(std::vector<std::string> bundleList, std::string key)
        {
            std::vector<std::string>::iterator it;
            std::cout << "BundleName: " << key << std::endl;
            it = find(bundleList.begin(), bundleList.end(), key);
            if (it != bundleList.end()) {
                return distance(bundleList.begin(), it);
            }
            return -1;
        }

        sptr<IBundleMgr> WuKongCommand::GetBundleMgrProxy() const
        {
            sptr<ISystemAbilityManager> systemAbilityManager =
                    SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (!systemAbilityManager) {
                APP_LOGE("failed to get system ability mgr.");
                return nullptr;
            }

            sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(
                BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
            if (!remoteObject) {
                APP_LOGE("failed to get bundle manager proxy.");
                return nullptr;
            }

            APP_LOGI("get bundle manager proxy success.");
            return iface_cast<IBundleMgr>(remoteObject);
        }
    }
}
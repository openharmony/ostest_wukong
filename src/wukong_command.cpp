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


#include <iostream>
#include <getopt.h>
#include <random>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>
#include <regex>
#include <typeinfo>

#include "wukong_app_manager.h"
#include "wukong_event_manager.h"
#include "wukong_exception_manager.h"
#include "bundle_mgr_proxy.h"
#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "launcher_service.h"

#include "wukong_command.h"


using namespace OHOS::AAFwk;

namespace OHOS {
    namespace AppExecFwk {
        namespace {
            bool g_commandSEEDENABLE = false;
            bool g_commandCOUNTENABLE = false;
            bool g_commandBUNDLEENABLE = false;
            bool g_commandELEMENTSEARCH = false;
            bool g_commandHELPENABLE = false;
            bool g_commandVERSIONENABLE = false;
            bool g_commandAPPSWITCHENABLE = false;
            bool g_commandTOUCHENABLE = false;
            bool g_commandINSOMNIAENABLE = false;
            bool g_commandMOTIONENABLE = false;
            bool g_commandXENABLE = false;
            bool g_commandYENABLE = false;
            bool g_commandTouch = false;
            bool g_commandMOUSE = false;
            bool g_commandKEYBOARD = false;
            bool g_commandTIME = false;

            const std::string SHORT_OPTIONS = "a:b:c:ehi:s:t:vkl:x:y:m:n:o:p:T";
            const struct option LONG_OPTIONS[] = {
                {"appswitch", required_argument, nullptr, 'a'},
                {"bundle", required_argument, nullptr, 'b'},
                {"count", required_argument, nullptr, 'c'},
                {"element_search", no_argument, nullptr, 'e'},
                {"help", no_argument, nullptr, 'h'},
                {"interval", required_argument, nullptr, 'i'},
                {"seed", required_argument, nullptr, 's'},
                {"touch", required_argument, nullptr, 't'},
                {"version", no_argument, nullptr, 'v'},
                {"wake_sleep", no_argument, nullptr, 'k'},
                {"motion", required_argument, nullptr, 'l'},
                {"xSrc", required_argument, nullptr, 'x'},
                {"ySrc", required_argument, nullptr, 'y'},
                {"xDst", required_argument, nullptr, 'm'},
                {"mouse", required_argument, nullptr, 'o'},
                {"keyboard", required_argument, nullptr, 'p'},
                {"yDst", required_argument, nullptr, 'n'},
                {"touch_pos", no_argument, nullptr, 'u'},
                {"time", no_argument, nullptr, 'T'},
            };

            std::string DEFAULT_DIR = "/data/local/wukong";
            std::ofstream outFile;
            int64_t timeTemp = -1;

            int64_t GetMillisTime() {
                auto timeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
                auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
                return tmp.count();
            }

            class TestUtils {
            public:
                TestUtils() = default;
                virtual ~TestUtils() = default;
                static std::vector<std::string> split(const std::string &in, const std::string &delim) {
                    std::regex reg(delim);
                    std::vector<std::string> res = {
                        std::sregex_token_iterator(in.begin(), in.end(), reg, -1), std::sregex_token_iterator()};
                    return res;
                }
            };


            class touchEventInfo
            {
                public:
                struct eventData {
                    int xPosi;
                    int yPosi;
                    int interval = 1;
                };


                static void WriteEventHead(std::ofstream &outFile) {
                    outFile << "x" << ',';
                    outFile << "y" << ',';
                    outFile << "interval" << std::endl;
                }

                static void WriteEventData(std::ofstream &outFile, const eventData &data) {
                    outFile << data.xPosi << ',';
                    outFile << data.yPosi << ',';
                    outFile << data.interval << std::endl;
                }

                static void ReadEventLine(std::ifstream &inFile) {
                    char buffer[50];
                    int xPosi = -1;
                    int yPosi = -1;
                    int interval = -1;
                    bool jumpFlag = true;
                    while (!inFile.eof()) {
                        inFile >> buffer;
                        if (jumpFlag) {
                            jumpFlag = !jumpFlag;
                            continue;
                        }
                        jumpFlag = !jumpFlag;
                        std::string delim = ",";
                        auto caseInfo = TestUtils::split(buffer, delim);
                        xPosi = std::stoi(caseInfo[0]);
                        yPosi = std::stoi(caseInfo[1]);
                        interval = std::stoi(caseInfo[2]);

                        std::cout << xPosi << ";"
                                  << yPosi << ";"
                                  << interval << std::endl;
                        int result = WuKongEventManager::GetInstance()->TouchEvent(xPosi, yPosi);
                        usleep(interval * 1000);
                        result += 1;
                    }
                }
            };

            bool InitReportFolder()
            {
                DIR *rootDir = nullptr;
                if ((rootDir = opendir(DEFAULT_DIR.c_str())) == nullptr) {
                    int ret = mkdir(DEFAULT_DIR.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
                    if (ret != 0) {
                        std::cerr << "failed to create dir: " << DEFAULT_DIR << std::endl;
                        return false;
                    }
                }
            return true;
            }

            bool InitEventRecordFile(std::ofstream &outFile) {
                if (!InitReportFolder()) {
                    return false;
                }
                std::string filePath = DEFAULT_DIR + "/EventRecord" + ".csv";
                outFile.open(filePath, std::ios_base::out | std::ios_base::trunc);
                if (!outFile) {
                    std::cerr << "Failed to create csv file at:" << filePath << std::endl;
                    return false;
                }
                touchEventInfo::WriteEventHead(outFile);
                std::cout << "The result will be written in csv file at location: " << filePath << std::endl;
                return true;
            }
        }

        class InputEventCallback : public MMI::IInputEventConsumer {
        public:
            virtual void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override
            {
                std::cout << "keyCode" << keyEvent->GetKeyCode() << std::endl;
            }
            virtual void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override
            {
                MMI::PointerEvent::PointerItem item;
                bool result = pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
                if (!result) {
                    std::cout << "GetPointerItem Fail" << std::endl;
                }
                touchEventInfo::eventData data {};
                int64_t currentTime = GetMillisTime();
                if (timeTemp == -1) {
                    timeTemp = currentTime;
                    data.interval = 1000;
                } else {
                    data.interval = currentTime - timeTemp;
                    timeTemp = currentTime;
                }
                data.xPosi = item.GetGlobalX();
                data.yPosi = item.GetGlobalY();
                touchEventInfo::WriteEventData(outFile, data);
                std::cout << " PointerEvent received."
                          << " interval: " << data.interval
                          << " xPosi:" << data.xPosi
                          << " yPosi:" << data.yPosi << std::endl;
            }
            virtual void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override {}
            static std::shared_ptr<InputEventCallback> GetPtr();
        };

        std::shared_ptr<InputEventCallback> InputEventCallback::GetPtr()
        {
            return std::make_shared<InputEventCallback>();
        }

        WuKongCommand::WuKongCommand(int argc, char *argv[]) : ShellCommand(argc, argv, WUKONG_TOOL_NAME), eventlist(100, EVENTTYPE_INVALIDEVENT)
        {}

        ErrCode WuKongCommand::init()
        {
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::StartRecord()
        {
            if (!InitEventRecordFile(outFile)) {
                return OHOS::ERR_INVALID_VALUE;
            }
            auto callBackPtr = InputEventCallback::GetPtr();
            int32_t id1 = MMI::InputManager::GetInstance()->AddMonitor(callBackPtr);
            std::cout << id1 << std::endl;
            int flag = getc(stdin);
            std::cout << flag << std::endl;
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::StartReplay()
        {
            std::ifstream inFile(DEFAULT_DIR + "/EventRecord" + ".csv");
            touchEventInfo::ReadEventLine(inFile);
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::CreateCommandMap()
        {
            commandMap_ = {
                    {"-v", std::bind(&WuKongCommand::GetWuKongVersion, this)},
                    {"help", std::bind(&WuKongCommand::RunAsHelpCommand, this)},
                    {"exec", std::bind(&WuKongCommand::RunAsParseCommand, this)},
                    {"appinfo", std::bind(&WuKongCommand::ShowAllAppInfo, this)},
                    {"record", std::bind(&WuKongCommand::StartRecord, this)},
                    {"replay", std::bind(&WuKongCommand::StartReplay, this)},
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
            int result = OHOS::ERR_OK;
            
            int counter = 0;

            while (true) {
                int option = -1;
                counter++;
                option = getopt_long(argc_, argv_, SHORT_OPTIONS.c_str(), LONG_OPTIONS, nullptr);

                // process error
                if (optind < 0 || optind > argc_) {
                    return OHOS::ERR_INVALID_VALUE;
                }

                // process error
                if (option == -1) {
                    if (counter == 1 && strcmp(argv_[optind], cmd_.c_str()) == 0) {
                        // 'wukong exec' with no option: wukong exec
                        // 'wukong exec' with a wrong argument: wukong exec xxx
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
            result = InitEventCount();

            if (result == OHOS::ERR_OK) {
                result = RunAsExecCommand();
                resultReceiver_.append(STRING_WUKONG_INFO_OK);
            } else {
                resultReceiver_.append(STRING_WUKONG_INFO_NG);
            }
            return result;
        }

        ErrCode WuKongCommand::HandleUnknownOption(const char optopt)
        {
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
                case 'o':
                case 'p':
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
            return result;
        }

        ErrCode WuKongCommand::HandleNormalOption(const int option)
        {
            int result = OHOS::ERR_OK;
            switch (option) {
                case 'a': {
                    g_commandAPPSWITCHENABLE = true;
                    appswitchPercent = std::stof(optarg);
                    break;
                }
                case 'b': {
                    if (optarg) {
                        std::cout << "get bundleNameArgs =" << optarg << std::endl;
                        SplitString(optarg, bundleNameArgs, ",");
                    }
                    if (bundleNameArgs.size() >= 0) {
                        g_commandBUNDLEENABLE = true;
                    }
                    break;
                }
                case 'c': {
                    countArgs = std::stoi(optarg);
                    g_commandCOUNTENABLE = true;
                    total = countArgs;
                    break;
                }
                case 'e': {
                    g_commandELEMENTSEARCH = true;
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
                    motionPercent = std::stof(optarg);
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
                case 'u': {
                    g_commandTouch = true;
                    break;
                }
                case 'o': {
                    g_commandMOUSE = true;
                    mousePercent = std::stof(optarg);
                    break;
                }
                case 'p': {
                    g_commandKEYBOARD = true;
                    keyboardPercent = std::stof(optarg);
                    break;
                }
                case 'T': {
                    total = 100;
                    g_commandTIME = true;
                    break;
                }
            }
            return result;
        }

        ErrCode WuKongCommand::ShowAllAppInfo()
        {
            int result = GetAllAppInfo();
            if (result == OHOS::ERR_OK) {
                for (unsigned index = 0; index < bundleList.size(); index++) {
                    std::cout << "BundleName:  " << bundleList[index] << std::endl;
                    std::cout << "AbilityName:  " << abilityList[index] << std::endl;
                }
            } else {
                result = OHOS::ERR_INVALID_VALUE;
            }
            return result;
        }

        ErrCode WuKongCommand::GetAllAppInfo()
        {
            LauncherService launcherservice;
            int32_t userId = 100;
            std::vector<LauncherAbilityInfo> launcherAbilityInfos;
            launcherservice.GetAllLauncherAbilityInfos(userId, launcherAbilityInfos);
            for (auto item : launcherAbilityInfos) {
                bundleList.push_back(item.elementName.GetBundleName());
                abilityList.push_back(item.elementName.GetAbilityName());
            }
            return OHOS::ERR_OK;
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

            if(g_commandELEMENTSEARCH) {
                int result = RunElementCommand();
                return result;
            }
            int result = GetAllAppInfo();

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

            bool startReuslt = WuKongExceptionManager::GetInstance()->StartCatching();
            if (!startReuslt) {
                std::cerr << "[Error] failed to start catching" << std::endl;
                return OHOS::ERR_NO_INIT;
            }

            for (int i = 1; i <= total; i++) {
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
                } else if (g_commandTIME == true) {
                    RandomInject();
                } else {
                    int eventTypeListIndex = rand()%eventTypeList.size();
                    eventTypeId = eventTypeList[eventTypeListIndex];

                    switch (eventTypeId) {
                        case EVENTTYPE_TOUCHEVENT:
                            if (touchCount > 0) {
                                touchCount --;
                                std::cout << "touch remain count :" << touchCount << std::endl;
                                if (touchCount < 1) {
                                    eventTypeList.erase (eventTypeList.begin() + eventTypeListIndex);
                                }

                                touchX = rand() % windowsWidth;
                                touchY = rand() % windowsHeight;

                                result = WuKongEventManager::GetInstance()->TouchEvent(touchX, touchY);
                                std::cout << "touch: (" << touchX 
                                          << ", " << touchY << ")" <<  std::endl;
                                usleep(intervalArgs * oneMs);
                            }
                            break;
                        case EVENTTYPE_MOTIONEVENT:
                            if (motionCount > 0) {
                                motionCount --;
                                std::cout << "motion remain count :" << motionCount << std::endl;
                                if (motionCount < 1) {
                                    eventTypeList.erase (eventTypeList.begin() + eventTypeListIndex);
                                }

                                xSrcPosition = rand() % windowsWidth;
                                ySrcPosition = rand() % windowsHeight;
                                xDstPosition = rand() % windowsWidth;
                                yDstPosition = rand() % windowsHeight;

                                result = WuKongEventManager::GetInstance()->MotionEvent(
                                    xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
                                std::cout << "motion: (" << xSrcPosition << ", " << ySrcPosition << ")" 
                                          << "->" << "(" << xDstPosition << ", " << yDstPosition << ")"  <<  std::endl;
                                usleep(intervalArgs * oneMs);
                            }
                            break;
                        case EVENTTYPE_MOUSEEVENT:
                            if (mouseCount > 0) {
                                mouseCount --;
                                std::cout << "mouse remain count :" << mouseCount << std::endl;
                                if (mouseCount < 1) {
                                    eventTypeList.erase (eventTypeList.begin() + eventTypeListIndex);
                                }

                                int type = rand() % MOUSETYPECOUNT;
                                xClickPosition = rand() % windowsWidth;
                                yClickPosition = rand() % windowsHeight;

                                result = WuKongEventManager::GetInstance()->MouseEvent(xClickPosition, yClickPosition, type);
                                std::cout << "mouse: (" << xClickPosition << ", " 
                                                        << yClickPosition << ")" << ", type:" << type <<  std::endl;
                                usleep(intervalArgs * oneMs);
                            }
                            break;
                        case EVENTTYPE_KEYBOARDEVENT:
                            if (keyboardCount > 0) {
                                keyboardCount --;
                                std::cout << "keyboard remain count :" << keyboardCount << std::endl;
                                if (keyboardCount < 1) {
                                    eventTypeList.erase (eventTypeList.begin() + eventTypeListIndex);
                                }
                                std::vector<int> keycodelist;
                                WuKongEventManager::GetInstance()->GetKeycodeList(keycodelist);
                                if (keycodelist.size() > 0) {
                                    int keycode = keycodelist[rand() % keycodelist.size()];
                                    result = WuKongEventManager::GetInstance()->KeyBoardEvent(keycode);
                                    std::cout << "keycode: " << keycode << std::endl;
                                }
                                usleep(intervalArgs * oneMs);
                            }
                            break;
                        case EVENTTYPE_APPSWITCHEVENT:
                            if (appswitchCount > 0) {
                                appswitchCount --;
                                std::cout << "appswitch remain count:" << appswitchCount << std::endl;
                                if (appswitchCount < 1) {
                                    eventTypeList.erase (eventTypeList.begin() + eventTypeListIndex);
                                }
                                if (g_commandBUNDLEENABLE == true && bundleNameArgs.size() > 0) {
                                    for (unsigned int bundleIndex = 0; 
                                         bundleIndex < bundleNameArgs.size(); 
                                         bundleIndex++) {
                                        GetAllAppInfo();
                                        int index = FindElement(bundleList, bundleNameArgs[bundleIndex]);
                                    result = WuKongAppManager::GetInstance()->StartAbilityByBundleInfo(
                                                                                    abilityList[index],
                                                                                    bundleList[index]);
                                    result = PrintResultOfStartAbility(result, index);
                                    }
                                } else {
                                    index = getAbilityIndex();
                                    if (index == -1) {
                                        std::cout << "Not Found  bundle, please check bundle name!" << std::endl;
                                        result = OHOS::ERR_INVALID_VALUE;
                                        break;
                                    }
                                    result = WuKongAppManager::GetInstance()->StartAbilityByBundleInfo(
                                                                                    abilityList[index],
                                                                                    bundleList[index]);
                                    result = PrintResultOfStartAbility(result, index);
                                }
                                usleep(intervalArgs * oneMs);
                            }
                            break;
                    }
                }
            }
            WuKongExceptionManager::GetInstance()->StopCatching();

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
            int index = -1;
            
            if (bundleList.size() != 0) {
                if (g_commandBUNDLEENABLE) {
                    if (bundleNameArgs.size() != 0) {
                        index = FindElement(bundleList, bundleNameArgs.at(rand() % bundleNameArgs.size()));
                    }
                } else {
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

        ErrCode WuKongCommand::RunElementCommand()
        {
            int result = WuKongEventManager::GetInstance()->ElementEvent();
            return result;
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

        void WuKongCommand::SplitString(const std::string& optarg, 
                                        std::vector<std::string> &bundleNameArgs, 
                                        const std::string& c)
        {
            std::string::size_type start, end;
            end = optarg.find(c);
            start = 0;
            while (std::string::npos != end) {
                if (end != start) {
                    auto found = std::find(bundleNameArgs.begin(), bundleNameArgs.end(), optarg.substr(start, end-start));
                    if (bundleNameArgs.end() != found) {
                        std::cout<<"repeat bundle name"<<std::endl;
                        break;
                    }
                    bundleNameArgs.push_back(optarg.substr(start, end-start));
                    std::cout<<optarg.substr(start, end-start)<<std::endl;
                    
                    start = end + c.size();
                    end = optarg.find(c, start);
                } else {
                    std::cout<<"empty item"<<std::endl;

                    start = end + c.size();
                    end = optarg.find(c, start);
                }
            }
            if (start != optarg.length()) {
                auto found =std::find(bundleNameArgs.begin(), bundleNameArgs.end(), optarg.substr(start));
                if (bundleNameArgs.end() != found) {
                    std::cout<<"repeat bundle name"<<std::endl;
                } else {
                    bundleNameArgs.push_back(optarg.substr(start));
                }
            } 

            GetAllAppInfo();

            for (unsigned int i = 0; i < bundleNameArgs.size(); i++) {
                std::cout<<"search for "<<bundleNameArgs[i]<<std::endl;
                
                int index = FindElement(bundleList, bundleNameArgs[i]);

                std::cout<<"index = "<<index<<std::endl;
                if (-1 != index) {
                    std::cout<<"has "<<bundleNameArgs[i]<<std::endl;
                    continue;
                }
                std::cout<<"delete "<<bundleNameArgs[i]<<std::endl;
                bundleNameArgs.erase(find(bundleNameArgs.begin(), bundleNameArgs.end(), bundleNameArgs.at(i)));
            }

            std::cout<<"final get -b para with "<<bundleNameArgs.size()<<" items"<<std::endl;

        }

        ErrCode WuKongCommand::InitEventCount()
        {
            WuKongEventManager::GetInstance()->KeyCodeInit();
            float sumPercent = 0.0;
            int sumCount = 0;
            if (touchPercent > 0) {
                eventTypeList.push_back(EVENTTYPE_TOUCHEVENT);
                sumPercent += touchPercent;
                touchPercentDefult = 0;
                touchCount = (int)(touchPercent * total);
                sumCount +=  touchCount;
            }
            if (appswitchPercent > 0) {
                eventTypeList.push_back(EVENTTYPE_APPSWITCHEVENT);
                sumPercent += appswitchPercent;
                appswitchPercentDefult = 0;
                appswitchCount = (int)(appswitchPercent * total);
                sumCount +=  appswitchCount;
            }
            if (motionPercent > 0) {
                eventTypeList.push_back(EVENTTYPE_MOTIONEVENT);
                sumPercent += motionPercent;
                motionPercentDefult = 0;
                motionCount = (int)(motionPercent * total);
                sumCount +=  motionCount;
            }
            if (mousePercent > 0) {
                eventTypeList.push_back(EVENTTYPE_MOUSEEVENT);
                sumPercent += mousePercent;
                mousePercentDefult = 0;
                mouseCount = (int)(mousePercent * total);
                sumCount +=  mouseCount;
            }
            if (keyboardPercent > 0) {
                eventTypeList.push_back(EVENTTYPE_KEYBOARDEVENT);
                sumPercent += keyboardPercent;
                keyboardPercentDefult = 0;
                keyboardCount = (int)(keyboardPercent * total);
                sumCount +=  keyboardCount;
            }
            if (sumPercent > 1) {
                std::cout << "total percentage is over 1, please reset parameters!" << std::endl;
                return OHOS::ERR_INVALID_VALUE;
            }

            float defaultPercent = touchPercentDefult + appswitchPercentDefult + 
                                   motionPercentDefult + mousePercentDefult + 
                                   keyboardPercentDefult;

            if (defaultPercent > 0 && defaultPercent <= 1 && sumPercent < 1) {
                int lostCount = total - sumCount;
                if (touchPercentDefult > 0) {
                    eventTypeList.push_back(EVENTTYPE_TOUCHEVENT);
                    touchCount = (int)((touchPercentDefult / defaultPercent) * lostCount);
                }
                if (appswitchPercentDefult > 0) {
                    eventTypeList.push_back(EVENTTYPE_APPSWITCHEVENT);
                    appswitchCount = (int)((appswitchPercentDefult / defaultPercent) * lostCount);
                }
                if (motionPercentDefult > 0) {
                    eventTypeList.push_back(EVENTTYPE_MOTIONEVENT);
                    motionCount = (int)((motionPercentDefult / defaultPercent) * lostCount);
                }
                if (mousePercentDefult > 0) {
                    eventTypeList.push_back(EVENTTYPE_MOUSEEVENT);
                    mouseCount = (int)((mousePercentDefult / defaultPercent) * lostCount);
                }
                if (keyboardPercentDefult > 0) {
                    eventTypeList.push_back(EVENTTYPE_KEYBOARDEVENT);
                    keyboardCount = (int)((keyboardPercentDefult / defaultPercent) * lostCount);
                }
            }
            if ((total - (keyboardCount + mouseCount + motionCount + touchCount + appswitchCount )) > 0) {
                touchCount += total - (keyboardCount + mouseCount + motionCount + touchCount + appswitchCount);
            }
            std::cout  << "keyboardCount :" << keyboardCount 
                       << ", mouseCount :" << mouseCount 
                       << ", motionCount :" << motionCount 
                       << ", touchCount :" << touchCount 
                       << ", appswitchCount :" << appswitchCount << std::endl;
            return OHOS::ERR_OK;
        }

        ErrCode WuKongCommand::RandomInject()
        {
            int result = OHOS::ERR_OK;
            const int windowsWidth = 720;
            const int windowsHeight = 1080;
            int touchX = -1;
            int touchY = -1;
            int oneMs = 1000;
            int touchcount = 0;
            int motioncount = 0;
            int mousecount = 0;
            int appswitchcount = 0;
            int  keyboardcount = 0;
            int sumcount = 0;
            for (int index = 0; index < 100; index++) {
                if (touchCount > 0) {
                    eventlist[index] = EVENTTYPE_TOUCHEVENT;
                    touchCount --;
                    std::cout << "touchCount:" << touchCount << std::endl;
                } else if (appswitchCount > 0) {
                    eventlist[index] = EVENTTYPE_APPSWITCHEVENT;
                    appswitchCount --;
                    std::cout << "appswitchCount:" << appswitchCount << std::endl;
                } else if (motionCount > 0) {
                    eventlist[index] = EVENTTYPE_MOTIONEVENT;
                    motionCount --;
                    std::cout << "motionCount:" << motionCount << std::endl;
                } else if (mouseCount > 0) {
                    eventlist[index] = EVENTTYPE_MOUSEEVENT;
                    mouseCount --;
                    std::cout << "mouseCount:" << mouseCount << std::endl;
                } else if (keyboardCount > 0) {
                    eventlist[index] = EVENTTYPE_KEYBOARDEVENT;
                    keyboardCount --;
                    std::cout << "keyboardCount:" << keyboardCount << std::endl;
                }
                std::cout << "index:" << index <<  "eventlist.at(index):" << eventlist.at(index) << std::endl;
            }
            RandomShuffle(eventlist);
            for (auto it = eventlist.begin(); it != eventlist.end(); ++it) {
                std::cout  << *it ;
            }
            while (1) {
                sumcount++;
                std::cout << "sumcount: " << sumcount  <<  std::endl;
                int eventindex = rand() % 100;
                eventTypeId = eventlist.at(eventindex);
                int index = 0;
                switch (eventTypeId) {
                    case EVENTTYPE_TOUCHEVENT: {
                        touchX = rand() % windowsWidth;
                        touchY = rand() % windowsHeight;
                        result = WuKongEventManager::GetInstance()->TouchEvent(touchX, touchY);
                        std::cout << "touch: (" << touchX << ", " << touchY << ")" <<  std::endl;
                        touchcount ++;
                        std::cout << "touchcount: " << touchcount  <<  std::endl;
                        float touchpercent = (float)touchcount / sumcount;
                        std::cout << "touchpercent: " << touchpercent  <<  std::endl;
                        usleep(intervalArgs * oneMs);
                        break;
                    }
                    case EVENTTYPE_MOTIONEVENT: {
                        xSrcPosition = rand() % windowsWidth;
                        ySrcPosition = rand() % windowsHeight;
                        xDstPosition = rand() % windowsWidth;
                        yDstPosition = rand() % windowsHeight;

                        result = WuKongEventManager::GetInstance()->MotionEvent(
                            xSrcPosition, ySrcPosition, xDstPosition, yDstPosition);
                        std::cout << "motion: (" << xSrcPosition << ", " << ySrcPosition << ")" 
                                  << "->" << "(" << xDstPosition << ", " << yDstPosition << ")"  <<  std::endl;
                        motioncount ++;
                        std::cout << "motioncount: " << motioncount  <<  std::endl;
                        float motionpercent = (float)motioncount / sumcount;
                        std::cout << "motionpercent: " << motionpercent  <<  std::endl;
                        usleep(intervalArgs * oneMs);
                        break;
                    }
                    case EVENTTYPE_MOUSEEVENT: {
                        int type = rand() % MOUSETYPECOUNT;
                        xClickPosition = rand() % windowsWidth;
                        yClickPosition = rand() % windowsHeight;

                        result = WuKongEventManager::GetInstance()->MouseEvent(xClickPosition, 
                                                                               yClickPosition, 
                                                                               type);
                        std::cout << "mouse: (" << xClickPosition << ", "
                                                << yClickPosition << ")" << ", type:" << type <<  std::endl;
                        mousecount ++;
                        float mousepercent = (float)mousecount / sumcount;
                        std::cout << "mousepercent: " << mousepercent  <<  std::endl;
                        std::cout << "mousecount: " << mousecount  <<  std::endl;
                        usleep(intervalArgs * oneMs);
                        break;
                    }
                    case EVENTTYPE_KEYBOARDEVENT: {
                        std::vector<int> keycodelist;
                        WuKongEventManager::GetInstance()->GetKeycodeList(keycodelist);
                        std::cout << "keyboard size :" << keycodelist.size() << std::endl;
                        if (keycodelist.size() > 0) {
                            int keycode = keycodelist[rand() % keycodelist.size()];
                            result = WuKongEventManager::GetInstance()->KeyBoardEvent(keycode);
                            std::cout << "keycode: " << keycode << std::endl;
                            keyboardcount ++;
                            std::cout << "keyboardcount: " << keyboardcount  <<  std::endl;
                            float keyboardpercent = (float)keyboardcount / sumcount;
                            std::cout << "keyboardpercent: " << keyboardpercent  <<  std::endl;
                            usleep(intervalArgs * oneMs);
                        }
                        break;
                    }
                    case EVENTTYPE_APPSWITCHEVENT: {
                        index = getAbilityIndex();
                        if (index == -1) {
                            std::cout << "Not Found  bundle, please check bundle name!" << std::endl;
                            result = OHOS::ERR_INVALID_VALUE;
                            break;
                        }
                        result = WuKongAppManager::GetInstance()->StartAbilityByBundleInfo(
                                                                        abilityList[index],
                                                                        bundleList[index]);
                        result = PrintResultOfStartAbility(result, index);
                        appswitchcount ++;
                        std::cout << "appswitchcount: " << appswitchcount  <<  std::endl;
                        float appswitchpercent = (float)appswitchcount / sumcount;
                        std::cout << "appswitchpercent: " << appswitchpercent  <<  std::endl;
                        usleep(intervalArgs * oneMs);
                        break;
                    }
                }
            }
            return result;
        }
        void WuKongCommand::RandomShuffle( std::vector<int> &eventlist)
        {
            for (int i = eventlist.size() - 1 ; i > 0; --i) {
                std::swap(eventlist[i], eventlist[std::rand() % (i+1)]);
            }
        }
    }
}

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

#ifndef WUKONG_WUKONG_COMMAND_H
#define WUKONG_WUKONG_COMMAND_H

#include "shell_command.h"
#include "app_log_wrapper.h"
#include "bundle_mgr_interface.h"

#include <map>
#include <vector>


namespace OHOS {
    namespace AppExecFwk {
        namespace {
            const std::string WUKONG_TOOL_NAME = "wukong";

            const std::string WUKONG_TOOL_VERSION = "1.0.1\n";

            const std::string WUKONG_HELP_MSG = "usage: wukong exec <options> <arguments>\n"
                                                "These are common wukong options list:\n"
                                                "  -a, appswitch event percent\n"
                                                "  -b, the whitelist of  bundle name\n"
                                                "  -e, element search\n"
                                                "  -t, touch event percent\n"
                                                "  -c, count\n"
                                                "  -h, help\n"
                                                "  -i, interval\n"
                                                "  -s, seed\n"
                                                "  -o, mouse event\n"
                                                "  -p, keyboard event\n"
                                                "  -v, version\n"
                                                "  -l, motion event\n"
                                                "  -T, time\n"
                                                "  --wake_sleep, spec test: sleep and awake\n";

            const std::string STRING_WUKONG_INFO_OK = "wukong info successfully.\n";
            const std::string STRING_WUKONG_INFO_NG = "error: failed to wukong info.\n";
        }
        class WuKongCommand : public OHOS::AAFwk::ShellCommand {
        public:
            /**
             * @brief initialize class named WukongCommand.
             * @param argc
             * @param argv
             * @return -
             */
            WuKongCommand(int argc, char *argv[]);
            ~WuKongCommand() override
            {};
        private:
            // id of inject eventtype 
            enum EventTypeID {
                EVENTTYPE_TOUCHEVENT,
                EVENTTYPE_MOTIONEVENT,
                EVENTTYPE_MOUSEEVENT,
                EVENTTYPE_KEYBOARDEVENT,
                EVENTTYPE_ELEMENTEVENT,
                EVENTTYPE_APPSWITCHEVENT,
                EVENTTYPE_INVALIDEVENT
            };
            ErrCode init() override;
            ErrCode CreateCommandMap() override;
            ErrCode CreateMessageMap() override;

            ErrCode RunAsParseCommand();
            ErrCode RunAsExecCommand();
            ErrCode RunAsHelpCommand();
            ErrCode RunTouchCommand();
            ErrCode RunElementCommand();

            ErrCode GetWuKongVersion();
            ErrCode GetAllAppInfo();

            ErrCode ShowAllAppInfo();
            ErrCode InsertEvent();

            ErrCode RandomInsertTouch();
            ErrCode RandomInsertMotion();
            ErrCode RandomSleepAndAwake();

            ErrCode HandleUnknownOption(const char optopt);
            ErrCode HandleNormalOption(const int option);
            ErrCode PrintResultOfStartAbility(const ErrCode result, int index);
            ErrCode InitEventCount();

            ErrCode StartRecord();
            ErrCode StartReplay();
            /**
             * @brief according to probability to  inject event
             * @return NO_ERROR: successful; otherwise is failed.
             */
            ErrCode RandomInject();

            /**
             * @brief randomly arrange elements in vector
             * @param eventlist the list of eventid
             * @return
             */
           void RandomShuffle(std::vector<int> &eventlist);

            /**
             * @brief split the -b parameters with & c and  check the validity of parameters
             * @param const std::string& optarg: -b parameters from getopt_long()
             * @param std::vector<std::string> &bundleNameArgs: -b parameters
             * @param const std::string& c: Separator
             * @return
             */
            void SplitString(const std::string& optarg, std::vector<std::string> &bundleNameArgs, const std::string& c);

            int FindElement(std::vector<std::string> bundleList, std::string key);

            int getAbilityIndex();
            void checkPosition(int width, int height);
            std::vector<std::string> bundleNameArgs;
            std::vector<std::string> bundleList;
            std::vector<std::string> abilityList;
            std::vector<int> eventTypeList;
            std::vector<int> defaultEventTypeList{EVENTTYPE_TOUCHEVENT, EVENTTYPE_MOTIONEVENT, EVENTTYPE_MOUSEEVENT, 
                EVENTTYPE_KEYBOARDEVENT, EVENTTYPE_APPSWITCHEVENT};
            int countArgs = 10;
            int intervalArgs = 1500;
            int seedArgs = -1;

            // default percentage of event
            float touchPercentDefult = 0.20;
            float mousePercentDefult = 0.20;
            float keyboardPercentDefult = 0.20;
            float motionPercentDefult = 0.20;
            float appswitchPercentDefult = 0.20;

            // percentage of event
            float touchPercent = -1;
            float mousePercent = -1;
            float keyboardPercent = -1;
            float motionPercent = -1;
            float appswitchPercent = -1;

            int xSrcPosi = 0;
            int ySrcPosi = 0;
            int xDstPosi = 0;
            int yDstPosi = 0;

            // motion position
            int xSrcPosition;
            int ySrcPosition;
            int xDstPosition;
            int yDstPosition;

            // mouse position
            int xClickPosition;
            int yClickPosition;

            // init enent type id
            int eventTypeId = EVENTTYPE_INVALIDEVENT;

            // count of event
            int touchCount = 0;
            int mouseCount = 0;
            int keyboardCount = 0;
            int motionCount = 0;
            int appswitchCount = 0;

            const int userId = 100;

            // count of mouse event
            const int MOUSETYPECOUNT = 3;

            // total event count
            int total = 10;

            // eventlist
            std::vector<int> eventlist;
        };
    }
}

#endif // WUKONG_WUKONG_COMMAND_H

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
                                                "  -b, specific the bundle name\n"
                                                "  -t, touch event percent\n"
                                                "  -c, count\n"
                                                "  -h, help\n"
                                                "  -i, interval\n"
                                                "  -s, seed\n"
                                                "  -v, version\n"
                                                "  --spec_insomnia, spec test: sleep and awake\n";

            const std::string STRING_WUKONG_INFO_OK = "wukong info successfully.\n";
            const std::string STRING_WUKONG_INFO_NG = "error: failed to wukong info.\n";
        }
        class WuKongCommand : public OHOS::AAFwk::ShellCommand {
        public:
            WuKongCommand(int argc, char *argv[]);
            ~WuKongCommand() override
            {};
        private:
            ErrCode init() override;
            ErrCode CreateCommandMap() override;
            ErrCode CreateMessageMap() override;

            ErrCode RunAsParseCommand();
            ErrCode RunAsExecCommand();
            ErrCode RunAsHelpCommand();
            ErrCode RunTouchCommand();

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
            
            int FindElement(std::vector<std::string> bundleList, std::string key);

            int getAbilityIndex();
            void checkPosition(int width, int height);
            std::string bundleNameArgs = "";
            std::vector<std::string> bundleList;
            std::vector<std::string> abilityList;
            
            int countArgs = 10;
            int intervalArgs = 1500;
            int seedArgs = -1;
            float touchPercent = 0.50;
            float abilityPercent = 0.20;
            int xSrcPosi = 0;
            int ySrcPosi = 0;
            int xDstPosi = 0;
            int yDstPosi = 0;
            const int userId = 100;
        };
    }
}

#endif // WUKONG_WUKONG_COMMAND_H

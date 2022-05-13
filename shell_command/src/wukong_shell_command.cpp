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

#include "wukong_shell_command.h"
#include "ability_manager_client.h"
#include "appexecfwk_errors.h"
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"
#include "launcher_service.h"
#include "random_test_flow.h"
#include "semaphore_ex.h"
#include "special_test_flow.h"
#include "system_ability_definition.h"
#include "test_flow_factory.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string WUKONG_TOOL_NAME = "wukong";

const std::string WUKONG_TOOL_VERSION = "1.1.0\n";

const std::string WUKONG_HELP_MSG =
    "usage: wukong <command> [<arguments>]\n"
    "These are common wukong command list:\n"
    "   -h/help                    wukong help information\n"
    "   -v/--version               wukong version\n"
    "   exec                       run random test\n"
    "   special                    run special test\n"
    "   appinfo                    show all app information\n";
}  // namespace

WuKongShellCommand::WuKongShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, WUKONG_TOOL_NAME) {}

ErrCode WuKongShellCommand::init()
{
    return OHOS::ERR_OK;
}

ErrCode WuKongShellCommand::CreateCommandMap()
{
    TRACK_LOG_STD();
    commandMap_ = {
        {"--version", std::bind(&WuKongShellCommand::GetWuKongVersion, this)},
        {"-v", std::bind(&WuKongShellCommand::GetWuKongVersion, this)},
        {"help", std::bind(&WuKongShellCommand::RunAsHelpCommand, this)},
        {"stop", std::bind(&WuKongShellCommand::RunStopCommand, this)},
        {"exec", std::bind(&WuKongShellCommand::RunTestCommand, this)},
        {"special", std::bind(&WuKongShellCommand::RunTestCommand, this)},
        {"appinfo", std::bind(&WuKongShellCommand::ShowAllAppInfo, this)},
    };
    TRACK_LOG_END();
    return OHOS::ERR_OK;
}

ErrCode WuKongShellCommand::CreateMessageMap()
{
    TRACK_LOG_STD();
    messageMap_ = {
        {
            ERR_APPEXECFWK_INSTALL_INTERNAL_ERROR,
            "error: wukong internal error.",
        },
    };
    TRACK_LOG_END();
    return OHOS::ERR_OK;
}

ErrCode WuKongShellCommand::GetWuKongVersion()
{
    resultReceiver_ = WUKONG_TOOL_VERSION;
    return OHOS::ERR_OK;
}

ErrCode WuKongShellCommand::RunStopCommand()
{
    NamedSemaphore sem(SEMPHORE_STOP_NAME, 1);
    sem.Open();
    if (sem.GetValue() == 0) {
        sem.Post();
    }
    sem.Close();
    return OHOS::ERR_OK;
}

ErrCode WuKongShellCommand::RunTestCommand()
{
    TRACK_LOG_STD();
    int res = OHOS::ERR_OK;
    // get testFlow by cmd_ of ShellCommand
    std::shared_ptr<TestFlow> testFlow = TestFlowFactory::GetTestFlow(*this, cmd_);

    // check the command arguments
    // if argument is not ok, exit wukong command.
    res = testFlow->CheckVaildityCmd();
    if (res != OHOS::ERR_OK) {
        DEBUG_LOG("Command arguments is invaild and exit");
        return res;
    }

    // run test flow.
    res = testFlow->Run();
    if (res != OHOS::ERR_OK) {
        DEBUG_LOG("Test flow run failed");
    }
    TRACK_LOG_END();
    return res;
}

ErrCode WuKongShellCommand::RunAsHelpCommand()
{
    TRACK_LOG_STD();
    resultReceiver_.append(WUKONG_HELP_MSG);
    TRACK_LOG_END();
    return OHOS::ERR_OK;
}

char **WuKongShellCommand::GetArgv()
{
    return argv_;
}

int WuKongShellCommand::GetArgc()
{
    return argc_;
}

void WuKongShellCommand::ResultReceiverAppend(const std::string receiver)
{
    resultReceiver_.append(receiver);
}

ErrCode WuKongShellCommand::ShowAllAppInfo()
{
    TRACK_LOG_STD();
    ErrCode result = WuKongUtil::GetInstance()->GetAllAppInfo();
    std::vector<std::string> bundleList;
    std::vector<std::string> abilityList;

    WuKongUtil::GetInstance()->GetBundleList(bundleList, abilityList);
    if (result != OHOS::ERR_OK) {
        result = OHOS::ERR_INVALID_VALUE;
    }
    std::stringstream appInfo;
    for (unsigned index = 0; index < bundleList.size(); index++) {
        DEBUG_LOG_STR("Bundle Name: (%s), Ability Name: (%s)", bundleList[index].c_str(), abilityList[index].c_str());
        appInfo << "BundleName:  " << bundleList[index] << std::endl;
        appInfo << "AbilityName:  " << abilityList[index] << std::endl;
    }
    resultReceiver_.append(appInfo.str());
    TRACK_LOG_END();
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

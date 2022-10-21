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

#include <cstdlib>

#include "ability_manager_client.h"
#include "accessibility_element_info.h"
#include "accessibility_ui_test_ability.h"
#include "appexecfwk_errors.h"
#include "bundle_mgr_proxy.h"
#include "component_manager.h"
#include "iservice_registry.h"
#include "launcher_service.h"
#include "random_test_flow.h"
#include "special_test_flow.h"
#include "system_ability_definition.h"
#include "test_flow_factory.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string WUKONG_TOOL_NAME = "wukong";

const std::string WUKONG_TOOL_VERSION = "version: 3.2.0.0\n";

const std::string ACE_ENABLE = "param set persist.ace.testmode.enabled 1";

const std::string WUKONG_HELP_MSG =
    "usage: wukong <command> [<arguments>]\n"
    "These are common wukong command list:\n"
    "   help                       wukong help information\n"
    "   -v/--version               wukong version\n"
    "   exec                       run random test\n"
    "   special                    run special test\n"
    "   appinfo                    show all app information\n";
}  // namespace

WuKongShellCommand::WuKongShellCommand(int argc, char *argv[]) : ShellCommand(argc, argv, WUKONG_TOOL_NAME)
{
}

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
    WukongSemaphore sem(SEMPHORE_STOP_NAME, 1);
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
    // get testFlow by cmd_ of ShellCommand
    std::shared_ptr<TestFlow> testFlow = TestFlowFactory::GetTestFlow(*this, cmd_);
    if (testFlow == nullptr) {
        ERROR_LOG_STR("GetTestFlow TestFlow is null command (%s)", cmd_.c_str());
        return OHOS::ERR_INVALID_VALUE;
    }

    auto cm = ComponentManager::GetInstance();
    if (cm == nullptr) {
        ERROR_LOG("cm is nullptr");
        return OHOS::ERR_INVALID_VALUE;
    }
    uint32_t handle = cm->AddRegisterListener(testFlow);

    // check the command arguments
    // if argument is not ok, exit wukong command.
    ErrCode res = testFlow->CheckVaildityCmd();
    if (res != OHOS::ERR_OK) {
        DEBUG_LOG("Command arguments is invalid and exit");
        return res;
    }

    // connect to accessibility
    if (!cm->Connect()) {
        ERROR_LOG("ComponentManager Connect failed");
        return OHOS::ERR_INVALID_OPERATION;
    }
    DEBUG_LOG("connected successfully");

    auto aacPtr = OHOS::Accessibility::AccessibilityUITestAbility::GetInstance();
    OHOS::Accessibility::AccessibilityElementInfo root;
    if (aacPtr->GetRoot(root) != Accessibility::RET_OK) {
        system(ACE_ENABLE.c_str());
    }
    // run test flow.
    res = testFlow->Run();
    if (res != OHOS::ERR_OK) {
        DEBUG_LOG("Test flow run failed");
    }
    cm->Disconnect();
    cm->DeleteRegisterListener(handle);

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
    if (result != OHOS::ERR_OK) {
        return result;
    }
    DEBUG_LOG_STR("GetAllAppInfo result: (%u)", result);
    std::vector<std::string> bundleList;
    std::vector<std::string> abilityList;
    std::string iconpath;
    WuKongUtil::GetInstance()->GetIconPath(iconpath);
    WuKongUtil::GetInstance()->GetBundleList(bundleList, abilityList);

    std::stringstream appInfo;
    DEBUG_LOG_STR("bundleList size: (%u)", bundleList.size());
    for (unsigned index = 0; index < bundleList.size(); index++) {
        DEBUG_LOG_STR("Bundle Name: (%s), Ability Name: (%s)", bundleList[index].c_str(), abilityList[index].c_str());
        appInfo << "BundleName:  " << bundleList[index] << std::endl;
        appInfo << "AbilityName:  " << abilityList[index] << std::endl;
        DEBUG_LOG_STR("IconPath: %s", iconpath.c_str());
    }
    resultReceiver_.append(appInfo.str());
    TRACK_LOG_END();
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

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

#include "component_input.h"

#include "component_manager.h"
#include "input_factory.h"
#include "input_manager.h"
#include "input_msg_object.h"
#include "report.h"
#include "scene_delegate.h"
#include "tree_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint32_t PAGE_BACK_COUNT_MAX = 3;
const uint32_t LANUCH_APP_COUNT_MAX = 5;

ErrCode LauncherApp(const std::string& bundleName)
{
    auto appInput = InputFactory::GetInputAction(INPUTTYPE_APPSWITCHINPUT);
    if (appInput == nullptr) {
        ERROR_LOG("InputFactory::GetInputAction INPUTTYPE_APPSWITCHINPUT is null");
        return OHOS::ERR_INVALID_VALUE;
    }

    // launch app by AppSwitchInput function.
    std::shared_ptr<AppSwitchParam> appSwitchParam = std::make_shared<AppSwitchParam>();
    appSwitchParam->bundlename_ = bundleName;
    std::shared_ptr<SpcialTestObject> sto = appSwitchParam;
    auto result = appInput->OrderInput(sto);
    if (result != OHOS::ERR_OK) {
        ERROR_LOG("AppSwitchInput OrderInput failed");
    }
    return result;
}

uint32_t CheckLauncherApp(const std::shared_ptr<ComponentParam>& param)
{
    TRACK_LOG_STD();
    std::vector<std::string> tempAllowList;
    for (uint32_t i = 0; i < param->bundleName_.size(); i++) {
        // do not launch app when bundle is running.
        if (param->bundleRunning_[i] == true && param->bundleFinish_[i] == false) {
            return i;
        }
        // launch app when the bundle is stop and not finish.
        if (param->bundleRunning_[i] == false && param->bundleFinish_[i] == false) {
            tempAllowList.clear();
            tempAllowList.push_back(param->bundleName_[i]);
            WuKongUtil::GetInstance()->SetTempAllowList(tempAllowList);
            // launch app by AppSwitchInput function.
            if (LauncherApp(param->bundleName_[i]) != OHOS::ERR_OK) {
                return param->bundleName_.size();
            }
            // init bundleRunning status to stop.
            std::vector<bool> bundleRunning = param->bundleRunning_;
            std::fill(bundleRunning.begin(), bundleRunning.end(), false);

            // set current launched bundle is running.
            param->bundleRunning_[i] = true;
            TRACK_LOG_STR("%s", param->toString().c_str());
            TEST_RUN_LOG(param->bundleName_[i].c_str());
            return i;
        }
    }
    // not found bundle can be run, and return failed.
    return param->bundleName_.size();
}

bool CheckAbliltyFinished(const std::shared_ptr<AbilityTree>& abilityNode)
{
    TRACK_LOG_STD();
    bool abilityRunFinished = false;
    if (abilityNode == nullptr) {
        ERROR_LOG("abilityNode is nullptr");
        return abilityRunFinished;
    }
    uint32_t allCount = abilityNode->GetAllComponentCount();
    uint32_t inputCount = abilityNode->GetInputCount();
    TRACK_LOG_STR("ability (%s) component count (%u), input count (%u)", abilityNode->GetBundleName().c_str(), allCount,
                  inputCount);
    if (inputCount >= allCount) {
        abilityRunFinished = true;
    }
    TRACK_LOG_END();
    return abilityRunFinished;
}

bool CheckBundleFinished(const std::shared_ptr<WuKongTree>& parent)
{
    if (!CheckAbliltyFinished(std::static_pointer_cast<AbilityTree>(parent))) {
        return false;
    }
    for (auto child : parent->GetChildren()) {
        if (!CheckBundleFinished(std::static_pointer_cast<AbilityTree>(child))) {
            return false;
        }
    }
    return true;
}

bool CheckInputFinished(const std::shared_ptr<ComponentParam>& param)
{
    TRACK_LOG_STD();
    bool isFinished = false;
    auto currentAbilityPtr = TreeManager::GetInstance()->GetCurrentAbility();
    if (currentAbilityPtr == nullptr) {
        ERROR_LOG("GetCurrentAbility abilityNode is nullptr");
        return isFinished;
    }

    // check app input event finished and set param is finished.
    if (CheckBundleFinished(WuKongTree::GetRoot(currentAbilityPtr))) {
        for (uint32_t i = 0; i < param->bundleRunning_.size(); i++) {
            if (param->bundleRunning_[i] && param->bundleName_[i] == currentAbilityPtr->GetBundleName()) {
                param->bundleFinish_[i] = true;
                isFinished = true;
                break;
            }
        }
    }

    TRACK_LOG_END();
    return isFinished;
}

ErrCode JudgeBackResult(const std::shared_ptr<ComponentParam>& param, uint32_t launchIndex)
{
    TRACK_LOG_STD();
    ErrCode result;
    param->pageBack_[launchIndex]++;
    TRACK_LOG_STR("back count: %d", param->pageBack_[launchIndex]);
    if (param->pageBack_[launchIndex] > PAGE_BACK_COUNT_MAX) {
        result = LauncherApp(param->bundleName_[launchIndex]);
        if (result != OHOS::ERR_OK) {
            return result;
        }
        param->pageBack_[launchIndex] = 0;
        param->lanuchCount_[launchIndex]++;
        TRACK_LOG_STR("lanuchCount_[%d] = %d", launchIndex, param->lanuchCount_[launchIndex]);
        if (param->lanuchCount_[launchIndex] > LANUCH_APP_COUNT_MAX) {
            param->bundleFinish_[launchIndex] = true;
            ERROR_LOG("Failed to launch the app five times in a row and exit");
            param->lanuchCount_[launchIndex] = 0;
            return OHOS::ERR_INVALID_VALUE;
        }
    } else {
        result = ComponentManager::GetInstance()->BackToPrePage();
    }
    TRACK_LOG_END();
    return result;
}
}  // namespace
ComponentInput::ComponentInput() : InputAction()
{
}

ComponentInput::~ComponentInput()
{
}

ErrCode ComponentInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    auto componentPtr = std::static_pointer_cast<ComponentParam>(specialTestObject);
    if (componentPtr == nullptr) {
        ERROR_LOG("specialTestObject param is null");
        return OHOS::ERR_INVALID_VALUE;
    }
    // launch app and check if app has been started
    uint32_t launchIndex = CheckLauncherApp(componentPtr);
    if (launchIndex >= componentPtr->bundleName_.size()) {
        ERROR_LOG("launcher app failed, and stop run test");
        componentPtr->isAllFinished_ = true;
        ERROR_LOG(componentPtr->toString().c_str());
        return OHOS::ERR_INVALID_VALUE;
    }
    auto treemanager = TreeManager::GetInstance();
    auto delegate = SceneDelegate::GetInstance();
    // update component information
    ErrCode result = treemanager->UpdateComponentInfo();
    DEBUG_LOG_STR("update componentinfo result (%d)", result);
    if (result == OHOS::ERR_OK) {
        // choose scene and set valid components
        result = delegate->ChooseScene(false);
        if (result != OHOS::ERR_OK) {
            ERROR_LOG("choose scene failed");
            return result;
        }
        // judge if is neccessnary to back to previous page
        if (delegate->IsBackToPrePage()) {
            result = JudgeBackResult(componentPtr, launchIndex);
            if (result != OHOS::ERR_OK) {
                return result;
            }
        } else {
            // get the component from tree manager to input action
            auto elementInfo = treemanager->GetElementInfoByOrder();
            if (elementInfo == nullptr) {
                ERROR_LOG("elementinfo is nullptr");
                return OHOS::ERR_INVALID_VALUE;
            }
            // get the actions of component
            int actionType = JudgeComponentType(*(elementInfo.get()));
            if (actionType == Accessibility::ACCESSIBILITY_ACTION_INVALID) {
                actionType = OHOS::Accessibility::ACCESSIBILITY_ACTION_CLICK;
            }
            // input action to component
            result = ComponentManager::GetInstance()->ComponentEventInput(*(elementInfo.get()), actionType);
            if (result == OHOS::ERR_OK) {
                // record index of inputted component
                treemanager->SetInputcomponentIndex(actionType);
                componentPtr->pageBack_[launchIndex] = 0;
                componentPtr->lanuchCount_[launchIndex] = 0;
                std::shared_ptr<ComponmentInputMsg> componentInputMsg = std::make_shared<ComponmentInputMsg>();
                componentInputMsg->pageComponments = delegate->GetComponentTypeList();
                componentInputMsg->pageId_ = delegate->GetCurrentPageId();
                componentInputMsg->componmentType_ = elementInfo->GetComponentType();
                Report::GetInstance()->SyncInputInfo(componentInputMsg);
            }
        }
    }
    // check current bundle finished state.
    if (CheckInputFinished(componentPtr)) {
        componentPtr->isAllFinished_ = true;
        // confirm all bundle status.
        std::vector<bool> bundleFinishList = componentPtr->bundleFinish_;
        bool x = false;
        if (std::any_of(bundleFinishList.begin(), bundleFinishList.end(), [x](int y) { return x == y; })) {
            componentPtr->isAllFinished_ = false;
        }
    }
    DEBUG_LOG_STR("component order input result (%d)", result);
    return result;
}

ErrCode ComponentInput::RandomInput()
{
    auto treemanager = TreeManager::GetInstance();
    ErrCode result = treemanager->UpdateComponentInfo();
    DEBUG_LOG_STR("update componentinfo result (%d)", result);
    if (result == OHOS::ERR_OK) {
        auto delegate = SceneDelegate::GetInstance();
        delegate->ChooseScene(true);
        auto componentInfos = treemanager->GetActiveElementInfos();
        DEBUG_LOG_STR("component list size (%d)", componentInfos.size());
        DEBUG_LOG_STR("back: %d", delegate->IsBackToPrePage());
        if (delegate->IsBackToPrePage()) {
            result = ComponentManager::GetInstance()->BackToPrePage();
        } else if (componentInfos.size() > 0) {
            uint32_t index = (uint32_t)(rand()) % componentInfos.size();
            DEBUG_LOG_STR("component input index (%d)", index);
            int actionType = JudgeComponentType(*(componentInfos[index].get()));
            if (actionType == Accessibility::ACCESSIBILITY_ACTION_INVALID) {
                actionType = OHOS::Accessibility::ACCESSIBILITY_ACTION_CLICK;
            }
            result = ComponentManager::GetInstance()->ComponentEventInput(*(componentInfos[index].get()), actionType);
            if (result == OHOS::ERR_OK) {
                treemanager->SetInputcomponentIndex(actionType, index);
                std::shared_ptr<ComponmentInputMsg> componentInputMsg = std::make_shared<ComponmentInputMsg>();
                componentInputMsg->pageComponments = delegate->GetComponentTypeList();
                componentInputMsg->pageId_ = delegate->GetCurrentPageId();
                componentInputMsg->componmentType_ = componentInfos[index]->GetComponentType();
                Report::GetInstance()->SyncInputInfo(componentInputMsg);
            }
        } else {
            ERROR_LOG("component list is null");
            result = OHOS::ERR_NO_INIT;
        }
    }
    DEBUG_LOG_STR("component random input result (%d)", result);
    return result;
}

ErrCode ComponentInput::GetInputInfo()
{
    return OHOS::ERR_OK;
}

int ComponentInput::JudgeComponentType(OHOS::Accessibility::AccessibilityElementInfo& elementInfo)
{
    int actionType;
    TRACK_LOG_STD();
    // get action list of component
    std::vector<OHOS::Accessibility::AccessibleAction> actionlist = elementInfo.GetActionList();
    if (actionlist.empty()) {
        std::string componentType = elementInfo.GetComponentType();
        TRACK_LOG_STR("component type: %s", componentType.c_str());
        if (componentType == "TextInput" || componentType == "TextArea" || componentType == "Text") {
            actionType = Accessibility::ACCESSIBILITY_ACTION_SET_TEXT;
        } else if (componentType == "GridContainer") {
            actionType = Accessibility::ACCESSIBILITY_ACTION_SCROLL_FORWARD;
        } else if (componentType == "Slider") {
            actionType = COMPONENT_LEFT_SWAP;
        } else {
            actionType = Accessibility::ACCESSIBILITY_ACTION_CLICK;
        }
    } else {
        TRACK_LOG_STR("action list size: %u", actionlist.size());
        auto it = actionlist[(uint32_t)(rand()) % actionlist.size()];
        actionType = (int)it.GetActionType();
    }
    TRACK_LOG_STR("action type: %d", actionType);
    return actionType;
}
}  // namespace WuKong
}  // namespace OHOS

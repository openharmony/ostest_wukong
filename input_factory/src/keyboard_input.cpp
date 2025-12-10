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
#include <vector>
#include <string>

#include "keyboard_input.h"
#include "multimode_manager.h"
#include "wukong_define.h"
#include "wukong_util.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
namespace {
const int SINGLE_CODE_PER = 30;
const int ONE_HUNDRED_PERCENT = 100;
const int DOWN_TIME = 100;

struct KeyCombination {
    std::vector<int> keys;
    std::string description;
};

const std::vector<KeyCombination> COMBINATION_KEYS = {
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_A}, "AppCenter"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_C}, "ControlCenter"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_D}, "Desktop"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_F}, "FileExplorer"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_L}, "LockScreen"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_M}, "WindowMinimized"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_N}, "NotificationCenter"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_S}, "HiSearch"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_TAB}, "TaskCenter"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_UP}, "WindowMaximized"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_DOWN}, "WindowMinimized"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_LEFT}, "LeftSplitScreen"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_RIGHT}, "RightSplitScreen"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DEL}, "Back"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SLASH}, "ShortCut"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_C}, "Copy"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_V}, "Paste"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_X}, "Cut"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_Z}, "Cancel"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_Y}, "Recover"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_D}, "Delete"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_S}, "Save"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_ZOOMIN}, "ZoomIn"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_ZOOMOUT}, "ZoomOut"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_A}, "SelectAll"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_F}, "SearchInApp"},
    {{OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_TAB}, "SwitchFocus"},
    {{OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DEL}, "DeletePermanently"},
    {{OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_UP}, "DragSelection"},
    {{OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_DOWN}, "DragSelection"},
    {{OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_LEFT}, "ContiguousSelection"},
    {{OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_DPAD_RIGHT}, "ContiguousSelection"},
    {{OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_F4}, "CloseWindow"},
    {{OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_TAB}, "SwitchApps"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_S},
        "Screenshot"},
    {{OHOS::MMI::KeyEvent::KEYCODE_META_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_R},
        "ScreenRecording"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT,
      OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT,
      OHOS::MMI::KeyEvent::KEYCODE_ESCAPE},
        "TaskManager"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_Z},
        "RestoreTheLastOperation"},
    {{OHOS::MMI::KeyEvent::KEYCODE_CTRL_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_S},
        "SaveAs"},
    {{OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_F4},
        "CloseApp"},
    {{OHOS::MMI::KeyEvent::KEYCODE_ALT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_SHIFT_LEFT, OHOS::MMI::KeyEvent::KEYCODE_TAB},
        "SwitchAppsReverse"}};
}  // namespace

KeyboardInput::KeyboardInput() : InputAction()
{
    std::shared_ptr<MultimodeInputMsg> multimodeInputMsg = std::make_shared<MultimodeInputMsg>();
    multimodeInputMsg->inputType_ = INPUTTYPE_KEYBOARDINPUT;
    inputedMsgObject_ = multimodeInputMsg;
}
KeyboardInput::~KeyboardInput()
{
}

ErrCode KeyboardInput::RandomInput()
{
    ErrCode result = OHOS::ERR_OK;
    std::vector<int> keycodelist;

    int eventIndex = WuKongUtil::GetInstance()->Generate(ONE_HALF);
    if (eventIndex == 0) {
        int keyCodePercent = rand() % ONE_HUNDRED_PERCENT;
        MultimodeManager::GetInstance()->GetKeycodeList(keycodelist);
        if (keycodelist.size() > 0) {
            if (keyCodePercent < SINGLE_CODE_PER) {
                int keycode = keycodelist[(uint32_t)(rand()) % keycodelist.size()];
                result = MultimodeManager::GetInstance()->SingleKeyCodeInput(keycode, DOWN_TIME);
            } else {
                result = MultimodeManager::GetInstance()->MultiKeyCodeInput(DOWN_TIME);
            }
        } else {
            return OHOS::ERR_NO_INIT;
        }
    } else {
        result = CombinationKeySimulate();
    }
    
    if (result != OHOS::ERR_OK) {
        return result;
    }

    Report::GetInstance()->SyncInputInfo(inputedMsgObject_);
    return result;
}

ErrCode KeyboardInput::CombinationKeySimulate()
{
    int32_t screenWidth = -1;
    int32_t screenHeight = -1;
    ErrCode result = WuKongUtil::GetInstance()->GetScreenSize(screenWidth, screenHeight);
    if (result != OHOS::ERR_OK) {
        return result;
    }

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> comb(0, COMBINATION_KEYS.size() - 1);
    int index = comb(rng);
    auto &combination = COMBINATION_KEYS[index];
    std::string desc = combination.description;
    INFO_LOG_STR("Start simulating key combinations: %s", desc.c_str());
    result = MultimodeManager::GetInstance()->KeyCombinationSimulate(combination.keys);
    // 如果模拟的是截图的话，还要模拟一下鼠标的手动
    if (desc == "Screenshot") {
        std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
        std::uniform_int_distribution<int> width_dist(0, screenWidth - 1);
        std::uniform_int_distribution<int> height_dist(0, screenHeight - 1);

        // 随机一个坐标
        int xClickPosition = width_dist(rng);
        int yClickPosition = height_dist(rng);
        int xDstPosition = xClickPosition + ONE_HALF * DEFAULT_SWIPE_MOVE_LENGTH;
        int yDstPosition = yClickPosition + ONE_HALF * DEFAULT_SWIPE_MOVE_LENGTH;

        std::string srcPosition = WuKongUtil::GetInstance()->GetPositionStr(xClickPosition, yClickPosition);
        std::string dstPosition = WuKongUtil::GetInstance()->GetPositionStr(xDstPosition, yDstPosition);

        result = MultimodeManager::GetInstance()->MouseActionSimulate(
            OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT, 'g', srcPosition, dstPosition, 0);
    }

    return result;
}

InputType KeyboardInput::GetInputInfo()
{
    return INPUTTYPE_KEYBOARDINPUT;
}
}  // namespace WuKong
}  // namespace OHOS

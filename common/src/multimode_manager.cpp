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
#include "multimode_manager.h"

namespace OHOS {
namespace WuKong {
namespace {
const int SHORTEST_LEN = 2;
const int LONGEST_LEN = 20;
const int DEFAULT_PRESSURE = 5;
const int SLEEP_TIME = 16000;
}  // namespace
MultimodeManager::MultimodeManager()
{
    for (int i = OHOS::MMI::KeyEvent::KEYCODE_CALL; i <= OHOS::MMI::KeyEvent::KEYCODE_ENDCALL; i++) {
        keycodelist_.push_back(i);
    }

    for (int j = OHOS::MMI::KeyEvent::KEYCODE_0; j <= OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN; j++) {
        keycodelist_.push_back(j);
    }
}

MultimodeManager::~MultimodeManager()
{
}

ErrCode MultimodeManager::SingleKeyCodeInput(int keycode, int downtime)
{
    ErrCode result = ERR_OK;
    std::string keycodeType = OHOS::MMI::KeyEvent::KeyCodeToString(keycode);
    INFO_LOG_STR("keycodeType: %s", keycodeType.c_str());
    auto keyKeyboardEvent = OHOS::MMI::KeyEvent::Create();
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(keycode);
    item.SetPressed(true);
    item.SetDownTime(downtime);
    keyKeyboardEvent->SetKeyCode(keycode);
    keyKeyboardEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    keyKeyboardEvent->AddPressedKeyItems(item);
    // check if KeyEvent is valid
    if (keyKeyboardEvent->IsValid()) {
        MMI::InputManager::GetInstance()->SimulateInputEvent(keyKeyboardEvent);
    } else {
        WARN_LOG("keyevent down is invalid");
        return OHOS::ERR_NO_INIT;
    }
    keyKeyboardEvent->RemoveReleasedKeyItems(item);
    item.SetKeyCode(keycode);
    item.SetPressed(false);
    item.SetDownTime(downtime);
    keyKeyboardEvent->SetKeyCode(keycode);
    keyKeyboardEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    keyKeyboardEvent->AddPressedKeyItems(item);
    // check if KeyEvent is valid
    if (keyKeyboardEvent->IsValid()) {
        MMI::InputManager::GetInstance()->SimulateInputEvent(keyKeyboardEvent);
    } else {
        WARN_LOG("keyevent up is invalid");
        return OHOS::ERR_NO_INIT;
    }
    keyKeyboardEvent->RemoveReleasedKeyItems(item);

    return result;
}

ErrCode MultimodeManager::MultiKeyCodeInput(int downtime)
{
    ErrCode result = OHOS::ERR_OK;
    // generate the length of string randomly
    int stringLen = SHORTEST_LEN + random() % (LONGEST_LEN - 1);
    if (keycodelist_.size() > 0) {
        for (int i = 0; i < stringLen; i++) {
            int keycode = keycodelist_[(uint32_t)(rand()) % keycodelist_.size()];
            result = MultimodeManager::GetInstance()->SingleKeyCodeInput(keycode, downtime);
        }
    } else {
        return OHOS::ERR_NO_INIT;
    }
    return result;
}

void MultimodeManager::GetKeycodeList(std::vector<int> &keycodelist)
{
    keycodelist = keycodelist_;
}

ErrCode MultimodeManager::PointerInput(int x, int y, int pointertype, int actiontype)
{
    ErrCode result = OHOS::ERR_OK;
    auto pointerEvent = MMI::PointerEvent::Create();
    MMI::PointerEvent::PointerItem item;

    item.SetPointerId(0);
    item.SetDisplayX(x);
    item.SetDisplayY(y);
    item.SetPressure(DEFAULT_PRESSURE);
    pointerEvent->AddPointerItem(item);

    pointerEvent->SetPointerAction(actiontype);
    pointerEvent->SetSourceType(pointertype);
    pointerEvent->SetPointerId(0);

    MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

    return result;
}

ErrCode MultimodeManager::IntervalSwap(int xSrcPosition, int ySrcPosition, int xDstPosition, int yDstPosition)
{
    auto multiinput = MultimodeManager::GetInstance();
    ErrCode result = multiinput->PointerInput(xSrcPosition, ySrcPosition, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                              MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    usleep(SLEEP_TIME);
    int segment = 50;
    float secX = (xDstPosition - xSrcPosition) / (float)segment;
    float secY = (yDstPosition - ySrcPosition) / (float)segment;

    for (int i = 1; i < segment; ++i) {
        int mPosX = int(xSrcPosition + secX * i);
        int mPosY = int(ySrcPosition + secY * i);
        result = multiinput->PointerInput(mPosX, mPosY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                          MMI::PointerEvent::POINTER_ACTION_MOVE);
        if (result != OHOS::ERR_OK) {
            return result;
        }
        usleep(SLEEP_TIME);
    }
    result = multiinput->PointerInput(xDstPosition, yDstPosition, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_UP);
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

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

#include "wukong_event_manager.h"
#include "multimodal_event_handler.h"
#include <unistd.h>

namespace OHOS {
    namespace AppExecFwk {
        ErrCode WuKongEventManager::TouchEvent(int xPosition, int yPosition, int touchTime, int pressure)
        {
            auto pointerEvent = MMI::PointerEvent::Create();
            MMI::PointerEvent::PointerItem item;

            item.SetPointerId(0);
            item.SetGlobalX(xPosition);
            item.SetGlobalY(yPosition);
            item.SetPressure(pressure);
            pointerEvent->AddPointerItem(item);

            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
            pointerEvent->SetPointerId(0);

            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
            pointerEvent->SetPointerId(0);

            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

            return OHOS::ERR_OK;
        }

        ErrCode WuKongEventManager::MotionEvent(int xSrcPosition, int ySrcPosition,
                                                int xDstPosition, int yDstPosition,
                                                int pressure)
                                                {
            int sleepTime = 16000;

            auto downEvent = MMI::PointerEvent::Create();
            MMI::PointerEvent::PointerItem downItem;

            downItem.SetPointerId(0);
            downItem.SetGlobalX(xSrcPosition);
            downItem.SetGlobalY(ySrcPosition);
            downItem.SetPressure(pressure);
            downEvent->SetPointerId(0);
            downEvent->AddPointerItem(downItem);
            downEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
            downEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);

            MMI::InputManager::GetInstance()->SimulateInputEvent(downEvent);
            usleep(sleepTime);

            auto moveEvent = MMI::PointerEvent::Create();
            MMI::PointerEvent::PointerItem moveItem;

            int segment = 50;
            float secX = (xDstPosition - xSrcPosition) / (float) segment;
            float secY = (yDstPosition - ySrcPosition) / (float) segment;

            for (int i = 1; i < segment; ++i) {
                moveItem.SetPointerId(0);
                moveItem.SetGlobalX(int(xSrcPosition + secX * i));
                moveItem.SetGlobalY(int(ySrcPosition + secY * i));
                moveItem.SetPressure(pressure);
                moveEvent->SetPointerId(0);
                moveEvent->AddPointerItem(moveItem);
                moveEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
                moveEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);

                MMI::InputManager::GetInstance()->SimulateInputEvent(moveEvent);
                moveEvent->RemovePointerItem(0);
                usleep(sleepTime);
            }

            moveItem.SetPointerId(0);
            moveItem.SetGlobalX(xDstPosition);
            moveItem.SetGlobalY(yDstPosition);
            moveItem.SetPressure(pressure);
            moveEvent->SetPointerId(0);
            moveEvent->AddPointerItem(moveItem);
            moveEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
            moveEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);

            MMI::InputManager::GetInstance()->SimulateInputEvent(moveEvent);
            usleep(sleepTime);

            return OHOS::ERR_OK;
        }

        ErrCode WuKongEventManager::PowerOnAndOffEvent()
        {
            auto keyPowerEvent = OHOS::MMI::KeyEvent::Create();

            int32_t downTime = 0;
            MMI::KeyEvent::KeyItem item;
            item.SetKeyCode(MMI::KeyEvent::KEYCODE_POWER);
            item.SetPressed(true);
            item.SetDownTime(downTime);
            keyPowerEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_POWER);
            keyPowerEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
            keyPowerEvent->AddPressedKeyItems(item);

            MMI::InputManager::GetInstance()->SimulateInputEvent(keyPowerEvent);

            keyPowerEvent->RemoveReleasedKeyItems(item);
            item.SetKeyCode(OHOS::MMI::KeyEvent::KEYCODE_POWER);
            item.SetPressed(false);
            item.SetDownTime(downTime);
            keyPowerEvent->SetKeyCode(MMI::KeyEvent::KEYCODE_POWER);
            keyPowerEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
            keyPowerEvent->RemoveReleasedKeyItems(item);
            MMI::InputManager::GetInstance()->SimulateInputEvent(keyPowerEvent);
            keyPowerEvent->RemoveReleasedKeyItems(item);

            return OHOS::ERR_OK;
        }
    }
}

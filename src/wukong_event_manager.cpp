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


#include <unistd.h>
#include <iostream>
#include "input_manager.h"
#include "wukong_event_manager.h"

namespace OHOS {
    namespace AppExecFwk {
        using namespace OHOS::Accessibility;
        using namespace OHOS;
        using namespace std;
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

        ErrCode WuKongEventManager::ElementEvent()
        {
            if (ConnectToSysAbility()) {
                auto ability = AccessibilityUITestAbility::GetInstance();
                std::optional<AccessibilityElementInfo > elementInfo;
                ability->GetRootElementInfo(elementInfo);
                if (elementInfo.has_value()) {
                    GetChildElement(elementInfo.value());
                }
                return OHOS::ERR_OK;
            } else {
                std::cout << "Connect to AAMS failed" << std::endl;
                return OHOS::ERR_NO_INIT;
            }
        }

        ErrCode WuKongEventManager::MouseEvent(int xPosition, int yPosition, int type, int touchTime, int pressure)
        {
            auto pointerEvent = MMI::PointerEvent::Create();
            MMI::PointerEvent::PointerItem item;

            item.SetPointerId(0);
            item.SetGlobalX(xPosition);
            item.SetGlobalY(yPosition);
            item.SetPressure(pressure);
            pointerEvent->AddPointerItem(item);

            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
            pointerEvent->SetSourceType(type);
            pointerEvent->SetPointerId(0);

            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
            pointerEvent->SetSourceType(type);
            pointerEvent->SetPointerId(0);

            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

            return OHOS::ERR_OK;
        }

        ErrCode WuKongEventManager::KeyBoardEvent(int keycode, int touchTime, int pressure)
        {
            auto keyKeyBoardEvent = OHOS::MMI::KeyEvent::Create();

            int32_t downTime = 100;
            MMI::KeyEvent::KeyItem item;
            item.SetKeyCode(keycode);
            item.SetPressed(true);
            item.SetDownTime(downTime);
            keyKeyBoardEvent->SetKeyCode(keycode);
            keyKeyBoardEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
            keyKeyBoardEvent->AddPressedKeyItems(item);
            if (keyKeyBoardEvent->IsValid() == true) {
                MMI::InputManager::GetInstance()->SimulateInputEvent(keyKeyBoardEvent);
            } else {
                std::cout << "keyevent down is invalid" << std::endl;
                return OHOS::ERR_NO_INIT;
            }
            keyKeyBoardEvent->RemoveReleasedKeyItems(item);
            item.SetKeyCode(keycode);
            item.SetPressed(false);
            item.SetDownTime(downTime);
            keyKeyBoardEvent->SetKeyCode(keycode);
            keyKeyBoardEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
            keyKeyBoardEvent->AddPressedKeyItems(item);
            if (keyKeyBoardEvent->IsValid() == true) {
                MMI::InputManager::GetInstance()->SimulateInputEvent(keyKeyBoardEvent);
            } else {
                std::cout << "keyevent up is invalid" << std::endl;
                return OHOS::ERR_NO_INIT;
            }
            keyKeyBoardEvent->RemoveReleasedKeyItems(item);

            return OHOS::ERR_OK;
        }

        void UiEventMonitor::SetOnAbilityConnectCallback(std::function<void()> onConnectCb)
        {
            onConnectCallback_ = std::move(onConnectCb);
        }

        void UiEventMonitor::SetOnAbilityDisConnectCallback(std::function<void()> onDisConnectCb)
        {
            onDisConnectCallback_ = std::move(onDisConnectCb);
        }

        void UiEventMonitor::OnAbilityConnected()
        {
            if (onConnectCallback_ != nullptr) {
                onConnectCallback_();
            }
        }

        void UiEventMonitor::OnAbilityDisconnected()
        {
            if (onDisConnectCallback_ != nullptr) {
                onDisConnectCallback_();
            }
        }

        void UiEventMonitor::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
        {
            std::cout << "OnAccessibilityEvent Start" << eventInfo.GetEventType() << std::endl;
        }

        uint64_t UiEventMonitor::GetLastEventMillis()
        {
            if (lastEventMillis_.load() <= 0) {
                lastEventMillis_.store(1);
            }
            return lastEventMillis_.load();
        }

        bool UiEventMonitor::WaitEventIdle(uint32_t idleThresholdMs, uint32_t timeoutMs)
        {
            const auto currentMs = 1;
            if (lastEventMillis_.load() <= 0) {
                lastEventMillis_.store(currentMs);
            }
            if (currentMs - lastEventMillis_.load() >= idleThresholdMs) {
                return true;
            }
            static constexpr auto sliceMs = 10;
            return WaitEventIdle(idleThresholdMs, timeoutMs - sliceMs);
        }

        bool WuKongEventManager::ConnectToSysAbility()
        {
            if (connected == true) {
                return true;
            }
            std::mutex mtx;
            unique_lock<mutex> uLock(mtx);
            std::shared_ptr<UiEventMonitor> g_monitorInstance_ = std::make_shared<UiEventMonitor>();
            std::condition_variable condition;
            auto onConnectCallback = [&condition]() {
                std::cout <<"Success connect to AAMS"<< std::endl;
                condition.notify_all();
            };

            if (g_monitorInstance_ == nullptr) {
                g_monitorInstance_ = std::make_shared<UiEventMonitor>();
            }
            g_monitorInstance_->SetOnAbilityConnectCallback(onConnectCallback);
            auto ability = AccessibilityUITestAbility::GetInstance();
            if (!ability->RegisterListener(g_monitorInstance_)) {
                std::cout <<"Failed to register UiEventMonitor"<< std::endl;
                return false;
            }
            std::cout <<   "Start connect to AAMS" << std::endl;
            if (!ability->Connect()) {
                std::cout <<  "Failed to connect to AAMS" << std::endl;
                return false;
            }
            const auto timeout = chrono::milliseconds(500);
            if (condition.wait_for(uLock, timeout) == cv_status::timeout) {
                std::cout <<"Wait connection to AAMS timed out"<< std::endl;
                return  false;
            }
            connected = true;
            return true;
        }
        void WuKongEventManager::GetChildElement(AccessibilityElementInfo &elementInfo)
        {
            int childcounts = elementInfo.GetChildCount();
            map<string, string> action = {{ACTION_ARGU_HTML_ELEMENT,HTML_ITEM_BUTTON}};

            for (int index= 0; index < childcounts ; index++) {
                AccessibilityElementInfo childElement;
                elementInfo.GetChild(index, childElement);
                if (childElement.GetChildCount() > 0) {
                    GetChildElement(childElement);
                } else {
                    const auto rect = childElement.GetRectInScreen();
                    const auto rootBounds = Rect{rect.GetLeftTopXScreenPostion(), 
                                                 rect.GetRightBottomXScreenPostion(),
                                                 rect.GetLeftTopYScreenPostion(), 
                                                 rect.GetRightBottomYScreenPostion()};
                    if (childElement.IsClickable() == true) {
                        childElement.ExecuteAction(ACCESSIBILITY_ACTION_CLICK, action);
                    }
                }
            }
        }
        void WuKongEventManager::KeyCodeInit()
        {
            for(int i = OHOS::MMI::KeyEvent::KEYCODE_FN; 
                i <= OHOS::MMI::KeyEvent::KEYCODE_ENDCALL; 
                i++) {
                keycodelist_.push_back(i);
            }

            for(int j = OHOS::MMI::KeyEvent::KEYCODE_0; 
                j <= OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN; 
                j++) {
                keycodelist_.push_back(j);
            }
        }

        void WuKongEventManager::GetKeycodeList(std::vector<int> & keycodelist)
        {
            keycodelist = keycodelist_;
        }
    }
}

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

#ifndef WUKONG_WUKONG_EVENT_MANAGER_H
#define WUKONG_WUKONG_EVENT_MANAGER_H


#include "input_manager.h"
#include "singleton.h"
#include "accessibility_ui_test_ability.h"
#include <chrono>

namespace OHOS {
    namespace AppExecFwk {
        class WuKongEventManager : public DelayedSingleton<WuKongEventManager> {
        public:
            /**
             * @brief inject the touch event.
             * @param xPosition the coordinate of x of the touch event.
             * @param yPosition the coordinate of y of the touch event.
             * @param touchTime the touch time of the touch event.
             * @param pressure the pressure of the touch event.
             * @return NO_ERROR: successful; otherwise is failed.
             */ 
            ErrCode TouchEvent(int xPosition, int yPosition, int touchTime = 300, int pressure = 5);

            /**
             * @brief inject the motion event.
             * @param xSrcPosition the start coordinate of x of the motion event.
             * @param ySrcPosition the start coordinate of y of the motion event.
             * @param xDstPosition the end coordinate of x of the motion event.
             * @param yDstPosition the end coordinate of y of the motion event.
             * @param pressure the pressure of the motion event.
             * @return NO_ERROR: successful; otherwise is failed.
             */ 
            ErrCode MotionEvent(int xSrcPosition, int ySrcPosition,
                                int xDstPosition, int yDstPosition,
                                int pressure = 5);

            /**
             * @brief inject poweron/off event.
             * @return NO_ERROR: successful; otherwise is failed.
             */ 
            ErrCode PowerOnAndOffEvent();

            /**
             * @brief  inject shellcommandInputEvent.
             * @param command
             * @return NO_ERROR: successful; otherwise is failed.
             */ 
            ErrCode ShellCommandInputEvent(std::string command);

            /**
             * @brief inject the mouse event.
             * @param xPosition the coordinate of x of the mouse event.
             * @param yPosition the coordinate of y of the mouse event.
             * @param touchTime the touch time of the mouse event.
             * @param pressure the pressure of the mouse event.
             * @return NO_ERROR: successful; otherwise is failed.
             */ 
            ErrCode MouseEvent(int xPosition, int yPosition, int type, int touchTime = 300, int pressure = 5);

            /**
             * @brief inject the keyboad event.
             * @param keycode keycode of the keyboad event.
             * @param touchTime the touch time of the keyboad event.
             * @param pressure the pressure of the keyboad event.
             * @return NO_ERROR: successful; otherwise is failed.
             */ 
            ErrCode KeyBoardEvent(int keycode, int touchTime = 300, int pressure = 5);

            /**
             * @brief inject the element event.
             * @return NO_ERROR: successful; otherwise is failed.
             */
            ErrCode ElementEvent();

            /**
             * @brief connect to AAMS.
             * @return true: connect successful; false: connect fail.
             */
            bool ConnectToSysAbility();

            // get childelement and inject event .
            void GetChildElement(OHOS::Accessibility::AccessibilityElementInfo &elementInfo);

            // keycode list init
            void KeyCodeInit();

            // get keycode list
            void GetKeycodeList(std::vector<int> & keycodelist);
        private:
            // judge connected
            bool connected = false;

            // keycodelist
            std::vector<int> keycodelist_;
        };

        class UiEventMonitor final : public Accessibility::AccessibleAbilityListener {
        public:
            virtual ~UiEventMonitor() = default;

            void OnAbilityConnected() override;

            void OnAbilityDisconnected() override;

            void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo &eventInfo) override;

            void SetOnAbilityConnectCallback(std::function<void()> onConnectCb);

            void SetOnAbilityDisConnectCallback(std::function<void()> onDisConnectCb);

            bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent> &keyEvent) override
            {
                return false;
            }

            uint64_t GetLastEventMillis();

            bool WaitEventIdle(uint32_t idleThresholdMs, uint32_t timeoutMs);

        private:
            std::function<void()> onConnectCallback_ = nullptr;
            std::function<void()> onDisConnectCallback_ = nullptr;
            std::atomic<uint64_t> lastEventMillis_ = 0;
        };
    }
}
#endif // WUKONG_WUKONG_EVENT_MANAGER_H

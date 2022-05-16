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
#ifndef TEST_WUKONG_EVENT_MONITOR_H
#define TEST_WUKONG_EVENT_MONITOR_H

#include "accessibility_ui_test_ability.h"

namespace OHOS {
namespace WuKong {
class EventMonitor final : public Accessibility::AccessibleAbilityListener {
public:
    virtual ~EventMonitor() = default;

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
}  // namespace WuKong
}  // namespace OHOS
#endif

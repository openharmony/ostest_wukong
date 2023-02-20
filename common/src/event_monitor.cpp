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

#include "event_monitor.h"

void EventMonitor::SetOnAbilityConnectCallback(std::function<void()> onConnectCb)
{
    onConnectCallback_ = std::move(onConnectCb);
}

void EventMonitor::SetOnAbilityDisConnectCallback(std::function<void()> onDisConnectCb)
{
    onDisConnectCallback_ = std::move(onDisConnectCb);
}

void EventMonitor::OnAbilityConnected()
{
    if (onConnectCallback_ != nullptr) {
        onConnectCallback_();
    }
}

void EventMonitor::OnAbilityDisconnected()
{
    if (onDisConnectCallback_ != nullptr) {
        onDisConnectCallback_();
    }
}

void EventMonitor::OnAccessibilityEvent(const AccessibilityEventInfo &eventInfo)
{
    DEBUG_LOG_STR("OnAccessibilityEvent Start %s", std::to_string(eventInfo.GetEventType()).c_str());
}

uint64_t EventMonitor::GetLastEventMillis()
{
    if (lastEventMillis_.load() <= 0) {
        lastEventMillis_.store(1);
    }
    return lastEventMillis_.load();
}

bool EventMonitor::WaitEventIdle(uint32_t idleThresholdMs, uint32_t timeoutMs)
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
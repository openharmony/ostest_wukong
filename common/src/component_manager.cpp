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

#include "component_manager.h"

#include "multimode_manager.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::vector<std::string> typeList = {"Toggle", "Column", "Row", "Text", "ListItem", "Button"};
const std::string permissionBundleName = "com.ohos.permissionmanager";
const int DIV = 2;
const int DOWNTIME = 10;
}  // namespace

class ComponentEventMonitor : public Accessibility::AccessibleAbilityListener {
public:
    virtual ~ComponentEventMonitor() = default;

    void OnAbilityConnected() override;

    void OnAbilityDisconnected() override;

    void OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo& eventInfo) override;

    void SetOnAbilityConnectCallback(std::function<void()> onConnectCb);

    void SetOnAbilityDisConnectCallback(std::function<void()> onDisConnectCb);

    bool OnKeyPressEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) override
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

void ComponentEventMonitor::SetOnAbilityConnectCallback(std::function<void()> onConnectCb)
{
    onConnectCallback_ = std::move(onConnectCb);
}

void ComponentEventMonitor::SetOnAbilityDisConnectCallback(std::function<void()> onDisConnectCb)
{
    onDisConnectCallback_ = std::move(onDisConnectCb);
}

void ComponentEventMonitor::OnAbilityConnected()
{
    if (onConnectCallback_ != nullptr) {
        onConnectCallback_();
    }
}

void ComponentEventMonitor::OnAbilityDisconnected()
{
    if (onDisConnectCallback_ != nullptr) {
        onDisConnectCallback_();
    }
}

void ComponentEventMonitor::OnAccessibilityEvent(const Accessibility::AccessibilityEventInfo& eventInfo)
{
    DEBUG_LOG_STR("OnAccessibilityEvent Start %s", std::to_string(eventInfo.GetEventType()).c_str());
    DEBUG_LOG_STR("current bundlie: %s", eventInfo.GetBundleName().c_str());
    if (eventInfo.GetBundleName() == permissionBundleName) {
        auto listenerlist = ComponentManager::GetInstance()->GetListenerList();
        for (auto it : listenerlist) {
            it->OnPermissionScreenShown();
        }
    }
}

uint64_t ComponentEventMonitor::GetLastEventMillis()
{
    return 0;
}

bool ComponentEventMonitor::WaitEventIdle(uint32_t idleThresholdMs, uint32_t timeoutMs)
{
    return true;
}

ComponentManager::ComponentManager()
{
}
ComponentManager::~ComponentManager()
{
}

bool ComponentManager::Connect()
{
    if (connected_ == true) {
        return true;
    }
    std::mutex mtx;
    std::unique_lock<std::mutex> uLock(mtx);
    std::shared_ptr<ComponentEventMonitor> g_monitorInstance_ = std::make_shared<ComponentEventMonitor>();
    std::condition_variable condition;
    auto onConnectCallback = [&condition]() {
        std::cout << "Success connect to AAMS" << std::endl;
        condition.notify_all();
    };

    if (g_monitorInstance_ == nullptr) {
        g_monitorInstance_ = std::make_shared<ComponentEventMonitor>();
    }
    g_monitorInstance_->SetOnAbilityConnectCallback(onConnectCallback);
    auto ability = Accessibility::AccessibilityUITestAbility::GetInstance();
    if (!ability->RegisterAbilityListener(g_monitorInstance_)) {
        std::cout << "Failed to register ComponentEventMonitor" << std::endl;
        return false;
    }
    std::cout << "Start connect to AAMS" << std::endl;
    if (ability->Connect() != Accessibility::RET_OK) {
        std::cout << "Failed to connect to AAMS" << std::endl;
        return false;
    }
    const auto timeout = std::chrono::milliseconds(500);
    if (condition.wait_for(uLock, timeout) == std::cv_status::timeout) {
        std::cout << "Wait connection to AAMS timed out" << std::endl;
        return false;
    }
    connected_ = true;
    return true;
}

void ComponentManager::Disconnect()
{
}
uint32_t ComponentManager::AddRegisterListener(std::shared_ptr<ComponentManagerListener> listener)
{
    TRACK_LOG_STD();
    listenerList_.push_back(listener);
    TRACK_LOG_STR("Add linstener count (%d)", listenerList_.size());
    return listenerList_.size() - 1;
}

void ComponentManager::DeleteRegisterListener(const uint32_t handle)
{
    if (listenerList_.size() > handle) {
        listenerList_.erase(listenerList_.begin() + handle);
    }
}

std::vector<std::shared_ptr<ComponentManagerListener>> ComponentManager::GetListenerList()
{
    return listenerList_;
}

ErrCode ComponentManager::GetReportInfo(std::string& info)
{
    ErrCode result = OHOS::ERR_OK;
    return result;
}

ErrCode ComponentManager::PermoissionInput()
{
    DEBUG_LOG("handle permission window");
    return OHOS::ERR_OK;
}

ErrCode ComponentManager::CreateEventInputMap()
{
    componentMap_ = {
        {Accessibility::ACCESSIBILITY_ACTION_CLICK,
         std::bind(&ComponentManager::ComponentTouchInput, this, std::placeholders::_1)},
        {Accessibility::ACCESSIBILITY_ACTION_SCROLL_FORWARD,
         std::bind(&ComponentManager::ComponentUpSwapInput, this, std::placeholders::_1)},

        {Accessibility::ACCESSIBILITY_ACTION_SCROLL_BACKWARD,
         std::bind(&ComponentManager::ComponentDownSwapInput, this, std::placeholders::_1)},
        {Accessibility::ACCESSIBILITY_ACTION_SET_TEXT,
         std::bind(&ComponentManager::ComponentMultikeyInput, this, std::placeholders::_1)},
    };
    return OHOS::ERR_OK;
}

ErrCode ComponentManager::ComponentEventInput(OHOS::Accessibility::AccessibilityElementInfo& elementInfo,
                                              const OHOS::Accessibility::ActionType actionType)
{
    int result = OHOS::ERR_OK;
    CreateEventInputMap();
    auto componentRespond = componentMap_[actionType];
    if (componentRespond == nullptr) {
        componentRespond = componentMap_[Accessibility::ACCESSIBILITY_ACTION_CLICK];
    }
    result = componentRespond(elementInfo);
    return result;
}

ErrCode ComponentManager::ComponentTouchInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result = OHOS::ERR_OK;
    Accessibility::Rect componentBounds;
    auto touchInput = MultimodeManager::GetInstance();
    componentBounds = elementInfo.GetRectInScreen();
    int boundStartX = componentBounds.GetLeftTopXScreenPostion();
    int boundStartY = componentBounds.GetLeftTopYScreenPostion();
    int boundEndX = componentBounds.GetRightBottomXScreenPostion();
    int boundEndY = componentBounds.GetRightBottomYScreenPostion();

    // Calculate touch position
    int elementTouchX = boundStartX + (boundEndX - boundStartX) / DIV;
    int elementTouchY = boundStartY + (boundEndY - boundStartY) / DIV;
    DEBUG_LOG_STR("componentTouch Position: (%d, %d)", elementTouchX, elementTouchY);
    result = touchInput->PointerInput(elementTouchX, elementTouchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_DOWN);
    result = touchInput->PointerInput(elementTouchX, elementTouchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_UP);
    return result;
}

ErrCode ComponentManager::BackToPrePage()
{
    ErrCode result = ERR_OK;
    int backKeyCode = OHOS::MMI::KeyEvent::KEYCODE_BACK;
    result = MultimodeManager::GetInstance()->SingleKeyCodeInput(backKeyCode, DOWNTIME);
    return result;
}

ErrCode ComponentManager::ComponentUpSwapInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result = OHOS::ERR_OK;
    Accessibility::Rect componentBounds = elementInfo.GetRectInScreen();
    int boundStartX = componentBounds.GetLeftTopXScreenPostion();
    int boundStartY = componentBounds.GetLeftTopYScreenPostion();
    int boundEndX = componentBounds.GetRightBottomXScreenPostion();
    int boundEndY = componentBounds.GetRightBottomYScreenPostion();
    int componentUpSwapStartX = boundStartX + (boundEndX - boundStartX) / DIV;
    int componentUpSwapStartY = boundStartY;
    int componentUpSwapEndX = boundStartX + (boundEndX - boundStartX) / DIV;
    int componentUpSwapEndY = boundEndY;
    result = MultimodeManager::GetInstance()->IntervalSwap(componentUpSwapStartX, componentUpSwapStartY,
                                                           componentUpSwapEndX, componentUpSwapEndY);
    return result;
}

ErrCode ComponentManager::ComponentDownSwapInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result = OHOS::ERR_OK;
    Accessibility::Rect componentBounds;
    int boundStartX = componentBounds.GetLeftTopXScreenPostion();
    int boundStartY = componentBounds.GetLeftTopYScreenPostion();
    int boundEndX = componentBounds.GetRightBottomXScreenPostion();
    int boundEndY = componentBounds.GetRightBottomYScreenPostion();
    int componentDownSwapStartX = boundStartX + (boundEndX - boundStartX) / DIV;
    int componentDownSwapStartY = boundEndY;
    int componentDownSwapEndX = boundStartX + (boundEndX - boundStartX) / DIV;
    int componentDownSwapEndY = boundStartY;
    result = MultimodeManager::GetInstance()->IntervalSwap(componentDownSwapStartX, componentDownSwapStartY,
                                                           componentDownSwapEndX, componentDownSwapEndY);
    return result;
}

ErrCode ComponentManager::ComponentMultikeyInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result = OHOS::ERR_OK;
    result = MultimodeManager::GetInstance()->MultiKeyCodeInput(DOWNTIME);
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

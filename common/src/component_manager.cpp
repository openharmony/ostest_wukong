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

#include "accessibility_ui_test_ability.h"
#include "multimode_manager.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string permissionBundleName = "com.ohos.permissionmanager";
const int DIV = 2;
const int DOWNTIME = 10;
const int ONESECOND = 1000000;
const int TWOSECONDS = 2000000;
const int OFFSET = 10;
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
    DEBUG_LOG_STR("OnAccessibilityEvent Start %u", eventInfo.GetEventType());
    DEBUG_LOG_STR("current bundle: %s", eventInfo.GetBundleName().c_str());
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
    componentMap_ = {
        {Accessibility::ACCESSIBILITY_ACTION_CLICK,
         std::bind(&ComponentManager::ComponentTouchInput, this, std::placeholders::_1)},
        {Accessibility::ACCESSIBILITY_ACTION_SCROLL_FORWARD,
         std::bind(&ComponentManager::ComponentUpSwapInput, this, std::placeholders::_1)},

        {Accessibility::ACCESSIBILITY_ACTION_SCROLL_BACKWARD,
         std::bind(&ComponentManager::ComponentDownSwapInput, this, std::placeholders::_1)},
        {Accessibility::ACCESSIBILITY_ACTION_SET_TEXT,
         std::bind(&ComponentManager::ComponentMultikeyInput, this, std::placeholders::_1)},
        {COMPONENT_LEFT_SWAP, std::bind(&ComponentManager::ComponentLeftSwapInput, this, std::placeholders::_1)},
    };
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

    g_monitorInstance_->SetOnAbilityConnectCallback(onConnectCallback);
    auto ability = Accessibility::AccessibilityUITestAbility::GetInstance();
    if (ability->RegisterAbilityListener(g_monitorInstance_) != Accessibility::RET_OK) {
        std::cout << "Failed to register ComponentEventMonitor" << std::endl;
        return false;
    }
    std::cout << "Start connect to AAMS" << std::endl;
    if (ability->Connect() != Accessibility::RET_OK) {
        std::cout << "Failed to connect to AAMS" << std::endl;
        return false;
    }
    const auto timeout = std::chrono::milliseconds(1000);
    if (condition.wait_for(uLock, timeout) == std::cv_status::timeout) {
        std::cout << "Wait connection to AAMS timed out" << std::endl;
        return false;
    }
    connected_ = true;
    return true;
}

void ComponentManager::Disconnect()
{
    auto auita = Accessibility::AccessibilityUITestAbility::GetInstance();
    if (auita != nullptr) {
        auita->Disconnect();
    }
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
    return OHOS::ERR_OK;
}

ErrCode ComponentManager::ComponentEventInput(OHOS::Accessibility::AccessibilityElementInfo& elementInfo,
                                              const int actionType)
{
    CreateEventInputMap();
    // get position of current component
    GetComponentPosition(elementInfo);
    auto componentRespond = componentMap_[actionType];
    if (componentRespond == nullptr) {
        componentRespond = componentMap_[Accessibility::ACCESSIBILITY_ACTION_CLICK];
    }
    ErrCode result = componentRespond(elementInfo);
    return result;
}

ErrCode ComponentManager::ComponentTouchInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result;
    auto touchInput = MultimodeManager::GetInstance();
    // Calculate touch position
    int32_t elementTouchX = startX_ + (endX_ - startX_) / DIV;
    int32_t elementTouchY = startY_ + (endY_ - startY_) / DIV;
    std::string type = elementInfo.GetContent();
    INFO_LOG_STR("component Content: Touch Position: (%d, %d)", elementTouchX, elementTouchY);
    DEBUG_LOG_STR("component Content: (%s), Touch Position: (%d, %d)", type.c_str(), elementTouchX, elementTouchY);
    result = touchInput->PointerInput(elementTouchX, elementTouchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                      MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (result == OHOS::ERR_OK) {
        result = touchInput->PointerInput(elementTouchX, elementTouchY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                          MMI::PointerEvent::POINTER_ACTION_UP);
    }
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
    // Calculate swap position
    int32_t componentUpSwapStartX = startX_ + (endX_ - startX_) / DIV;
    int32_t componentUpSwapStartY = endY_ - OFFSET;
    int32_t componentUpSwapEndX = componentUpSwapStartX;
    int32_t componentUpSwapEndY = startY_ + OFFSET;
    INFO_LOG_STR("Component Up Swap: (%d, %d) -> (%d, %d)", componentUpSwapStartX, componentUpSwapStartY,
                 componentUpSwapEndX, componentUpSwapEndY);
    ErrCode result = MultimodeManager::GetInstance()->IntervalSwap(componentUpSwapStartX, componentUpSwapStartY,
                                                                   componentUpSwapEndX, componentUpSwapEndY);
    usleep(TWOSECONDS);
    return result;
}

ErrCode ComponentManager::ComponentDownSwapInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    // Calculate swap position
    int32_t componentDownSwapStartX = startX_ + (endX_ - startX_) / DIV;
    int32_t componentDownSwapStartY = startY_ + OFFSET;
    int32_t componentDownSwapEndX = componentDownSwapStartX;
    int32_t componentDownSwapEndY = endY_ - OFFSET;
    INFO_LOG_STR("Component Down Swap: (%d, %d) -> (%d, %d)", componentDownSwapStartX, componentDownSwapStartY,
                 componentDownSwapEndX, componentDownSwapEndY);
    ErrCode result = MultimodeManager::GetInstance()->IntervalSwap(componentDownSwapStartX, componentDownSwapStartY,
                                                                   componentDownSwapEndX, componentDownSwapEndY);
    usleep(TWOSECONDS);
    return result;
}

ErrCode ComponentManager::ComponentMultikeyInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result = ComponentTouchInput(elementInfo);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    usleep(ONESECOND);
    result = MultimodeManager::GetInstance()->MultiKeyCodeInput(DOWNTIME);
    return result;
}

ErrCode ComponentManager::ComponentLeftSwapInput(Accessibility::AccessibilityElementInfo& elementInfo)
{
    ErrCode result;
    // Calculate swap position
    int32_t leftSwapStartX = startX_ + OFFSET;
    int32_t leftSwapEndX = endX_ + OFFSET;
    int32_t leftSwapStartY = startY_ + (endY_ - startY_) / DIV;
    int32_t leftSwapEndY = leftSwapStartY;
    INFO_LOG_STR("Component Left Swap: (%d, %d) -> (%d, %d)", leftSwapStartX, leftSwapStartY, leftSwapEndX,
                 leftSwapEndY);
    result = MultimodeManager::GetInstance()->IntervalSwap(leftSwapStartX, leftSwapStartY, leftSwapEndX, leftSwapEndY);
    return result;
}

void ComponentManager::GetComponentPosition(Accessibility::AccessibilityElementInfo& elementInfo)
{
    Accessibility::Rect componentBounds = elementInfo.GetRectInScreen();
    startX_ = componentBounds.GetLeftTopXScreenPostion();
    startY_ = componentBounds.GetLeftTopYScreenPostion();
    endX_ = componentBounds.GetRightBottomXScreenPostion();
    endY_ = componentBounds.GetRightBottomYScreenPostion();
}
}  // namespace WuKong
}  // namespace OHOS

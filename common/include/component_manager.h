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
#ifndef TEST_WUKONG_COMPONENT_MANAGER_H
#define TEST_WUKONG_COMPONENT_MANAGER_H

#include "accessibility_element_info.h"
#include "accessibility_ui_test_ability.h"
#include "component_tree.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
enum ComponentStatus {
    COMPONENT_STATUS_DISCONNECT,
    COMPONENT_STATUS_CONNECTED,
    COMPONENT_STATUS_CONNECTING
};

class ComponentManagerListener {
public:
    virtual void OnStatusUpdated(ComponentStatus stutus) = 0;
    virtual void OnScreenUpdated() = 0;
    virtual void OnPermissionScreenShown() = 0;
};
class ComponentManager : public DelayedSingleton<ComponentManager> {
public:
    bool Connect();

    void Disconnect();

    uint32_t AddRegisterListener(std::shared_ptr<ComponentManagerListener> listener);

    void DeleteRegisterListener(const uint32_t handle);

    ErrCode GetReportInfo(std::string& info);

    ErrCode PermoissionInput();

    std::vector<std::shared_ptr<ComponentManagerListener>> GetListenerList();

    ErrCode ComponentEventInput(OHOS::Accessibility::AccessibilityElementInfo& elementInfo,
                                const OHOS::Accessibility::ActionType actionType);

    /**
     * @brief Create a Event Input Map.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode CreateEventInputMap();

    /**
     * @brief input touch event for the target component.
     * @param elementInfo element, also call component.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentTouchInput(Accessibility::AccessibilityElementInfo& elementInfo);

    ErrCode BackToPrePage();

    ErrCode ComponentUpSwapInput(Accessibility::AccessibilityElementInfo& elementInfo);

    ErrCode ComponentDownSwapInput(Accessibility::AccessibilityElementInfo& elementInfo);

    ErrCode ComponentMultikeyInput(Accessibility::AccessibilityElementInfo& elementInfo);

    DECLARE_DELAYED_SINGLETON(ComponentManager);

private:
    std::vector<std::shared_ptr<ComponentManagerListener>> listenerList_;
    bool connected_ = false;
    int injectTimes_ = 0;
    OHOS::Accessibility::ActionType actionType;
    std::map<Accessibility::ActionType, std::function<ErrCode(Accessibility::AccessibilityElementInfo&)>> componentMap_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_COMPONENT_MANAGER_H

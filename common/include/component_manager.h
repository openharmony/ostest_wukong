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
    virtual void OnStatusUpdated(ComponentStatus status) = 0;
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

    /**
     * @brief input multimode event for the current page.
     * @param elementInfo current component
     * @param actionType the action type of current component
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentEventInput(OHOS::Accessibility::AccessibilityElementInfo& elementInfo, const int actionType);

    /**
     * @brief input back event for the current page.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode BackToPrePage();

    DECLARE_DELAYED_SINGLETON(ComponentManager);

private:
    bool connected_ = false;

    int32_t startX_ = -1;
    int32_t endX_ = -1;
    int32_t startY_ = -1;
    int32_t endY_ = -1;

    std::vector<std::shared_ptr<ComponentManagerListener>> listenerList_;
    std::map<int, std::function<ErrCode(Accessibility::AccessibilityElementInfo&)>> componentMap_;

    /**
     * @brief get the positin of current component .
     * @param elementInfo current component.
     */
    void GetComponentPosition(Accessibility::AccessibilityElementInfo& elementInfo);

    /**
     * @brief input up swap event for the current component.
     * @param elementInfo current component.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentUpSwapInput(Accessibility::AccessibilityElementInfo& elementInfo);

    /**
     * @brief input dowan swap event for the target component.
     * @param elementInfo element, also call component.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentDownSwapInput(Accessibility::AccessibilityElementInfo& elementInfo);

    /**
     * @brief input keyboard event for the target component.
     * @param elementInfo element, also call component.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentMultikeyInput(Accessibility::AccessibilityElementInfo& elementInfo);

    /**
     * @brief input left swap event for the target component.
     * @param elementInfo element, also call component.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentLeftSwapInput(Accessibility::AccessibilityElementInfo& elementInfo);

    /**
     * @brief input touch event for the target component.
     * @param elementInfo element, also call component.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode ComponentTouchInput(Accessibility::AccessibilityElementInfo& elementInfo);

    /**
     * @brief Create a Event Input Map.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode CreateEventInputMap();
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_COMPONENT_MANAGER_H

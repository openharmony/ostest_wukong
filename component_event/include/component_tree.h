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

#ifndef TEST_WUKONG_COMPONENT_TREE_H
#define TEST_WUKONG_COMPONENT_TREE_H

#include <map>

#include "accessibility_element_info.h"
#include "wukong_define.h"
#include "wukong_tree.h"

namespace OHOS {
namespace WuKong {
class ComponentTree : public WuKongTree {
public:
    ComponentTree() : WuKongTree(), expectedInputCount_(0)
    {
        type_.clear();
        inputTypeCountMap_.clear();
    }
    virtual ~ComponentTree()
    {
    }

    const std::string& GetType()
    {
        return type_;
    }

    bool IsVisible()
    {
        return isVisible_;
    }

    OHOS::Accessibility::Rect GetPosition()
    {
        return rect_;
    }

private:
    friend class TreeManager;
    virtual bool SetNodeId() override;
    virtual bool RecursUpdateInfo(const std::shared_ptr<ComponentTree>& source);
    void AddTypeInputCount(uint32_t type)
    {
        auto it = inputTypeCountMap_.find(type);
        if (it != inputTypeCountMap_.end()) {
            it->second++;
        } else {
            inputTypeCountMap_[type] = 1;
        }
    }
    OHOS::Accessibility::Rect rect_ {};
    bool isVisible_ = false;
    std::string type_;
    uint32_t expectedInputCount_;
    std::map<uint32_t, uint32_t> inputTypeCountMap_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_COMPONENT_TREE_H

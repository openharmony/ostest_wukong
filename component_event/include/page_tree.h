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

#ifndef TEST_WUKONG_PAGE_TREE_H
#define TEST_WUKONG_PAGE_TREE_H

#include "component_tree.h"
#include "wukong_define.h"
#include "wukong_tree.h"

namespace OHOS {
namespace WuKong {
class PageTree : public WuKongTree {
public:
    PageTree() : WuKongTree(), validComponentCount_(0), allComponentCount_(0)
    {
    }
    virtual ~PageTree()
    {
        TRACK_LOG_STR("Node (0x%016llX)", !parent_.expired(), nodeId_);
    }

    void SetValidComponentCount(uint32_t count)
    {
        validComponentCount_ = count;
    }
    void SetAllComponentCount(uint32_t count)
    {
        allComponentCount_ = count;
    }
    uint32_t GetValidComponentCount()
    {
        return validComponentCount_;
    }
    uint32_t GetAllComponentCount()
    {
        return allComponentCount_;
    }
    const std::shared_ptr<ComponentTree>& GetCurrentComponentNode()
    {
        return inputElementInfo_;
    }

private:
    friend class TreeManager;
    virtual bool SetNodeId() override;
    void SetCurrentComponentNode(const std::shared_ptr<ComponentTree>& info)
    {
        inputElementInfo_ = info;
    }
    std::shared_ptr<ComponentTree> inputElementInfo_ = nullptr;
    uint32_t validComponentCount_ = 0;
    uint32_t allComponentCount_ = 0;
    uint32_t count_ = 0;
};
}  // namespace WuKong
}  // namespace OHOS

#endif

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

#ifndef TEST_WUKONG_WUKONG_TREE_H
#define TEST_WUKONG_WUKONG_TREE_H

#include <vector>

#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class WuKongTree {
public:
    static const std::shared_ptr<WuKongTree> GetRoot(const std::shared_ptr<WuKongTree>& child)
    {
        if (child->GetParent() == nullptr) {
            return child;
        }
        return GetRoot(child->GetParent());
    }

public:
    WuKongTree() : nodeId_(0), index_(0), inputCount_(0), parent_(), children_(0)
    {
    }
    virtual ~WuKongTree()
    {
    }

    /**
     * @brief Get node id for node specification value.
     * @return uint64 specification value.
     */
    virtual uint64_t GetNodeId()
    {
        return nodeId_;
    }

    /**
     * @brief Get Index for value list index.
     * @return uint32 index value.
     */
    virtual uint32_t GetIndex()
    {
        return index_;
    }

    /**
     * @brief Get node parent pointer, if root node return nullptr.
     * @return WuKongTree shared pointer.
     */
    virtual const std::shared_ptr<WuKongTree> GetParent()
    {
        TRACK_LOG_STR("current note has parent (%d), Node (0x%016llX)", !parent_.expired(), nodeId_);
        return parent_.lock();
    }

    /**
     * @brief Get node children pointer, return 0 if no child.
     * @return WuKongTree vector.
     */
    virtual const std::vector<std::shared_ptr<WuKongTree>>& GetChildren()
    {
        return children_;
    }

    /**
     * @brief Get node input event count for input statistics.
     * @return uint32 statistics value.
     */
    virtual uint32_t GetInputCount()
    {
        return inputCount_;
    }
    /**
     * @brief Are they equal, and check nodeId.
     * @param other WuKongTree shared pointer.
     * @return return compare result
     */
    virtual bool IsEqual(const std::shared_ptr<WuKongTree>& other)
    {
        if (other == nullptr) {
            return false;
        }
        return (nodeId_ == other->nodeId_);
    }

protected:
    friend class TreeManager;
    virtual bool SetNodeId() = 0;
    virtual void SetIndex(uint32_t index)
    {
        index_ = index;
    }
    virtual void SetParent(const std::shared_ptr<WuKongTree>& parent)
    {
        parent_ = parent;
    }
    virtual void AddChild(const std::shared_ptr<WuKongTree>& child)
    {
        children_.push_back(child);
    }
    virtual void AddInputCount()
    {
        inputCount_++;
    }
    bool RecursUpdateNodeIndex(const uint32_t offset);
    uint64_t GetSubName(std::string name, uint32_t count);

    /**
     * @brief node specification value.
     */
    uint64_t nodeId_;
    /**
     * @brief value list index.
     */
    uint32_t index_;
    /**
     * @brief input statistics.
     */
    uint32_t inputCount_;
    /**
     * @brief node parent pointer.
     */
    std::weak_ptr<WuKongTree> parent_;
    /**
     * @brief node children pointers.
     */
    std::vector<std::shared_ptr<WuKongTree>> children_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_WUKONG_TREE_H

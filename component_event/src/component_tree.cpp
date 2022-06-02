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

#include "component_tree.h"

#include "tree_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint8_t COMPONENT_TYPE_POSION = 48;
const uint8_t COMPONENT_WIDTH_POSION = 36;
const uint8_t COMPONENT_HEIGHT_POSION = 24;
const uint8_t COMPONENT_CONTENT_POSION = 8;
const uint8_t COMPONENT_RESERVED_POSION = 0;
uint64_t GetSubName(std::string name, int count)
{
    TRACK_LOG_STR("name %s", name.c_str());
    const uint8_t heightPosion = 8;
    uint64_t subName = 0;
    uint32_t nameSize = name.size();
    for (uint32_t index = count; index > 0; index--) {
        if (index > nameSize) {
            continue;
        }
        subName |= name[nameSize - index];
        if (index > 1) {
            subName = subName << heightPosion;
        }
    }
    return subName;
}
}  // namespace
bool ComponentTree::RecursUpdateInfo(const std::shared_ptr<ComponentTree>& source)
{
    // Copy source count info to new component node.
    expectedInputCount_ = source->expectedInputCount_;
    inputCount_ = source->inputCount_;
    inputTypeCountMap_ = source->inputTypeCountMap_;

    // Recurs component tree for copy count info.
    for (auto child : children_) {
        for (auto sourceChild : source->children_) {
            if (child->IsEqual(sourceChild)) {
                std::static_pointer_cast<ComponentTree>(child)->RecursUpdateInfo(
                    std::static_pointer_cast<ComponentTree>(sourceChild));
                break;
            }
        }
    }
    return true;
}

/**
 * @brief component Node Id specification format
 * @details
 * |----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
 * |TYPE-16            |WIDTH-12      |HEIGHT-12     |CONTENT - 16       |RESERVED |
 */
bool ComponentTree::SetNodeId()
{
    const uint8_t typeCount = 2;
    const uint8_t contentCount = 2;
    const uint8_t div = 16;
    nodeId_ = 0;
    auto treeMgr = TreeManager::GetInstance();
    auto elementInfo = treeMgr->GetNewElementInfoList(index_);
    if (elementInfo == nullptr) {
        ERROR_LOG("get new element info is nullptr");
        return false;
    }

    auto rect = elementInfo->GetRectInScreen();
    // type is component type or component id of the ElementInfo
    uint64_t type = GetSubName(elementInfo->GetComponentType(), typeCount);
    // w is width of the ElementInfo
    uint64_t w = rect.GetRightBottomXScreenPostion() - rect.GetLeftTopXScreenPostion();
    // h is width of the ElementInfo
    uint64_t h = rect.GetRightBottomYScreenPostion() - rect.GetLeftTopYScreenPostion();
    // the ElementInfo content of 2 length
    uint64_t str = GetSubName(elementInfo->GetContent(), contentCount);

    // the ElementInfo name of 4 length
    uint64_t xy = (rect.GetLeftTopXScreenPostion() / div + rect.GetLeftTopYScreenPostion() / div);
    TRACK_LOG_STR("component Type: (%d), Width: (%d), Height: (%d)", (uint32_t)type, (uint32_t)w, (uint32_t)h);
    nodeId_ |= type << COMPONENT_TYPE_POSION;
    nodeId_ |= w << COMPONENT_WIDTH_POSION;
    nodeId_ |= h << COMPONENT_HEIGHT_POSION;
    nodeId_ |= str << COMPONENT_CONTENT_POSION;
    nodeId_ |= xy << COMPONENT_RESERVED_POSION;
    TRACK_LOG_STR("component Node ID: (0x%016llX)", nodeId_);
    return true;
}
}  // namespace WuKong
}  // namespace OHOS

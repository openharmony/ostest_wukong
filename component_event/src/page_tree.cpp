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

#include "page_tree.h"

#include "tree_manager.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint8_t PAGE_COUNT_POSION = 50;
const uint8_t PAGE_NODE_COUNT_POSION = 36;
const uint8_t PAGE_BRANCH_COUNT_POSION = 28;
const uint8_t PAGE_HEIGHT_POSION = 22;
const uint8_t PAGE_TWO_LAYER_WIDTH_POSION = 14;
const uint8_t PAGE_LAST_LAYER_WIDTH_POSION = 0;
uint64_t pageCount = 0;
uint64_t nodeCount = 0;
uint64_t branchCount = 0;
uint32_t layer = 0;
uint32_t height = 0;
uint32_t lastWidth = 0;
bool g_isLeftBranch = false;
void RecursStatistics(std::shared_ptr<ComponentTree> parent)
{
    // all page node count Statistics
    pageCount++;
    uint32_t childCount = parent->GetChildren().size();

    // layer pointer move to next
    layer++;
    if (childCount == 0) {
        // node Statistics
        nodeCount++;
    } else {
        // branch Statistics
        branchCount++;
        if (layer >= height) {
            // tree max height Statistics
            height++;

            // last tree width.
            lastWidth = childCount;
        }
    }

    // recurs child.
    for (auto child : parent->GetChildren()) {
        RecursStatistics(std::static_pointer_cast<ComponentTree>(child));
    }
    // layer pointer move to previous
    layer--;
}
}  // namespace

/**
 * @brief Page Node Id specification format
 * @details
 * |----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
 * |ALL COUNT -14   |NODECOUNT-14|BRHCNT -8|TRH |TWO WIDTH -14   |LAST WIDTH -14   |
 */
bool PageTree::SetNodeId()
{
    nodeId_ = 0;
    auto componentTree = TreeManager::GetInstance()->GetNewComponents();
    if (componentTree->GetNodeId() == 0) {
        WARN_LOG("Component Tree is Empty");
        return false;
    }
    if (componentTree->GetParent() != nullptr) {
        WARN_LOG("Component Tree is not root");
        return false;
    }

    // init statistics variables
    pageCount = 0;
    nodeCount = 0;
    branchCount = 0;
    height = 1;
    layer = 0;
    lastWidth = 0;
    g_isLeftBranch = true;

    // recurs statistics
    RecursStatistics(componentTree);

    uint32_t twoWidth = componentTree->GetChildren().size();
    DEBUG_LOG_STR("Page Count: (%d), Node: (%d), Branch: (%d), Height: (%d), Two Width: (%d), Last Width: (%d)",
                  (uint32_t)pageCount, (uint32_t)nodeCount, (uint32_t)branchCount, height, lastWidth, twoWidth);
    count_ = (uint32_t)pageCount;

    // make node id for compare page.
    nodeId_ |= pageCount << PAGE_COUNT_POSION;
    nodeId_ |= nodeCount << PAGE_NODE_COUNT_POSION;
    nodeId_ |= branchCount << PAGE_BRANCH_COUNT_POSION;
    nodeId_ |= height << PAGE_HEIGHT_POSION;
    nodeId_ |= twoWidth << PAGE_TWO_LAYER_WIDTH_POSION;
    nodeId_ |= lastWidth << PAGE_LAST_LAYER_WIDTH_POSION;
    TRACK_LOG_STR("Page Node ID: (0x%016llX)", nodeId_);
    return true;
}
}  // namespace WuKong
}  // namespace OHOS

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
    PageTree() : WuKongTree(), validCount_(0), invalidCount_(0), count_(0)
    {
    }
    virtual ~PageTree()
    {
    }

private:
    friend class TreeManager;
    virtual bool SetNodeId() override;

    uint32_t validCount_;
    uint32_t invalidCount_;
    uint32_t count_;
};
}  // namespace WuKong
}  // namespace OHOS

#endif
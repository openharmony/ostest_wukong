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

#ifndef TEST_WUKONG_ABILITY_TREE_H
#define TEST_WUKONG_ABILITY_TREE_H

#include "page_tree.h"
#include "wukong_define.h"
#include "wukong_tree.h"

namespace OHOS {
namespace WuKong {
class AbilityTree : public WuKongTree {
public:
    AbilityTree()
        : WuKongTree(),
          deviceId_(""),
          bundleName_(""),
          abilityName_(""),
          moduleName_(""),
          allComponentCount_(0)
    {
    }
    virtual ~AbilityTree()
    {
        TRACK_LOG_END();
    }
    uint32_t GetAllComponentCount()
    {
        return allComponentCount_;
    }
    const std::string& GetBundleName()
    {
        return bundleName_;
    }
    const std::string& GetAbilityName()
    {
        return abilityName_;
    }

private:
    friend class TreeManager;
    virtual bool SetNodeId() override;
    void AddAllComponentCount(uint32_t count)
    {
        allComponentCount_ += count;
        DEBUG_LOG_STR("Add count allComponentCount_ (%u), count (%u)", allComponentCount_, count);
    }
    std::string deviceId_;
    std::string bundleName_;
    std::string abilityName_;
    std::string moduleName_;
    uint32_t allComponentCount_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_ABILITY_TREE_H

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

#include "wukong_tree.h"

namespace OHOS {
namespace WuKong {
bool WuKongTree::RecursUpdateNodeIndex(const uint32_t offset)
{
    index_ = index_ + offset;
    for (auto child : children_) {
        child->RecursUpdateNodeIndex(offset);
    }
    return true;
}
uint64_t WuKongTree::GetSubName(std::string name, uint32_t count, bool isAbility)
{
    TRACK_LOG_STR("name %s", name.c_str());
    const uint8_t heightPosion = 8;
    uint64_t subName = 0;
    uint32_t nameSize = name.size();
    if (isAbility) {
        GetClearnAbility(name);
        nameSize = name.size();
        for (uint32_t index = count; index > 0; index--) {
            if (index > nameSize) {
                continue;
            }
            subName |= name[nameSize - index];
            if (index > 1) {
                subName = subName << heightPosion;
            }
        }
    } else {
        for (uint32_t index = count; index > 0; index--) {
            if (index > nameSize) {
                continue;
            }
            subName |= name[nameSize - index];
            if (index > 1) {
                subName = subName << heightPosion;
            }
        }
    }
    return subName;
}
void WuKongTree::GetClearnAbility(std::string &name)
{
    std::string target = "Ability";
    std::string replacement = "";
    size_t pos = name.find(target);
    while (pos != std::string::npos) {
        name.replace(pos, target.length(), replacement);
        pos = name.find(target, pos + replacement.length());
    }
}
}  // namespace WuKong
}  // namespace OHOS
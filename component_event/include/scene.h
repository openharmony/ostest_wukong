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

#ifndef TEST_WUKONG_SCENE_H
#define TEST_WUKONG_SCENE_H

#include "component_tree.h"

namespace OHOS {
namespace WuKong {
class Scene {
protected:
    /**
     * @brief judge the component type and store it in the list
     * @param inputComponentList the list of component that can input
     * @param componentList the list of all component
     * @return ERR_OK is success,other is fail
     */
    virtual ErrCode SetInputComponentList(std::vector<std::shared_ptr<ComponentTree>> &componentList) = 0;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_SCENE_H

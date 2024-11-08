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

#ifndef TEST_WUKONG_NORMAL_SCENE_H
#define TEST_WUKONG_NORMAL_SCENE_H

#include "scene.h"
#include "tree_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class NormalScene : public Scene {
public:
    NormalScene();
    virtual ~NormalScene();

    virtual ErrCode SetInputComponentList(std::vector<std::shared_ptr<ComponentTree>> &componentList) override;

    /**
     * @brief judge whether to back to the previous page
     * @return true is back,false is not back
     */
    bool IsBackToPrePage()
    {
        return isBack_;
    }

    ErrCode SetInputComponent(std::vector<std::shared_ptr<ComponentTree>> &componentList,
                              std::shared_ptr<ComponentTree> &componentinfo);

    ErrCode SetInputComponentListForFocusInput(std::vector<std::shared_ptr<ComponentTree>> &componentList);

    void SubtractComponent(std::vector<std::shared_ptr<ComponentTree>> &componentList,
    std::vector<uint32_t> &indexList);

    bool IsComponentBlock(std::shared_ptr<ComponentTree> &comp, std::vector<std::string> &compIdBlock,
    std::vector<std::string> &compTypeBlock);

private:
    bool isBack_ = false;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_NORMAL_SCENE_H

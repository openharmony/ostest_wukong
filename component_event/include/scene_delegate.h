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

#ifndef TEST_WUKONG_SCENE_DELEGATE_H
#define TEST_WUKONG_SCENE_DELEGATE_H

#include "singleton.h"
#include "tree_manager.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class SceneDelegate : public DelayedSingleton<SceneDelegate> {
public:
    SceneDelegate();
    ~SceneDelegate();

    /**
     * @brief judge the scene
     * @param isRandom whether it is a random test
     * @return ERR_OK is success,other is fail
     */
    ErrCode ChooseScene(bool isRandom);

    /**
     * @brief judge whether to back to the previous page
     * @return true is back,false is not back
     */
    bool IsBackToPrePage()
    {
        return isBack_;
    }

    std::vector<std::string> GetComponentTypeList()
    {
        return componentType_;
    }

    uint64_t GetCurrentPageId()
    {
        return pageId_;
    }

private:
    /**
     * @brief compare two component tree
     * @param componentinfo the information of a component tree
     * @param othercomponentinfo the information of another component tree
     * @return ERR_OK is success,other is fail
     */
    ErrCode CompareComponentInfos(std::shared_ptr<ComponentTree> &newcomponentinfo,
                                  std::shared_ptr<ComponentTree> &oldcomponentinfo, bool isRandom);

    /**
     * @brief get all componentinfo of active page
     * @param componentinfo the root component node of active page
     * @return ERR_OK is success,other is fail
     */
    ErrCode GetCurrentComponentInfo(std::shared_ptr<ComponentTree> componentinfo,
                                    std::vector<std::shared_ptr<ComponentTree>> &componentlist);

    /**
     * @brief set the available component
     * @param componentinfo the root component node of active page
     * @return ERR_OK is success,other is fail
     */
    ErrCode SetAvailableComponentList(std::shared_ptr<ComponentTree> componentinfo, bool isRandom);

    /**
     * @brief find the same component in new component list and old component list
     * @param newcomponentlist new component list
     * @param oldcomponentlist old component list
     * @return same count
     */
    uint32_t FindSame(const std::vector<std::shared_ptr<ComponentTree>> &newcomponentlist,
                      const std::vector<std::shared_ptr<ComponentTree>> &oldcomponentlist);

    /**
     * @brief set the available component
     * @param isFound whether newcomponentinfos is same with its parent node
     * @param isRandom whether it is a random test
     * @return ERR_OK is success,other is fail
     */
    ErrCode FindSamePageInChildren(bool &isFound, bool isRandom);

    /**
     * @brief set the available component
     * @param isFound whether newcomponentinfos is same with its child node
     * @param isRandom whether it is a random test
     * @return ERR_OK is success,other is fail
     */
    ErrCode FindSamePageInParent(bool &isFound, bool isRandom);

    /**
     * @brief check if the component is in screen
     * @param componentinfo the component
     * @return true is in screen,false is not in screen
     */
    bool IsComponentInScreen(const std::shared_ptr<ComponentTree> componentinfo);

    std::vector<std::shared_ptr<ComponentTree>> componentList_;
    std::vector<std::string> componentType_;
    bool isBack_ = false;
    uint64_t pageId_ = 0;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_SCENE_DELEGATE_H

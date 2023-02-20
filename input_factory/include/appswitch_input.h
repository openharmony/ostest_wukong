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

#ifndef TEST_WUKONG_APPSWITCH_INPUT_H
#define TEST_WUKONG_APPSWITCH_INPUT_H

#include <string>

#include "input_action.h"
#include "special_test_object.h"
#include "input_msg_object.h"
namespace OHOS {
namespace WuKong {
class AppswitchInput : public InputAction {
public:
    AppswitchInput();
    ~AppswitchInput();

    ErrCode OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject);

    /**
     * @brief started up a app by ability.
     * @return Return ERR_OK on success， others on failure.
     */
    ErrCode RandomInput();

    /**
     * @brief get information of inputted event.
     * @return Return ERR_OK on success， others on failure.
     */
    ErrCode GetInputInfo();

    /**
     * @brief output the information: if the app has started up successfully.
     * @param result
     * @param index bundlelist's index.
     * @return Return ERR_OK on success， others on failure.
     */
    ErrCode PrintResultOfStartAbility(const ErrCode result, uint32_t index);

private:
    /**
     * @brief get ability name  index through bundle name index
     * @param bundleList the list of all bundle name
     * @param index index of ability name in ability name list.
     * @return Return ERR_OK on success,others on failure.
     */
    uint32_t GetAbilityIndex(std::vector<std::string> &bundleList);

    std::shared_ptr<InputedMsgObject> inputedMsgObject_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_APPSWITCH_INPUT_H

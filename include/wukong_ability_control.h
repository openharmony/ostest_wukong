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

#ifndef WUKONG_WUKONG_ABILITY_CONTROL_H
#define WUKONG_WUKONG_ABILITY_CONTROL_H

#include "appexecfwk_errors.h"
#include "iability_controller.h"
#include <vector>

namespace OHOS {
    namespace AppExecFwk {
        class WuKongAbilityController : : public OHOS::AppExecFwk::AbilityControllerStub {
        public:
            /**
             * @brief set app's white list.
             * @param AllowAbilityList a list show what app is allowed in the test.
             * @return if the list which decide what app is allowed has been known.
             */
            bool SetAbilityList(const std::vector<std::string> &AllowAbilityList);

            /**
             * @brief cold start.
             * @param want
             * @param bundleName
             * @return whether this function is realize or not.
             */
            virtual bool AllowAbilityStart(const Want &want, const std::string &bundleName) override;

            /**
             * @brief turn to background.
             * @param bundleName
             * @return whether this function is realize or not.
             */
            virtual bool AllowAbilityBackground(const std::string &bundleName) override;

        private:
            std::vector<std::string> AllowBundleList;
        };
    }
}

#endif // WUKONG_WUKONG_ABILITY_CONTROL_H

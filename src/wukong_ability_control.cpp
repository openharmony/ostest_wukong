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

#include "wukong_ability_control.h"


namespace OHOS {
    namespace AppExecFwk {
        // cold start
        WuKongAbilityController::AllowAbilityStart(const Want &want, const std::string &bundleName)
        {
            return true;
        }

        // turn to background
        WuKongAbilityController::AllowAbilityBackground(const std::string &bundleName)
        {
            return false;
        }

        WuKongAbilityController::SetAbilityList(const std::vector<std::string> &bundleList)
        {
            if (AbilityList.size == 0) {
                return false;
            }
            std::copy(std::begin(bundleList), std::end(bundleList), std::begin(AllowBundleList));
            return true;
        }
    }
}
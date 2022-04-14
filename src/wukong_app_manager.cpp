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

#include "wukong_app_manager.h"
#include "element_name.h"
#include "ability_manager_client.h"
#include "app_log_wrapper.h"

namespace OHOS {
    namespace AppExecFwk {
        // start ability by bundleInfo's entryHapName and entryHap->mainAbility。
        ErrCode WuKongAppManager::StartAbilityByBundleInfo(std::string AbilityNameArgs, std::string BundleNameArgs)
        {
            Want want;
            int result;

            if (AbilityNameArgs.size() == 0 || BundleNameArgs.size() == 0) {
                if (AbilityNameArgs.size() == 0) {
                    resultReceiver_.append(HELP_MSG_NO_ABILITY_NAME_OPTION + "\n");
                }
                if (BundleNameArgs.size() == 0) {
                    resultReceiver_.append(HELP_MSG_NO_BUNDLE_NAME_OPTION + "\n");
                }
                result = OHOS::ERR_INVALID_VALUE;
            } else {
                ElementName element("", BundleNameArgs, AbilityNameArgs);
                want.SetElement(element);
                result = OHOS::AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
                if (result == OHOS::ERR_OK) {
                    resultReceiver_ = STRING_START_ABILITY_OK + "\n";
                } else {
                    resultReceiver_ = STRING_START_ABILITY_NG + "\n";
                }
            }
            return result;
        }
    }
}

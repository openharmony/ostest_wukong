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

#ifndef WUKONG_WUKONG_APP_MANAGER_H
#define WUKONG_WUKONG_APP_MANAGER_H

#include "appexecfwk_errors.h"
#include "singleton.h"

namespace OHOS {
    namespace AppExecFwk {
        namespace {
            const std::string HELP_MSG_NO_ABILITY_NAME_OPTION = "error: -a <ability-name> is expected";
            const std::string HELP_MSG_NO_BUNDLE_NAME_OPTION = "error: -b <bundle-name> is expected";

            const std::string STRING_START_ABILITY_OK = "start ability successfully.";
            const std::string STRING_START_ABILITY_NG = "error: failed to start ability.";
        }
        class WuKongAppManager : public DelayedSingleton<WuKongAppManager> {
        public:
            /**
             * @brief start ability by bundleInfo's entryHapName and entryHap->mainAbility
             * @param entryHapName EntryHap Name
             * @param mainAbilityName entryHap->mainAbility Name
             * @return Returns ERR_OK on success, others on failure.
             */
            ErrCode StartAbilityByBundleInfo(std::string AbilityNameArgs, std::string BundleNameArgs);

        protected:
            std::string resultReceiver_ = "";
        };
    }
}

#endif // WUKONG_WUKONG_APP_MANAGER_H

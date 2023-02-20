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
#ifndef TEST_WUKONG_APP_MANAGER_H
#define TEST_WUKONG_APP_MANAGER_H

#include "ability_controller_stub.h"
#include "appexecfwk_errors.h"
#include "singleton.h"

namespace OHOS {
namespace WuKong {
class AppManager : public DelayedSingleton<AppManager> {
public:
    /**
     * @brief start ability by bundleInfo's entryHapName and entryHap->mainAbility
     * @param abilityName ability name
     * @param bundleName bundle name
     * @return Returns ERR_OK on success, others on failure.
     */
    ErrCode StartAbilityByBundleInfo(std::string abilityName, std::string bundleName);
    void SetAbilityController();

private:
    class BlockAbilityController : public OHOS::AppExecFwk::AbilityControllerStub {
    public:
        /**
         * @brief cold start.
         * @param want want information
         * @param bundleName bundle name
         * @return whether this function is realize or not.
         */
        virtual bool AllowAbilityStart(const AAFwk::Want &want, const std::string &bundleName) override;

        /**
         * @brief turn to background.
         * @param bundleName bundle name
         * @return whether this function is realize or not.
         */
        virtual bool AllowAbilityBackground(const std::string &bundleName) override;

    private:
        std::vector<std::string> AllowBundleList;
    };

    // block ability controller
    sptr<BlockAbilityController> abilityController_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_APP_MANAGER_H

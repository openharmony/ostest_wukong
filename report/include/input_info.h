/*
 * @Description:
 * @Date: 2022-05-15 18:49:26
 * @LastEditTime: 2022-05-15 19:49:06
 * @FilePath: /wukong/report/include/input_info.h
 * @author: lemon
 */
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

#ifndef TEST_WUKONG_INPUF_INFO_H
#define TEST_WUKONG_INPUF_INFO_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "input_action.h"
#include "report.h"
#include "singleton.h"

namespace OHOS {
namespace WuKong {
class InputInfo final : public DelayedSingleton<InputInfo> {
    DECLARE_DELAYED_SINGLETON(InputInfo)
    friend class Report;
public:
    void SetBundleName(const std::string &bundleName);
    void SetAbilityName(const std::string &abilityName);
    void SetInputType(InputType inputType);
    void SetComponmentName(const std::string &componmentName);
    void SetComponmentInputedTimes(int componmentInputedTimes);
    void SetComponmentTotals(int componmentTotals);
private:
    std::string GetBundleName();
    std::string GetAbilityName();
    std::string GetComponmentName();
    std::string GetComponmentInputedTimes();
    std::string GetComponmentTotals();
    void reset();
    /*
     * @brief update current conponment statistics
     * @param componmentStatistics  in order mean componment type, input times, total componment
     * @return
     */
    std::string InputTypeToString();

    std::string bundleName_;
    std::string abilityName_;
    InputType inputType_;
    std::string componmentName_;
    int componmentInputedTimes_;
    int componmentTotals_;
};
}  // namespace WuKong
}  // namespace OHOS

#endif
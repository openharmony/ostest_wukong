/*
 *Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef TEST_WUKONG_STATISTICS_ABILITY_H
#define TEST_WUKONG_STATISTICS_ABILITY_H
#include <iomanip>
#include <string>

#include "data_set.h"
#include "statistics.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {

namespace {
class AbilityStatisticsRecord {
public:
    std::string bundleName_ = "";
    uint32_t inputedAbilityCount_ = 0;
    uint32_t abilitiesCount_ = 0;
    std::vector<std::string> inputedAbilities_;
};
}  // namespace

class StatisticsAbility : public Statistics {
public:
    StatisticsAbility() = default;
    ~StatisticsAbility() = default;
    void StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                          std::map<std::string, std::shared_ptr<Table>> &destTables);

private:
    void UpdateLine(std::shared_ptr<AbilityStatisticsRecord> abilityStatisticsRecordPtr,
                    std::vector<std::string> &line);
    void SrcDatasPretreatment(std::vector<std::map<std::string, std::string>> srcDatas);
    std::map<std::string, std::shared_ptr<AbilityStatisticsRecord>> abilityStatisticsMsg_;
    std::vector<std::string> bundles_;
    std::vector<std::string> headers_ = {"bundleName", "inputedAbilityCount", "abilitiesCount", "coverage"};
    std::vector<std::vector<std::string>> record_;
};

}  // namespace WuKong
}  // namespace OHOS
#endif
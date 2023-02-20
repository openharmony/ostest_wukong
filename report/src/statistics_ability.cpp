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

#include "statistics_ability.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint32_t DECIMAL_LENGTH = 2;
const float PERCENTAGE = 100.0;
}

void StatisticsAbility::StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                                         std::map<std::string, std::shared_ptr<Table>> &destTables)
{
    SrcDatasPretreatment(srcDatas);
    std::shared_ptr<AbilityStatisticsRecord> globalAbilityStatisticsRecordPtr =
        std::make_shared<AbilityStatisticsRecord>();
    globalAbilityStatisticsRecordPtr->bundleName_ = "total";
    std::vector<std::string> line;
    for (auto bundle : abilityStatisticsMsg_) {
        DEBUG_LOG_STR("start bundlename{%s}", bundle.first.c_str());
        std::shared_ptr<AbilityStatisticsRecord> curAbilityStatisticsRecordPtr = bundle.second;
        globalAbilityStatisticsRecordPtr->inputedAbilityCount_ += curAbilityStatisticsRecordPtr->inputedAbilityCount_;
        globalAbilityStatisticsRecordPtr->abilitiesCount_ += curAbilityStatisticsRecordPtr->abilitiesCount_;

        UpdateLine(curAbilityStatisticsRecordPtr, line);
        record_.push_back(line);
    }
    UpdateLine(globalAbilityStatisticsRecordPtr, line);
    record_.push_back(line);
    std::shared_ptr<Table> table = std::make_shared<Table>(headers_, record_);
    record_.clear();
    table->SetName("all");
    table->SetDetail("ability");
    destTables["all"] = table;
}

void StatisticsAbility::SrcDatasPretreatment(std::vector<std::map<std::string, std::string>> srcDatas)
{
    for (auto srcData : srcDatas) {
        DEBUG_LOG_STR("bundlename{%s} | abilityName{%s}", srcData["bundleName"].c_str(),
                      srcData["abilityName"].c_str());

        // check whether bundle is entered resolve create or reuse already exist  StatisticsMsgPtr
        std::shared_ptr<AbilityStatisticsRecord> curAbilityStatisticsRecordPtr = std::make_shared<AbilityStatisticsRecord>();
        std::map<std::string, std::shared_ptr<AbilityStatisticsRecord>>::iterator abilityStatisticsRecordIter =
            abilityStatisticsMsg_.find(srcData["bundleName"]);
        if (abilityStatisticsRecordIter != abilityStatisticsMsg_.end()) {
            DEBUG_LOG_STR("use inited curStatisticsMsgPtr by bundleName{%s}", srcData["bundleName"].c_str());
            curAbilityStatisticsRecordPtr = abilityStatisticsMsg_[srcData["bundleName"]];
        }
        curAbilityStatisticsRecordPtr->bundleName_ = srcData["bundleName"];
        std::vector<std::string>::iterator inputedAbilitiesIter =
            find(curAbilityStatisticsRecordPtr->inputedAbilities_.begin(),
                 curAbilityStatisticsRecordPtr->inputedAbilities_.end(), srcData["bundleName"]);
        if (inputedAbilitiesIter == curAbilityStatisticsRecordPtr->inputedAbilities_.end()) {
            curAbilityStatisticsRecordPtr->inputedAbilities_.push_back(srcData["bundleName"]);
        }
        std::vector<std::string> abilities;
        WuKongUtil::GetInstance()->GetAllAbilitiesByBundleName(srcData["bundleName"], abilities);
        DEBUG_LOG_STR("bundleName{%s} all ability size{%d}", srcData["bundleName"].c_str(), abilities.size());
        curAbilityStatisticsRecordPtr->inputedAbilityCount_ = curAbilityStatisticsRecordPtr->inputedAbilities_.size();
        if (abilities.size() == 0) {
            curAbilityStatisticsRecordPtr->abilitiesCount_ = curAbilityStatisticsRecordPtr->inputedAbilities_.size();
        } else {
            curAbilityStatisticsRecordPtr->abilitiesCount_ = abilities.size();
        }

        abilityStatisticsMsg_[srcData["bundleName"]] = curAbilityStatisticsRecordPtr;
    }
}

void StatisticsAbility::UpdateLine(std::shared_ptr<AbilityStatisticsRecord> abilityStatisticsRecordPtr,
                                   std::vector<std::string> &line)
{
    std::stringstream bufferStream;
    std::string bundleName = abilityStatisticsRecordPtr->bundleName_;
    std::string inputedAbilityCount = std::to_string(abilityStatisticsRecordPtr->inputedAbilityCount_);
    std::string abilitiesCount = std::to_string(abilityStatisticsRecordPtr->abilitiesCount_);
    std::string curCoverageStr = "";
    if (abilityStatisticsRecordPtr->abilitiesCount_ > 0) {
        float proportion =
            (abilityStatisticsRecordPtr->inputedAbilityCount_ * PERCENTAGE) / abilityStatisticsRecordPtr->abilitiesCount_;
        bufferStream.str("");
        bufferStream << std::setiosflags(std::ios::fixed) << std::setprecision(DECIMAL_LENGTH) << proportion;
        curCoverageStr = bufferStream.str() + "%";
    }

    DEBUG_LOG_STR("line content bundleName{%s} | inputedAbilityCount{%s} | abilitiesCount{%s} | curCoverageStr{%s}",
                  bundleName.c_str(), inputedAbilityCount.c_str(), abilitiesCount.c_str(), curCoverageStr.c_str());
    line = {bundleName, inputedAbilityCount, abilitiesCount, curCoverageStr};
}

}  // namespace WuKong
}  // namespace OHOS

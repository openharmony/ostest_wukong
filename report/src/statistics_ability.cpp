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
using namespace std;

void StatisticsAbility::StatisticsDetail(vector<map<string, string>> srcDatas,
                                         map<string, shared_ptr<Table>> &destTables)
{
    string app;
    string ability;
    vector<string> inputedAbility;
    vector<string> line;
    vector<string>::iterator appsIter, abilityIter;
    struct abilityRecord abilityRecord;
    stringstream bufferStream;
    for (auto srcDatasIter : srcDatas) {
        app = srcDatasIter["bundleName"];
        ability = srcDatasIter["abilityName"];
        DEBUG_LOG_STR("bundleName{%s} abilityName{%s}", app.c_str(), ability.c_str());
        // check app is insert apps
        appsIter = find(apps_.begin(), apps_.end(), app);
        if (appsIter == apps_.end()) {
            DEBUG_LOG_STR("bundleName{%s} map init", app.c_str());
            apps_.push_back(app);
            vector<string> abilities;
            appMapInputedAbilitys_[app] = abilities;
            WuKongUtil::GetInstance()->GetAllAbilitiesByBundleName(app, abilities);
            DEBUG_LOG_STR("bundleName{%s} all ability size{%d}", app.c_str(), abilities.size());
            appMapAllAbilitys_[app] = abilities;
            bool getStatus = true;
            if (abilities.size() == 0) {
                getStatus = false;
            }
            abilityRecord = {abilities.size(), 0, getStatus};
            appMapAbilityRecord_[app] = abilityRecord;
        }
        inputedAbility = appMapInputedAbilitys_[app];
        DEBUG_LOG_STR("record sumOfAbility{%d} sumOfInputedAbility{%d}", appMapAbilityRecord_[app].sumOfAbility,
                     appMapAbilityRecord_[app].sumOfInputedAbility);
        abilityIter = find(inputedAbility.begin(), inputedAbility.end(), ability);
        if (abilityIter == inputedAbility.end()) {
            inputedAbility.push_back(ability);
            appMapInputedAbilitys_[app] = inputedAbility;
            abilityRecord = appMapAbilityRecord_[app];
            abilityRecord.sumOfInputedAbility++;
            if (abilityRecord.getStatus == false) {
                appMapAllAbilitys_[app] = inputedAbility;
            }
            DEBUG_LOG_STR("push vector current inputedAbility{%d}", abilityRecord.sumOfInputedAbility);

            appMapAbilityRecord_[app] = abilityRecord;
        }
    }
    int inputedAbilityTotal = 0, abilitiesTotal = 0, inputedAbilityCount = 0, abilitiesCount = 0;
    float proportion;
    string proportionStr;
    for (auto appIter : apps_) {
        line.push_back(appIter);
        inputedAbilityCount = appMapInputedAbilitys_[appIter].size();
        inputedAbilityTotal += inputedAbilityCount;
        line.push_back(to_string(inputedAbilityCount));
        abilitiesCount = appMapAllAbilitys_[appIter].size();
        line.push_back(to_string(abilitiesCount));
        abilitiesTotal += abilitiesCount;
        DEBUG_LOG_STR("format table action bundleName{%s} , abilitiesCount{%d}", appIter.c_str(), abilitiesCount);
        if (abilitiesCount <= 0) {
            ERROR_LOG("statistics error");
            return;
        }
        proportion = (inputedAbilityCount * 100.0) / abilitiesCount;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(2) << proportion;
        proportionStr = bufferStream.str() + "%";
        line.push_back(proportionStr);
        record_.push_back(line);
        line.clear();
    }
    if (abilitiesTotal <= 0) {
        ERROR_LOG("statistics error");
        return;
    }
    proportion = (inputedAbilityTotal * 100.0) / abilitiesTotal;
    bufferStream.str("");
    bufferStream << setiosflags(ios::fixed) << setprecision(2) << proportion;
    proportionStr = bufferStream.str() + "%";
    line = {"total", to_string(inputedAbilityTotal), to_string(abilitiesTotal), proportionStr};
    record_.push_back(line);
    shared_ptr<Table> table = make_shared<Table>(headers_, record_);
    record_.clear();
    table->SetName("all");
    table->SetDetail("abilityStatistics");
    destTables["all"] = table;
}
}  // namespace WuKong
}  // namespace OHOS

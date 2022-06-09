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

#include "statistics_componment.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdlib.h>

namespace OHOS {
namespace WuKong {
using namespace std;

void StatisticsComponment::StatisticsDetail(vector<map<string, string>> srcDatas,
                                            map<string, shared_ptr<Table>> &destTables)
{
    if (!SrcDatasPreprocessing(srcDatas)) {
        return;
    }
    string app, componment;
    multimap<string, string> appRecord;
    stringstream bufferStream;
    map<string, multimap<string, string>>::iterator appContainerIter;

    for (auto appsIter : apps_) {
        // check app is record in appContainer
        appContainerIter = appContainer_.find(appsIter);
        if (appContainerIter == appContainer_.end()) {
            break;
        }
        app = appsIter;
        appRecord = appContainer_[app];
        DEBUG_LOG("appRecord have:");
        for (auto appRecordIter : appRecord) {
            DEBUG_LOG_STR("first{%s} second{%s}:", appRecordIter.first.c_str(), appRecordIter.second.c_str());
        }
        map<string, vector<string>> componmentItems;
        // check whether the current type has established a statistical relationship
        if (tablesItems_.count(app) != 0) {
            componmentItems = tablesItems_[app];
        }

        DEBUG_LOG_STR("app{%s} have:", appsIter.c_str());
        for (auto componmentsIter : componmentItems) {
            DEBUG_LOG_STR("componment{%s} execTimes{%s},proportion{%s}:", componmentsIter.first.c_str(),
                          (componmentsIter.second.at(0)).c_str(), (componmentsIter.second.at(1)).c_str());
        }
        uint32_t countExecTimes = appRecord.size();
        uint32_t countInputedTimes = 0, countExpectInputTimes = 0;
        vector<string> line;
        map<string, coverageDetail>::iterator allStatisticIter;

        // record eventType,execTimes,proportion,inputedTimes,ExpectInputTimes,coverage
        for (auto componmentsIter : componments_) {
            DEBUG_LOG_STR("current componment{%s}", componmentsIter.c_str());
            std::map<std::string, coverageDetail> componmentsOfApp;
            coverageDetail coverageDetail = {0, 0, 0};
            allStatisticIter = allStatistic_.find(componmentsIter);
            if (allStatisticIter != allStatistic_.end()) {
                coverageDetail = allStatistic_[componmentsIter];
                DEBUG_LOG_STR("use prev coverageDetail inputedTimes{%d} componmentTotals{%d}",
                              coverageDetail.inputedTimes, coverageDetail.componmentTotals);
            }
            componmentsOfApp = coverages_[app];
            if (componmentsOfApp.count(componmentsIter) == 0) {
                DEBUG_LOG_STR("componments{%s} not match", componmentsIter.c_str());
                continue;
            }
            line.push_back(componmentsIter);
            uint32_t execTimes = 0;
            if (componmentItems.count(componmentsIter) != 0) {
                line = componmentItems[componmentsIter];
                execTimes += atoi(line.at(0).c_str());
            }
            execTimes += appRecord.count(componmentsIter);
            coverageDetail.times += execTimes;
            line.push_back(to_string(execTimes));
            if (countExecTimes <= 0) {
                ERROR_LOG("statistics error of countExecTimes");
                return;
            }
            float proportion = (execTimes * 100.0) / countExecTimes;
            bufferStream.str("");
            bufferStream << setiosflags(ios::fixed) << setprecision(2) << proportion;
            string proportionStr = bufferStream.str() + "%";
            line.push_back(proportionStr);
            std::map<std::string, std::vector<std::string>> tmpInput;
            uint32_t inputedTimes = coverages_[app][componmentsIter].inputedTimes;
            countInputedTimes += inputedTimes;
            coverageDetail.inputedTimes += countInputedTimes;
            line.push_back(to_string(inputedTimes));
            uint32_t expectInputTimes = coverages_[app][componmentsIter].componmentTotals;
            countExpectInputTimes += expectInputTimes;
            coverageDetail.componmentTotals += countExpectInputTimes;
            line.push_back(to_string(expectInputTimes));
            if (expectInputTimes <= 0) {
                ERROR_LOG("statistics error of expectInputTimes");
                return;
            }
            float coverage = (inputedTimes * 100.0) / expectInputTimes;
            bufferStream.str("");
            bufferStream << setiosflags(ios::fixed) << setprecision(2) << coverage;
            string coverageStr = bufferStream.str() + "%";
            line.push_back(coverageStr);
            record_.push_back(line);
            line.clear();

            allStatistic_[componmentsIter] = coverageDetail;
        }
        if (countExpectInputTimes <= 0) {
            ERROR_LOG("statistics error of countExpectInputTimes");
            return;
        }
        float totalCoverage = (countInputedTimes * 100.0) / countExpectInputTimes;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(2) << totalCoverage;
        string totalCoverageStr = bufferStream.str() + "%";
        line = {"total",
                to_string(countExecTimes),
                "100%",
                to_string(countInputedTimes),
                to_string(countExpectInputTimes),
                totalCoverageStr};
        record_.push_back(line);
        shared_ptr<Table> table = make_shared<Table>(headers_, record_);
        record_.clear();
        line.clear();
        table->SetName(app);
        table->SetDetail("componmentStatistics");
        destTables[app] = table;
    }
    // appContainer_.clear();
    vector<string> globalLine;
    uint32_t globalStatisticsCountTimes = 0, globalStatisticsCountInputedTimes = 0,
                 globalStatisticsCountComponmentTotals = 0;
    for (auto allStatisticIter : allStatistic_) {
        globalLine.push_back(allStatisticIter.first);
        coverageDetail componmentCoverageDetail = allStatisticIter.second;
        globalLine.push_back(to_string(componmentCoverageDetail.times));
        if (execCount_ == 0) {
            return;
        }
        float globalStatisticsProportion = (componmentCoverageDetail.times * 100.0) / execCount_;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(2) << globalStatisticsProportion;
        string globalStatisticsProportionStr = bufferStream.str() + "%";
        globalLine.push_back(globalStatisticsProportionStr);
        globalLine.push_back(to_string(componmentCoverageDetail.inputedTimes));
        globalLine.push_back(to_string(componmentCoverageDetail.componmentTotals));
        globalStatisticsCountTimes += componmentCoverageDetail.times;
        globalStatisticsCountInputedTimes += componmentCoverageDetail.inputedTimes;
        globalStatisticsCountComponmentTotals += componmentCoverageDetail.componmentTotals;
        float globalStatisticsCoverage =
            (componmentCoverageDetail.inputedTimes * 100.0) / componmentCoverageDetail.componmentTotals;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(2) << globalStatisticsCoverage;
        string globalStatisticsCoverageStr = bufferStream.str() + "%";
        globalLine.push_back(globalStatisticsCoverageStr);
        record_.push_back(globalLine);
        globalLine.clear();
    }
    string globalAllStatisticsCoverageStr = "";
    if (globalStatisticsCountComponmentTotals != 0) {
        float globalAllStatisticsCoverage =
            (globalStatisticsCountInputedTimes * 100.0) / globalStatisticsCountComponmentTotals;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(2) << globalAllStatisticsCoverage;
        globalAllStatisticsCoverageStr = bufferStream.str() + "%";
    }


    globalLine = {"total",
                  to_string(globalStatisticsCountTimes),
                  "",
                  to_string(globalStatisticsCountInputedTimes),
                  to_string(globalStatisticsCountComponmentTotals),
                  globalAllStatisticsCoverageStr};
    record_.push_back(globalLine);
    shared_ptr<Table> table = make_shared<Table>(headers_, record_);
    table->SetName("all");
    table->SetDetail("componmentStatistics");
    destTables["all"] = table;
    record_.clear();
    allStatistic_.clear();
}

bool StatisticsComponment::SrcDatasPreprocessing(std::vector<std::map<std::string, std::string>> srcDatas)
{
    string app, componment;
    vector<string>::iterator appsIter;
    vector<string>::iterator componmentsIter;
    multimap<string, string> appRecord;
    map<string, multimap<string, string>>::iterator appContainerIter;

    for (auto srcDatasIter : srcDatas) {
        app = srcDatasIter["bundleName"];
        DEBUG_LOG_STR("current bundleName{%s}", app.c_str());
        // check app is insert apps
        appsIter = find(apps_.begin(), apps_.end(), app);
        if (appsIter == apps_.end()) {
            apps_.push_back(app);
        }
        // check appContainer is insert app,get app map record
        appContainerIter = appContainer_.find(app);
        if (appContainerIter != appContainer_.end()) {
            DEBUG_LOG_STR("find appRecord from addContainer bundleName{%s}", app.c_str());
            appRecord = appContainerIter->second;
        }
        // check componment
        if (srcDatasIter.count("componment") == 0) {
            return false;
        }
        componment = srcDatasIter["componment"];
        coverageDetail coverageDetail;
        coverageDetail.inputedTimes = atoi(srcDatasIter["inputedTimes"].c_str());
        coverageDetail.componmentTotals = atoi(srcDatasIter["componmentTotals"].c_str());
        coverages_[app][componment] = coverageDetail;
        appRecord.insert({componment, "componment"});

        appContainer_[app] = appRecord;
        componmentsIter = find(componments_.begin(), componments_.end(), componment);
        if (componmentsIter == componments_.end()) {
            DEBUG_LOG_STR("init all statistics componment{%s}", componment.c_str());
            componments_.push_back(componment);
        }
        execCount_++;
        appRecord.clear();
    }

    return true;
}
}  // namespace WuKong
}  // namespace OHOS

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

#include <sstream>

namespace OHOS {
namespace WuKong {
using namespace std;
const int NUMBER_TWO = 2;
const float ONE_HUNDRED_PERCENT = 100;

void StatisticsComponment::StatisticsDetail
                            (vector<map<string, string>> srcDatas,
                             map<string, shared_ptr<Table>> &destTables)
{
    if (!SrcDatasPreprocessing(srcDatas)) {
        return;
    }
    string app, componment;
    multimap<string, string> appRecord;
    stringstream bufferStream;

    for (appsIter_ = apps_.begin(); appsIter_ != apps_.end(); appsIter_++) {
        // check app is record in appContainer
        appContainerIter_ = appContainer_.find(*appsIter_);
        if (appContainerIter_ == appContainer_.end()) {
            break;
        }
        app = (*appsIter_);
        appRecord = appContainer_[app];
        map<string, vector<string>> componmentItems;
        // check whether the current type has established a statistical relationship
        if (tablesItems_.count(app) != 0) {
            componmentItems = tablesItems_[app];
        }
        int countExecTimes = appRecord.size();
        int countInputedTimes = 0, countExpectInputTimes = 0;
        vector<string> line;
        // record eventType,execTimes,proportion,inputedTimes,ExpectInputTimes,coverage
        for (componmentsIter_ = componments_.begin(); componmentsIter_ != componments_.end(); componmentsIter_++) {
            std::map<std::string, std::vector<std::string>> componmentsOfApp;

            componmentsOfApp = coverages_[app];
            if (componmentsOfApp.count(*componmentsIter_) == 0) {
                continue;
            }
            line.push_back(*componmentsIter_);
            int execTimes = 0;
            if (componmentItems.count(*componmentsIter_) != 0) {
                line = componmentItems[*componmentsIter_];
                execTimes += atoi(line.at(0).c_str());
            }
            execTimes += appRecord.count(*componmentsIter_);
            line.push_back(to_string(execTimes));
            if (countExecTimes <= 0 && execTimes <= 0)
            {
                ERROR_LOG("statistics error.");
                return;
            }
                float proportion = (execTimes * ONE_HUNDRED_PERCENT) / countExecTimes;
            bufferStream.str("");
            bufferStream << setiosflags(ios::fixed) << setprecision(NUMBER_TWO) << proportion;
            string proportionStr = bufferStream.str() + "%";
            line.push_back(proportionStr);
            std::map<std::string, std::vector<std::string>> tmpInput;
            int inputedTimes = atoi((coverages_[app][*componmentsIter_]).at(0).c_str());
            countInputedTimes += inputedTimes;
            line.push_back(to_string(inputedTimes));
            int ExpectInputTimes = atoi(coverages_[app][*componmentsIter_].at(1).c_str());
            countExpectInputTimes += ExpectInputTimes;
            line.push_back(to_string(ExpectInputTimes));
            if (ExpectInputTimes != 0) {
                float coverage = (inputedTimes * ONE_HUNDRED_PERCENT) / ExpectInputTimes;
            bufferStream.str("");
            bufferStream << setiosflags(ios::fixed) << setprecision(NUMBER_TWO) << coverage;
            }
            string coverageStr = bufferStream.str() + "%";
            line.push_back(coverageStr);
            record_.push_back(line);
            line.clear();
        }
        float totalCoverage = (countInputedTimes * 100.0) / countExpectInputTimes;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(NUMBER_TWO) << totalCoverage;
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
        table->SetName(app);
        table->SetDetail("componmentStatistics");
        destTables[app] = table;
    }
    appContainer_.clear();
    record_.clear();
}

bool StatisticsComponment::SrcDatasPreprocessing(std::vector<std::map<std::string, std::string>> srcDatas)
{
    string app, componment;
    multimap<string, string> appRecord;
    appContainerIter_ = appContainer_.begin();
    vector<map<string, string>>::iterator srcDatasIter;
    for (srcDatasIter = srcDatas.begin(); srcDatasIter != srcDatas.end(); srcDatasIter++) {
        // check bundle name
        if (srcDatasIter->count("bundleName") == 0) {
            return false;
        }
        app = (*srcDatasIter)["bundleName"];
        // check app is insert apps
        appsIter_ = find(apps_.begin(), apps_.end(), app);
        if (appsIter_ == apps_.end()) {
            apps_.push_back(app);
        }
        // check appContainer is insert app,get app map record
        appContainerIter_ = appContainer_.find(app);
        if (appContainerIter_ != appContainer_.end()) {
            appRecord = appContainerIter_->second;
        }
        // check componment
        if (srcDatasIter->count("componment") == 0) {
            return false;
        }
        componment = (*srcDatasIter)["componment"];
        vector<string> coveratgeDetail;
        coveratgeDetail.push_back((*srcDatasIter)["inputedTimes"]);
        coveratgeDetail.push_back((*srcDatasIter)["componmentTotals"]);
        coverages_[app][componment] = coveratgeDetail;
        appRecord.insert({componment, "componment" });
        appContainer_[app] = appRecord;
        componmentsIter_ = find(componments_.begin(), componments_.end(), componment);
        if (componmentsIter_ == componments_.end()) {
            componments_.push_back(componment);
        }
        execCount_++;
        appRecord.clear();
    }

    return true;
}
}  // namespace WuKong
}  // namespace OHOS
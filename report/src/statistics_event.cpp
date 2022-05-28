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
#include "statistics_event.h"

#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>

namespace OHOS {
namespace WuKong {
using namespace std;
const int NUMBER_TWO = 2;

void StatisticsEvent::StatisticsDetail
                        (vector<map<string, string>> srcDatas,
                        map<string, shared_ptr<Table>> &destTables)
{
    if (!SrcDatasPreprocessing(srcDatas)) {
        return;
    }
    string app, event;
    multimap<string, string> appRecord;
    stringstream bufferStream;
    vector<string> line;
    int countAllExecTimes = 0;
    for (appsIter_ = apps_.begin(); appsIter_ != apps_.end(); appsIter_++) {
        // check app is record in appContainer
        appContainerIter_ = appContainer_.find(*appsIter_);
        if (appContainerIter_ == appContainer_.end()) {
            break;
        }
        app = (*appsIter_);
        appRecord = appContainer_[app];
        map<string, vector<string>> eventItems;
        // check whether the current type has established a statistical relationship
        if (tablesItems_.count(app) != 0) {
            eventItems = tablesItems_[app];
        }
        int countExecTimes = appRecord.size();
        countAllExecTimes += countExecTimes;
        // record eventType,execTimes,proportion
        for (eventsIter_ = events_.begin(); eventsIter_ != events_.end(); eventsIter_++) {
            line.push_back(*eventsIter_);
            int execTimes = 0;
            if (eventItems.count(*eventsIter_) != 0) {
                line = eventItems[*eventsIter_];
                execTimes += atoi(line.at(0).c_str());
            }
            execTimes += appRecord.count(*eventsIter_);
            line.push_back(to_string(execTimes));
            if (countExecTimes != 0) {
                float proportion = (execTimes * 100.0) / countExecTimes;
            bufferStream.str("");
            bufferStream << setiosflags(ios::fixed) << setprecision(NUMBER_TWO) << proportion;
            }
            string proportionStr = bufferStream.str() + "%";
            line.push_back(proportionStr);
            record_.push_back(line);
            line.clear();
        }
        line = {"total", to_string(countExecTimes), "100%"};
        record_.push_back(line);
        shared_ptr<Table> table = make_shared<Table>(headers_, record_);
        record_.clear();
        line.clear();
        table->SetName(app);
        table->SetDetail("eventStatistics");
        destTables[app] = table;
    }
    for (auto iter : allStatistic_) {
        int execTimes = iter.second;
        float proportion = (execTimes * 100.0) / countAllExecTimes;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(NUMBER_TWO) << proportion;
        string proportionStr = bufferStream.str() + "%";
        line.push_back(iter.first);
        line.push_back(to_string(execTimes));
        line.push_back(proportionStr);
        record_.push_back(line);
        line.clear();
    }
    line = {"total", to_string(countAllExecTimes), "100%"};
    record_.push_back(line);
    shared_ptr<Table> table = make_shared<Table>(headers_, record_);
    table->SetName("all");
    table->SetDetail("eventStatistics");
    destTables["all"] = table;
    appContainer_.clear();
    record_.clear();
    allStatistic_.clear();
}

bool StatisticsEvent::SrcDatasPreprocessing(std::vector<std::map<std::string, std::string>> srcDatas)
{
    string app, event;
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
        // check evnet
        if (srcDatasIter->count("event") == 0) {
            return false;
        }
        event = (*srcDatasIter)["event"];
        appRecord.insert( { event, "event" } );
        appContainer_[app] = appRecord;
        eventsIter_ = find(events_.begin(), events_.end(), event);
        if (eventsIter_ == events_.end()) {
            events_.push_back(event);
            allStatistic_[event] = 1;
        } else {
            int total = allStatistic_[event] + 1;
            allStatistic_[event] = total;
        }
        execCount_++;
        appRecord.clear();
    }

    return true;
}
}  // namespace WuKong
}  // namespace OHOS
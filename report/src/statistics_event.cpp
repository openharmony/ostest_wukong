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

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

namespace OHOS {
namespace WuKong {
using namespace std;

void StatisticsElemnt::StatisticsDetail(vector<map<string, string>> srcDatas,
                                        map<string, shared_ptr<Table>> &destTables)
{
    if (!SrcDatasPreprocessing(srcDatas)) {
        return;
    }
    multimap<string, string> appRecord;
    stringstream bufferStream;
    vector<string> line;
    map<string, multimap<string, string>>::iterator appContainerIter;
    int countAllExecTimes = 0;
    for (auto appsIter : apps_) {
        // check app is record in appContainer
        appContainerIter = appContainer_.find(appsIter);
        if (appContainerIter == appContainer_.end()) {
            break;
        }
        appRecord = appContainer_[appsIter];
        DEBUG_LOG("appRecord have:");
        for (auto appRecordIter : appRecord) {
            DEBUG_LOG_STR("first{%s} second{%s}:", appRecordIter.first.c_str(), appRecordIter.second.c_str());
        }

        map<string, vector<string>> eventItems;
        // check whether the current type has established a statistical relationship
        if (tablesItems_.count(appsIter) != 0) {
            eventItems = tablesItems_[appsIter];
        }
        DEBUG_LOG_STR("app{%s} have:", appsIter.c_str());
        for (auto eventsIter : eventItems) {
            DEBUG_LOG_STR("event{%s} execTimes{%s},proportion{%s}:", eventsIter.first.c_str(),(eventsIter.second.at(0)).c_str(),(eventsIter.second.at(1)).c_str());
        }

        int countExecTimes = appRecord.size();
        countAllExecTimes += countExecTimes;
        // record eventType,execTimes,proportion
        for (auto eventsIter : events_) {
            DEBUG_LOG_STR("statistics event{%s}:", eventsIter.c_str());
            line.push_back(eventsIter);
            int execTimes = 0;
            if (eventItems.count(eventsIter) != 0) {
                line = eventItems[eventsIter];
                execTimes += atoi(line.at(0).c_str());
            }
            execTimes += appRecord.count(eventsIter);
            DEBUG_LOG_STR("appRecord eventCount{%d}:", appRecord.count(eventsIter));
            line.push_back(to_string(execTimes));
            if (countExecTimes <= 0) {
                ERROR_LOG("statistics error");
                return;
            }
            float proportion = (execTimes * 100.0) / countExecTimes;
            bufferStream.str("");
            bufferStream << setiosflags(ios::fixed) << setprecision(2) << proportion;
            string proportionStr = bufferStream.str() + "%";
            line.push_back(proportionStr);
            record_.push_back(line);
            line.clear();
        }
        line = {"total", to_string(countExecTimes), "100.00%"};
        record_.push_back(line);
        shared_ptr<Table> table = make_shared<Table>(headers_, record_);
        record_.clear();
        line.clear();
        table->SetName(appsIter);
        table->SetDetail("eventStatistics");
        destTables[appsIter] = table;
    }
    for (auto iter : allStatistic_) {
        int execTimes = iter.second;
        if (execCount_ <= 0) {
            ERROR_LOG("statistics error");
            return;
        }
        float proportion = (execTimes * 100.0) / execCount_;
        bufferStream.str("");
        bufferStream << setiosflags(ios::fixed) << setprecision(2) << proportion;
        string proportionStr = bufferStream.str() + "%";
        line.push_back(iter.first);
        line.push_back(to_string(execTimes));
        line.push_back(proportionStr);
        record_.push_back(line);
        line.clear();
    }
    line = {"total", to_string(execCount_), "100.00%"};
    record_.push_back(line);
    shared_ptr<Table> table = make_shared<Table>(headers_, record_);
    table->SetName("all");
    table->SetDetail("eventStatistics");
    destTables["all"] = table;
    record_.clear();
}

bool StatisticsElemnt::SrcDatasPreprocessing(std::vector<std::map<std::string, std::string>> srcDatas)
{
    string app, event;
    vector<string>::iterator appsIter;
    vector<string>::iterator eventsIter;
    multimap<string, string> appRecord;
    map<string, multimap<string, string>>::iterator appContainerIter;
    for (auto srcDatasIter : srcDatas) {
        app = srcDatasIter["bundleName"];
        DEBUG_LOG_STR("current bundleName{%s}", app.c_str());
        // check app is insert apps
        appsIter = find(apps_.begin(), apps_.end(), app);
        if (appsIter == apps_.end()) {
            DEBUG_LOG_STR("push bundleName{%s}", app.c_str());
            apps_.push_back(app);
        }
        // check appContainer is insert app,get app map record
        appContainerIter = appContainer_.find(app);
        if (appContainerIter != appContainer_.end()) {
            DEBUG_LOG_STR("find appRecord from addContainer bundleName{%s}", app.c_str());
            appRecord = appContainerIter->second;
        }
        // check evnet
        if (srcDatasIter.count("event") == 0) {
            return false;
        }
        event = srcDatasIter["event"];
        appRecord.insert({event, "event"});
        appContainer_[app] = appRecord;
        eventsIter = find(events_.begin(), events_.end(), event);
        if (eventsIter == events_.end()) {
            DEBUG_LOG_STR("init all statistics event{%s}", event.c_str());
            events_.push_back(event);
            allStatistic_[event] = 1;
        } else {
            int total = allStatistic_[event] + 1;
            allStatistic_[event] = total;
            DEBUG_LOG_STR("add all statistics event{%s}", event.c_str());
        }
        DEBUG_LOG_STR("all statistics event{%s} total{%d}", event.c_str(), allStatistic_[event]);
        execCount_++;
        DEBUG_LOG_STR("execCount_{%d}", execCount_);
        appRecord.clear();
    }

    return true;
}

}  // namespace WuKong
}  // namespace OHOS

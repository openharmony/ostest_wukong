/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022.
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

#include "statistics_exception.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
using namespace std;
const uint32_t DECIMAL_LENGTH = 2;
const float PERCENTAGE = 100.0;
}  // namespace
void StatisticsException::StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                                           std::map<std::string, std::shared_ptr<Table>> &destTables)
{
    string crashType;
    stringstream bufferStream;
    std::vector<std::string>::iterator crashTypesIter;
    for (auto srcDatasIter : srcDatas) {
        // check exception name
        if (srcDatasIter.count("exception") == 0) {
            return;
        }
        crashType = srcDatasIter["exception"];
        // check app is insert apps
        crashTypesIter = find(crashTypes_.begin(), crashTypes_.end(), crashType);
        if (crashTypesIter == crashTypes_.end()) {
            DEBUG_LOG_STR("crashType{%s} init", crashType.c_str());
            crashTypes_.push_back(crashType);
            exceptionTypeCount_[crashType] = 1;
        } else {
            exceptionTypeCount_[crashType]++;
        }
        exceptionTotal_++;
    }

    int curExceptionTypeCount;
    float proportion;
    string proportionStr;
    vector<string> line;
    for (auto crashTypesIter : crashTypes_) {
        line.push_back(crashTypesIter);
        curExceptionTypeCount = exceptionTypeCount_[crashTypesIter];
        DEBUG_LOG_STR("curExceptionType{%s} Count{%d}", crashTypesIter.c_str(), curExceptionTypeCount);
        line.push_back(to_string(curExceptionTypeCount));
        if (exceptionTotal_ <= 0) {
            ERROR_LOG("statistics error");
            return;
        }
        proportion = (curExceptionTypeCount * PERCENTAGE) / exceptionTotal_;
        bufferStream.str("");
        bufferStream << std::setiosflags(std::ios::fixed) << std::setprecision(DECIMAL_LENGTH) << proportion;
        proportionStr = bufferStream.str() + "%";
        line.push_back(proportionStr);
        record_.push_back(line);
        line.clear();
    }
    if (exceptionTotal_ != 0) {
        line = {"total", to_string(exceptionTotal_), "100%"};
        record_.push_back(line);
    }
    shared_ptr<Table> table = make_shared<Table>(headers_, record_);
    record_.clear();
    table->SetName("exception");
    table->SetDetail("statistics");
    destTables["exception"] = table;
}
}  // namespace WuKong
}  // namespace OHOS

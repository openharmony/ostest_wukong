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

#include "statistics_exception.h"

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
}  // namespace
void StatisticsException::StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                                           std::map<std::string, std::shared_ptr<Table>> &destTables)
{
    std::stringstream bufferStream;
    for (auto srcDatasIter : srcDatas) {
        std::string crashType;
        // check exception name
        if (srcDatasIter.count("exception") == 0) {
            return;
        }
        crashType = srcDatasIter["exception"];
        // check app is insert apps
        std::vector<std::string>::iterator crashTypesIter = find(crashTypes_.begin(), crashTypes_.end(), crashType);
        if (crashTypesIter == crashTypes_.end()) {
            DEBUG_LOG_STR("crashType{%s} init", crashType.c_str());
            crashTypes_.push_back(crashType);
            exceptionTypeCount_[crashType] = 1;
        } else {
            exceptionTypeCount_[crashType]++;
        }
        exceptionTotal_++;
    }

    std::vector<std::string> line;
    for (auto crashTypesIter : crashTypes_) {
        std::string proportionStr;
        line.push_back(crashTypesIter);
        int curExceptionTypeCount = exceptionTypeCount_[crashTypesIter];
        DEBUG_LOG_STR("curExceptionTypeCount{%d}", curExceptionTypeCount);
        line.push_back(std::to_string(curExceptionTypeCount));
        if (exceptionTotal_ <= 0) {
            ERROR_LOG("statistics error");
            return;
        }
        float proportion = (curExceptionTypeCount * PERCENTAGE) / exceptionTotal_;
        bufferStream.str("");
        bufferStream << std::setiosflags(std::ios::fixed) << std::setprecision(DECIMAL_LENGTH) << proportion;
        proportionStr = bufferStream.str() + "%";
        line.push_back(proportionStr);
        record_.push_back(line);
        line.clear();
    }
    if (exceptionTotal_ != 0) {
        line = {"total", std::to_string(exceptionTotal_), "100%"};
        record_.push_back(line);
    }
    std::shared_ptr<Table> table = std::make_shared<Table>(headers_, record_);
    record_.clear();
    table->SetName("exception");
    table->SetDetail("statistics");
    destTables["exception"] = table;
}
}  // namespace WuKong
}  // namespace OHOS

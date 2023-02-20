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
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdlib.h>

namespace OHOS {
namespace WuKong {
namespace {
const uint32_t DECIMAL_LENGTH = 2;
const float PERCENTAGE = 100.0;
}  // namespace

void StatisticsComponment::StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                                            std::map<std::string, std::shared_ptr<Table>> &destTables)
{
    SrcDatasPretreatment(srcDatas);
    // loop bundle store componment statistics msg
    for (auto bundle : componmentStatisticsMsg_) {
        DEBUG_LOG_STR("start bundlename{%s}", bundle.first.c_str());
        std::shared_ptr<ComponmentStatisticsMsg> curComponmentStatisticsMsgPtr = bundle.second;
        uint32_t curComonmentTypeLength = curComponmentStatisticsMsgPtr->componmentTypes_.size();
        std::vector<std::string> line;
        std::shared_ptr<ComponmentStatisticsRecord> curComponmentStatisticsRecordPtr = nullptr;
        std::shared_ptr<ComponmentStatisticsRecord> curBundleAllStatisticsPtr =
            std::make_shared<ComponmentStatisticsRecord>();
        curBundleAllStatisticsPtr->componmentType_ = "total";
        for (uint32_t i = 0; i < curComonmentTypeLength; i++) {
            curComponmentStatisticsRecordPtr = curComponmentStatisticsMsgPtr->componmentTypeRecord_[i];
            UpdateLine(curComponmentStatisticsRecordPtr, curComponmentStatisticsMsgPtr->componmentTypeTotal_, line);
            record_.push_back(line);
            curBundleAllStatisticsPtr->execTimes_ += curComponmentStatisticsRecordPtr->execTimes_;
            curBundleAllStatisticsPtr->inputedTimes_ += curComponmentStatisticsRecordPtr->inputedTimes_;
            curBundleAllStatisticsPtr->expectInputTimes_ += curComponmentStatisticsRecordPtr->expectInputTimes_;
        }

        UpdateLine(curBundleAllStatisticsPtr, curComponmentStatisticsMsgPtr->componmentTypeTotal_, line);
        record_.push_back(line);
        std::shared_ptr<Table> table = std::make_shared<Table>(headers_, record_);
        table->SetName(bundle.first);
        table->SetDetail("componment");
        destTables[bundle.first] = table;
        record_.clear();
    }

    GlobalComponmentTypeStatistics();
    std::shared_ptr<Table> globalTable = std::make_shared<Table>(headers_, record_);
    globalTable->SetName("all");
    globalTable->SetDetail("componment");
    destTables["all"] = globalTable;
    record_.clear();
}

void StatisticsComponment::SrcDatasPretreatment(std::vector<std::map<std::string, std::string>> srcDatas)
{
    for (auto srcData : srcDatas) {
        DEBUG_LOG_STR("bundlename{%s} | componment{%s} | inputedTimes{%s} | componmentTotals{%s}",
                      srcData["bundleName"].c_str(), srcData["componment"].c_str(), srcData["inputedTimes"].c_str(),
                      srcData["componmentTotals"].c_str());
        std::vector<std::string>::iterator globalComponmentTypesIter =
            find(globalComponmentTypes_.begin(), globalComponmentTypes_.end(), srcData["componment"]);
        if (globalComponmentTypesIter == globalComponmentTypes_.end()) {
            DEBUG_LOG_STR("push componment{%s} to globalComponmentTypes_", srcData["componment"].c_str());
            globalComponmentTypes_.push_back(srcData["componment"]);
        }

        // check whether bundle is entered resolve create or reuse already exist  StatisticsMsgPtr
        std::shared_ptr<ComponmentStatisticsMsg> curStatisticsMsgPtr = std::make_shared<ComponmentStatisticsMsg>();
        std::map<std::string, std::shared_ptr<ComponmentStatisticsMsg>>::iterator componmentStatisticsMsgIter =
            componmentStatisticsMsg_.find(srcData["bundleName"]);
        if (componmentStatisticsMsgIter != componmentStatisticsMsg_.end()) {
            DEBUG_LOG_STR("use inited curStatisticsMsgPtr by bundleName{%s}", srcData["bundleName"].c_str());
            curStatisticsMsgPtr = componmentStatisticsMsg_[srcData["bundleName"]];
        }
        // check whether componmentType is entered resolve create or reuse already exist ComponmentStatisticsRecordPtr
        std::shared_ptr<ComponmentStatisticsRecord> curComponmentStatisticsRecordPtr =
            std::make_shared<ComponmentStatisticsRecord>();
        uint32_t index = curStatisticsMsgPtr->ComponmentTypesIndex(srcData["componment"]);
        uint32_t curComponmentTypeTotal = curStatisticsMsgPtr->componmentTypeTotal_;
        if (index != curStatisticsMsgPtr->componmentTypes_.size()) {
            curComponmentStatisticsRecordPtr = curStatisticsMsgPtr->componmentTypeRecord_[index];
            DEBUG_LOG_STR("use inited curComponmentStatisticsRecordPtr in index{%d} | componment{%s}", index,
                          srcData["componment"].c_str());
        }
        // update record msg
        curComponmentStatisticsRecordPtr->componmentType_ = srcData["componment"];
        curComponmentStatisticsRecordPtr->execTimes_++;
        curComponmentStatisticsRecordPtr->inputedTimes_ = (unsigned int)atoi(srcData["inputedTimes"].c_str());
        curComponmentStatisticsRecordPtr->expectInputTimes_ = (unsigned int)atoi(srcData["componmentTotals"].c_str());

        if (curStatisticsMsgPtr->componmentTypeRecord_.size() > index) {
            curStatisticsMsgPtr->componmentTypeRecord_[index] = curComponmentStatisticsRecordPtr;
            curStatisticsMsgPtr->componmentTypes_[index] = srcData["componment"];
        } else {
            curStatisticsMsgPtr->componmentTypeRecord_.push_back(curComponmentStatisticsRecordPtr);
            curStatisticsMsgPtr->componmentTypes_.push_back(srcData["componment"]);
        }

        curComponmentTypeTotal++;
        DEBUG_LOG_STR("curComponmentTypeTotal{%d}", curComponmentTypeTotal);
        curStatisticsMsgPtr->componmentTypeTotal_ = curComponmentTypeTotal;
        componmentStatisticsMsg_[srcData["bundleName"]] = curStatisticsMsgPtr;
        execCount_++;
    }
}

void StatisticsComponment::UpdateLine(std::shared_ptr<ComponmentStatisticsRecord> ComponmentStatisticsRecordPtr,
                                      unsigned int componmentTypeTotal, std::vector<std::string> &line)
{
    std::stringstream bufferStream;
    std::string curComponmentType = ComponmentStatisticsRecordPtr->componmentType_;
    std::string curExecTimes = std::to_string(ComponmentStatisticsRecordPtr->execTimes_);
    std::string curProportionStr = "";
    std::string curInputedTimes = std::to_string(ComponmentStatisticsRecordPtr->inputedTimes_);
    std::string curExpectInputTimes = std::to_string(ComponmentStatisticsRecordPtr->expectInputTimes_);
    std::string curCoverageStr = "";
    if (componmentTypeTotal > 0) {
        float proportion = (ComponmentStatisticsRecordPtr->execTimes_ * PERCENTAGE) / componmentTypeTotal;
        bufferStream.str("");
        bufferStream << std::setiosflags(std::ios::fixed) << std::setprecision(DECIMAL_LENGTH) << proportion;
        curProportionStr = bufferStream.str() + "%";
    }
    if (ComponmentStatisticsRecordPtr->expectInputTimes_ > 0) {
        float coverage = (ComponmentStatisticsRecordPtr->inputedTimes_ * PERCENTAGE) /
                         ComponmentStatisticsRecordPtr->expectInputTimes_;
        bufferStream.str("");
        bufferStream << std::setiosflags(std::ios::fixed) << std::setprecision(DECIMAL_LENGTH) << coverage;
        curCoverageStr = bufferStream.str() + "%";
    }
    DEBUG_LOG_STR(
        "line content curComponmentType{%s} | curExecTimes{%s} | curProportionStr{%s} | curInputedTimes{%s} | "
        "curExpectInputTimes{%s} | curCoverageStr{%s}",
        curComponmentType.c_str(), curExecTimes.c_str(), curProportionStr.c_str(), curInputedTimes.c_str(),
        curExpectInputTimes.c_str(), curCoverageStr.c_str());
    line = {curComponmentType, curExecTimes, curProportionStr, curInputedTimes, curExpectInputTimes, curCoverageStr};
}

void StatisticsComponment::GlobalComponmentTypeStatistics()
{
    std::vector<std::string> line;
    std::shared_ptr<ComponmentStatisticsRecord> globalAllStatisticsPtr = std::make_shared<ComponmentStatisticsRecord>();
    globalAllStatisticsPtr->componmentType_ = "total";
    for (auto comonmentType : globalComponmentTypes_) {
        std::shared_ptr<ComponmentStatisticsRecord> componmentTypeRecordPtr =
            std::make_shared<ComponmentStatisticsRecord>();
        componmentTypeRecordPtr->componmentType_ = comonmentType;
        for (auto bundle : componmentStatisticsMsg_) {
            std::shared_ptr<ComponmentStatisticsMsg> curComponmentStatisticsMsgPtr = bundle.second;
            uint32_t index = curComponmentStatisticsMsgPtr->ComponmentTypesIndex(comonmentType);
            if (curComponmentStatisticsMsgPtr->componmentTypeRecord_.size() > index) {
                std::shared_ptr<ComponmentStatisticsRecord> curComponmentStatisticsRecordPtr =
                    curComponmentStatisticsMsgPtr->componmentTypeRecord_[index];
                componmentTypeRecordPtr->execTimes_ += curComponmentStatisticsRecordPtr->execTimes_;
                componmentTypeRecordPtr->inputedTimes_ += curComponmentStatisticsRecordPtr->inputedTimes_;
                componmentTypeRecordPtr->expectInputTimes_ += curComponmentStatisticsRecordPtr->expectInputTimes_;
            }
        }
        globalAllStatisticsPtr->execTimes_ += componmentTypeRecordPtr->execTimes_;
        globalAllStatisticsPtr->inputedTimes_ += componmentTypeRecordPtr->inputedTimes_;
        globalAllStatisticsPtr->expectInputTimes_ += componmentTypeRecordPtr->expectInputTimes_;
        UpdateLine(componmentTypeRecordPtr, execCount_, line);
        record_.push_back(line);
    }
    UpdateLine(globalAllStatisticsPtr, execCount_, line);
    record_.push_back(line);
}
}  // namespace WuKong
}  // namespace OHOS

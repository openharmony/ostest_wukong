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

#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

namespace OHOS {
namespace WuKong {
namespace {
const uint32_t DECIMAL_LENGTH = 2;
const float PERCENTAGE = 100.0;
}  // namespace

void StatisticsEvent::StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                                       std::map<std::string, std::shared_ptr<Table>> &destTables)
{
    SrcDatasPretreatment(srcDatas);
    // loop bundle store event statistics msg
    for (auto bundle : eventStatisticsMsg_) {
        DEBUG_LOG_STR("start bundlename{%s}", bundle.first.c_str());
        std::shared_ptr<EventStatisticsMsg> curEventStatisticsMsgPtr = bundle.second;
        uint32_t curElementTypeLength = curEventStatisticsMsgPtr->eventTypes_.size();
        std::vector<std::string> line;
        std::shared_ptr<EventStatisticsRecord> curEventStatisticsRecordPtr = nullptr;
        std::shared_ptr<EventStatisticsRecord> curBundleAllStatisticsPtr =
            std::make_shared<EventStatisticsRecord>();
        curBundleAllStatisticsPtr->eventType_ = "total";
        for (uint32_t i = 0; i < curElementTypeLength; i++) {
            curEventStatisticsRecordPtr = curEventStatisticsMsgPtr->eventTypeRecord_[i];
            UpdateLine(curEventStatisticsRecordPtr, curEventStatisticsMsgPtr->eventTypeTotal_, line);
            record_.push_back(line);
            curBundleAllStatisticsPtr->execTimes_ += curEventStatisticsRecordPtr->execTimes_;
        }

        UpdateLine(curBundleAllStatisticsPtr, curEventStatisticsMsgPtr->eventTypeTotal_, line);
        record_.push_back(line);
        std::shared_ptr<Table> table = std::make_shared<Table>(headers_, record_);
        table->SetName(bundle.first);
        table->SetDetail("event");
        destTables[bundle.first] = table;
        record_.clear();
    }

    GlobalElementTypesStatistics();
    std::shared_ptr<Table> globalTable = std::make_shared<Table>(headers_, record_);
    globalTable->SetName("all");
    globalTable->SetDetail("event");
    destTables["all"] = globalTable;
    record_.clear();
}

void StatisticsEvent::SrcDatasPretreatment(std::vector<std::map<std::string, std::string>> srcDatas)
{
    for (auto srcData : srcDatas) {
        DEBUG_LOG_STR("bundlename{%s} | eventType{%s}", srcData["bundleName"].c_str(), srcData["event"].c_str());
        std::vector<std::string>::iterator globalElementTypesIter =
            find(globalElementTypes_.begin(), globalElementTypes_.end(), srcData["event"]);
        if (globalElementTypesIter == globalElementTypes_.end()) {
            DEBUG_LOG_STR("push event{%s} to globalElementTypes_", srcData["event"].c_str());
            globalElementTypes_.push_back(srcData["event"]);
        }

        // check whether bundle is entered resolve create or reuse already exist  StatisticsMsgPtr
        std::shared_ptr<EventStatisticsMsg> curStatisticsMsgPtr = std::make_shared<EventStatisticsMsg>();
        std::map<std::string, std::shared_ptr<EventStatisticsMsg>>::iterator eventStatisticsMsgIter =
            eventStatisticsMsg_.find(srcData["bundleName"]);
        if (eventStatisticsMsgIter != eventStatisticsMsg_.end()) {
            DEBUG_LOG_STR("use inited curStatisticsMsgPtr by bundleName{%s}", srcData["bundleName"].c_str());
            curStatisticsMsgPtr = eventStatisticsMsg_[srcData["bundleName"]];
        }
        // check whether eventType is entered resolve create or reuse already exist EventStatisticsRecordPtr
        std::shared_ptr<EventStatisticsRecord> curEventStatisticsRecordPtr =
            std::make_shared<EventStatisticsRecord>();
        uint32_t index = curStatisticsMsgPtr->ElementTypesIndex(srcData["event"]);
        uint32_t curElementTypeTotal = curStatisticsMsgPtr->eventTypeTotal_;
        if (index != curStatisticsMsgPtr->eventTypes_.size()) {
            curEventStatisticsRecordPtr = curStatisticsMsgPtr->eventTypeRecord_[index];
            DEBUG_LOG_STR("use inited curEventStatisticsRecordPtr in index{%d} | event{%s}", index,
                          srcData["event"].c_str());
        }
        // update record msg
        curEventStatisticsRecordPtr->eventType_ = srcData["event"];
        curEventStatisticsRecordPtr->execTimes_++;

        if (curStatisticsMsgPtr->eventTypeRecord_.size() > index) {
            curStatisticsMsgPtr->eventTypeRecord_[index] = curEventStatisticsRecordPtr;
            curStatisticsMsgPtr->eventTypes_[index] = srcData["event"];
        } else {
            curStatisticsMsgPtr->eventTypeRecord_.push_back(curEventStatisticsRecordPtr);
            curStatisticsMsgPtr->eventTypes_.push_back(srcData["event"]);
        }

        curElementTypeTotal++;
        DEBUG_LOG_STR("curElementTypeTotal{%d}", curElementTypeTotal);
        curStatisticsMsgPtr->eventTypeTotal_ = curElementTypeTotal;
        eventStatisticsMsg_[srcData["bundleName"]] = curStatisticsMsgPtr;
        execCount_++;
    }
}
void StatisticsEvent::UpdateLine(std::shared_ptr<EventStatisticsRecord> EventStatisticsRecordPtr,
                                 uint32_t eventTypeTotal, std::vector<std::string> &line)
{
    std::stringstream bufferStream;
    std::string curElementType = EventStatisticsRecordPtr->eventType_;
    std::string curExecTimes = std::to_string(EventStatisticsRecordPtr->execTimes_);
    std::string curProportionStr = "";
    if (eventTypeTotal > 0) {
        float proportion = (EventStatisticsRecordPtr->execTimes_ * PERCENTAGE) / eventTypeTotal;
        bufferStream.str("");
        bufferStream << std::setiosflags(std::ios::fixed) << std::setprecision(DECIMAL_LENGTH) << proportion;
        curProportionStr = bufferStream.str() + "%";
    }

    DEBUG_LOG_STR("line content curElementType{%s} | curExecTimes{%s}", curElementType.c_str(), curExecTimes.c_str());
    line = {curElementType, curExecTimes, curProportionStr};
}

void StatisticsEvent::GlobalElementTypesStatistics()
{
    std::vector<std::string> line;
    std::shared_ptr<EventStatisticsRecord> globalAllStatisticsPtr = std::make_shared<EventStatisticsRecord>();
    globalAllStatisticsPtr->eventType_ = "total";
    for (auto eventType : globalElementTypes_) {
        std::shared_ptr<EventStatisticsRecord> eventStatisticsRecordPtr = std::make_shared<EventStatisticsRecord>();
        eventStatisticsRecordPtr->eventType_ = eventType;
        for (auto bundle : eventStatisticsMsg_) {
            std::shared_ptr<EventStatisticsMsg> curEventStatisticsMsgPtr = bundle.second;
            uint32_t index = curEventStatisticsMsgPtr->ElementTypesIndex(eventType);
            if (curEventStatisticsMsgPtr->eventTypeRecord_.size() > index) {
                std::shared_ptr<EventStatisticsRecord> curEventStatisticsRecordPtr =
                    curEventStatisticsMsgPtr->eventTypeRecord_[index];
                eventStatisticsRecordPtr->execTimes_ += curEventStatisticsRecordPtr->execTimes_;
            }
        }
        globalAllStatisticsPtr->execTimes_ += eventStatisticsRecordPtr->execTimes_;
        UpdateLine(eventStatisticsRecordPtr, execCount_, line);
        record_.push_back(line);
    }
    UpdateLine(globalAllStatisticsPtr, execCount_, line);
    record_.push_back(line);
}
}  // namespace WuKong
}  // namespace OHOS

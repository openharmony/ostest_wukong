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

#ifndef TEST_WUKONG_STATISTICS_EVENT_H
#define TEST_WUKONG_STATISTICS_EVENT_H

#include <iomanip>
#include <string>

#include "data_set.h"
#include "statistics.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class EventStatisticsRecord {
public:
    std::string eventType_ = "";
    uint32_t execTimes_ = 0;
};

class EventStatisticsMsg {
public:
    /*
     * @brief find eventType position in eventTypes_
     * @param eventType
     * @return index
     */
    uint32_t ElementTypesIndex(const std::string &eventType)
    {
        uint32_t index = eventTypes_.size();
        DEBUG_LOG_STR("eventTypes_.size{%d}", index);
        std::vector<std::string>::iterator eventTypesIter;
        eventTypesIter = find(eventTypes_.begin(), eventTypes_.end(), eventType);
        if (eventTypesIter != eventTypes_.end()) {
            index = (uint32_t)(eventTypesIter - eventTypes_.begin());
            DEBUG_LOG_STR("find index{%d}", index);
        }
        DEBUG_LOG_STR("find index{%d}", index);
        return index;
    }
    std::vector<std::string> eventTypes_;
    std::vector<std::shared_ptr<EventStatisticsRecord>> eventTypeRecord_;
    uint32_t eventTypeTotal_ = 0;
};

class StatisticsEvent : public Statistics {
public:
    StatisticsEvent() = default;
    ~StatisticsEvent() = default;
    void StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                          std::map<std::string, std::shared_ptr<Table>> &destTables);

private:
    /*
     * @brief statistics msg update to line
     * @param EventStatisticsRecordPtr store statistics msg
     * @param eventTypeTotal Proportion to calculate the total
     * @param line output
     * @return void
     */
    void UpdateLine(std::shared_ptr<EventStatisticsRecord> eventStatisticsRecordPtr, uint32_t eventTypeTotal,
                    std::vector<std::string> &line);
    /*
     * @brief Realize secondary classification and update statistics of source data through bundleName and event
     * methods
     * @param srcDatas filtered data
     * @return void
     */
    void SrcDatasPretreatment(std::vector<std::map<std::string, std::string>> srcDatas);
    /*
     * @brief Global Statistics for ElementTypesmethods
     * @return void
     */
    void GlobalElementTypesStatistics();

    // bundle map EventStatisticsMsgPtr
    std::map<std::string, std::shared_ptr<EventStatisticsMsg>> eventStatisticsMsg_;
    // all eventTypes record for global statistics used
    std::vector<std::string> globalElementTypes_;

    std::vector<std::string> headers_ = {"type", "execTimes", "proportion"};
    std::vector<std::vector<std::string>> record_;
    int execCount_ = 0;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

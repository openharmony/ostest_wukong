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

#ifndef TEST_WUKONG_STATISTICS_COMPONMENT_H
#define TEST_WUKONG_STATISTICS_COMPONMENT_H

#include <string>

#include "statistics.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {

class ComponmentStatisticsRecord {
public:
    std::string componmentType_ = "";
    uint32_t execTimes_ = 0;
    uint32_t inputedTimes_ = 0;
    uint32_t expectInputTimes_ = 0;
};

class ComponmentStatisticsMsg {
public:
    /*
     * @brief find componmentType position in componmentTypes_
     * @param componmentType
     * @return index
     */
    uint32_t ComponmentTypesIndex(const std::string &componmentType)
    {
        uint32_t index = componmentTypes_.size();
        DEBUG_LOG_STR("componmentTypes_.size{%d}", index);
        uint32_t findIndex = WuKongUtil::GetInstance()->FindElement(componmentTypes_, componmentType);
        if (findIndex != INVALIDVALUE) {
            DEBUG_LOG_STR("findIndex{%d}", findIndex);
            index = findIndex;
        }
        DEBUG_LOG_STR("current componmentType find index{%d}", index);
        return index;
    }
    std::vector<std::string> componmentTypes_;
    std::vector<std::shared_ptr<ComponmentStatisticsRecord>> componmentTypeRecord_;
    uint32_t componmentTypeTotal_ = 0;
};
class StatisticsComponment : public Statistics {
public:
    StatisticsComponment() = default;
    ~StatisticsComponment() = default;
    void StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                          std::map<std::string, std::shared_ptr<Table>> &destTables);

private:
    /*
     * @brief statistics msg update to line
     * @param ComponmentStatisticsRecordPtr store statistics msg
     * @param componmentTypeTotal Proportion to calculate the total
     * @param line output
     * @return void
     */
    void UpdateLine(std::shared_ptr<ComponmentStatisticsRecord> ComponmentStatisticsRecordPtr, uint32_t componmentTypeTotal,
                    std::vector<std::string> &line);
    /*
     * @brief Realize secondary classification and update statistics of source data through bundleName and componment
     * methods
     * @param srcDatas filtered data
     * @return void
     */
    void SrcDatasPretreatment(std::vector<std::map<std::string, std::string>> srcDatas);
    /*
     * @brief Global Statistics for ComponmentTypesmethods
     * @return void
     */
    void GlobalComponmentTypeStatistics();

    // bundle map ComponmentStatisticsMsgPtr
    std::map<std::string, std::shared_ptr<ComponmentStatisticsMsg>> componmentStatisticsMsg_;
    // all componmentType record for global statistics used
    std::vector<std::string> globalComponmentTypes_;
    std::vector<std::string> headers_ = {"type",         "execTimes",        "proportion",
                                         "inputedTimes", "expectInputTimes", "coverage"};
    std::vector<std::vector<std::string>> record_;
    uint32_t execCount_ = 0;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

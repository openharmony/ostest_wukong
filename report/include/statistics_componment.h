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

#include <iomanip>
#include <string>

#include "data_set.h"
#include "statistics.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
struct coverageDetail {
    uint32_t times;
    uint32_t inputedTimes;
    uint32_t componmentTotals;
};
}  // namespace
class StatisticsComponment : public Statistics {
public:
    StatisticsComponment() = default;
    ~StatisticsComponment() = default;
    void StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                          std::map<std::string, std::shared_ptr<Table>> &destTables);

private:
    /*
     * @brief source data statistics and convert data structure of the table depends
     * @param srcDatas filtered data
     * @return void
     */
    bool SrcDatasPreprocessing(std::vector<std::map<std::string, std::string>> srcDatas);
    // Record the apps that appear in the statistical process
    std::vector<std::string> apps_;
    // Record the componment that appear in the statistical process
    std::vector<std::string> componments_;
    // global componment statistics
    std::map<std::string, coverageDetail> allStatistic_;
    /*
     * Record the componment type corresponding to the app,
     * key is app,value is multimap used to count occurrences of componmentType
     * multimap key is componmentType, value is const string event
     */
    std::map<std::string, std::map<std::string, coverageDetail>> coverages_;
    /*
     * Record the componment type corresponding to the app,
     * key is app,value is multimap used to count occurrences of componmentType
     * multimap key is componmentType, value is const string componment
     */
    std::map<std::string, std::multimap<std::string, std::string>> appContainer_;
    /*
     * data structure of the table depends
     * key is app,value is map used to record execTimes
     * map key is componment, value is `execTimes,proportion,inputedTimes,ExpectInputTimes,coverage` vector
     */
    std::map<std::string, std::map<std::string, std::vector<std::string>>> tablesItems_;
    int execCount_ = 0;
    std::vector<std::string> headers_ = {"type",         "execTimes",        "proportion",
                                         "inputedTimes", "expectInputTimes", "coverage"};
    std::vector<std::vector<std::string>> record_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

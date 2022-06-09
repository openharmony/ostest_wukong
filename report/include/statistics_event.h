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
class StatisticsElemnt : public Statistics {
public:
    StatisticsElemnt() = default;
    ~StatisticsElemnt() = default;
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
    // Record the events that appear in the statistical process
    std::vector<std::string> events_;
    // global event statistics
    std::map<std::string, int> allStatistic_;
    /*
     * Record the event type corresponding to the app,
     * key is app,value is multimap used to count occurrences of eventType
     * multimap key is eventType, value is const string event
     */
    std::map<std::string, std::multimap<std::string, std::string>> appContainer_;

    /*
     * data structure of the table depends
     * key is app,value is map used to record execTimes
     * map key is eventType, value is `execTimes,proportion` vector
     */
    std::map<std::string, std::map<std::string, std::vector<std::string>>> tablesItems_;
    std::vector<std::string> headers_ = {"type", "execTimes", "proportion"};
    std::vector<std::vector<std::string>> record_;
    int execCount_ = 0;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

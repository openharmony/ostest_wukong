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

#include "data_set.h"

namespace OHOS {
namespace WuKong {
using namespace std;
void DataSet::FilterData(map<string, string> data)
{
    filter_->FilterDetail(filterType_, data, dataVector_);
}

void DataSet::SetFilterType(std::string filterType)
{
    filterType_ = filterType;
}
void DataSet::SetFilterStragety(shared_ptr<Filter> filter)
{
    filter_ = filter;
}

void DataSet::StatisticsData()
{
    statistics_->StatisticsDetail(dataVector_, tables_);
}

void DataSet::SetStatisticsStragety(shared_ptr<Statistics> statistics)
{
    statistics_ = statistics;
}

void DataSet::FormatCSVData(std::string name, std::string &content)
{
    std::map<std::string, std::shared_ptr<Table>>::iterator tablesIter = tables_.find(name);
    if (tablesIter == tables_.end()) {
        return;
    }
    std::shared_ptr<Table> table = tables_[name];
    formatCSV_->FormatDetail(table, content);
}

void DataSet::SetFormatCSVStragety(std::shared_ptr<Format> formatCSV)
{
    formatCSV_ = formatCSV;
}
}  // namespace WuKong
}  // namespace OHOS

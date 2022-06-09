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

#ifndef TEST_WUKONG_DATA_SET_H
#define TEST_WUKONG_DATA_SET_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "filter.h"
#include "format.h"
#include "statistics.h"
#include "table.h"
#include "wukong_define.h"
namespace OHOS {
namespace WuKong {
class DataSet {
public:
    DataSet() = default;
    ~DataSet() = default;
    void FilterData(std::map<std::string, std::string> data);
    void SetFilterType(std::string filterType);
    void SetFilterStragety(std::shared_ptr<Filter> filter);
    void StatisticsData();
    void SetStatisticsStragety(std::shared_ptr<Statistics> statistics);
    void FormatData(std::string name, std::string &content);
    void SetFormatStragety(std::shared_ptr<Format> format);
private:
    // filtered data vector
    std::vector<std::map<std::string, std::string>> dataVector_;
    std::shared_ptr<Filter> filter_ = nullptr;
    std::shared_ptr<Format> format_;
    std::shared_ptr<Statistics> statistics_ = nullptr;
    // filtered data vector statistical processing tables
    std::map<std::string, std::shared_ptr<Table>> tables_;
    std::string filterType_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

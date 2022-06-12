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

#include "table.h"

namespace OHOS {
namespace WuKong {

Table::Table(std::vector<std::string> cloumn_header, std::vector<std::vector<std::string>> record)
{
    uint32_t cols = cloumn_header.size(), rows = record.size();
    for (uint32_t col = 0; col < cols; col++) {
        uint32_t max = cloumn_header[col].size();
        for (uint32_t row = 0; row < rows; row++) {
            max = record[row][col].size() > max ? record[row][col].size() : max;
        }
        column_size_.push_back(max);
    }
    column_header_ = cloumn_header;
    record_ = record;
}

void Table::SetName(std::string name)
{
    name_ = name;
}

std::string Table::GetName()
{
    return name_;
}

void Table::SetDetail(std::string detail)
{
    detail_ = detail;
}

std::string Table::GetDetail()
{
    return detail_;
}

std::vector<std::string> Table::GetHeader()
{
    return column_header_;
}

std::vector<std::vector<std::string>> Table::GetRecord()
{
    return record_;
}

std::vector<int> Table::GetColumnSize()
{
    return column_size_;
}
}  // namespace WuKong
}  // namespace OHOS
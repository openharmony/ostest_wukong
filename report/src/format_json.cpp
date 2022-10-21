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

#include "format_json.h"
#include <sstream>
namespace OHOS {
namespace WuKong {
FormatJSON::FormatJSON() : Format()
{
}

FormatJSON::~FormatJSON()
{
}

void FormatJSON::FormatDetail(std::shared_ptr<Table> tablePtr, std::string &target)
{
    std::vector<std::string> header = tablePtr->GetHeader();
    std::vector<std::vector<std::string>> record = tablePtr->GetRecord();
    std::stringstream ss;
    ss << "[" << std::endl;
    for (uint32_t row = 0; row < record.size(); row++) {
        ss << "{" << std::endl;
        for (uint32_t col = 0; col < header.size(); col++) {
            ss  << header[col] << ":\"" << record[row][col] << "\"";
            if (col == (header.size() - 1)) {
                ss << "},";
                break;
            }
            ss << ",";
        }
        ss << std::endl;
    }
    ss << "]," << std::endl;
    target = ss.str();
}
}  // namespace WuKong
}  // namespace OHOS

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
#ifndef TEST_WUKONG_TABLE_H
#define TEST_WUKONG_TABLE_H

#include <string>
#include <vector>

namespace OHOS {
namespace WuKong {
class Table {
public:
    Table() = default;
    ~Table() = default;
    void SetName(std::string name);
    std::string GetName();
    void SetDetail(std::string detail);
    std::string GetDetail();
    Table(std::vector<std::string> cc, std::vector<std::vector<std::string>> rc);
    std::vector<std::string> GetHeader();
    std::vector<std::vector<std::string>> GetRecord();
    std::vector<int> GetColumnSize();

private:
    std::string name_;
    std::string detail_;
    std::vector<std::string> column_header_;
    std::vector<int> column_size_;
    std::vector<std::vector<std::string>> record_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

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

#ifndef TEST_WUKONG_STATISTICS_EXCEPTION_H
#define TEST_WUKONG_STATISTICS_EXCEPTION_H
#include <iomanip>
#include <string>

#include "data_set.h"
#include "statistics.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class StatisticsException : public Statistics {
public:
    StatisticsException() = default;
    ~StatisticsException() = default;
    void StatisticsDetail(std::vector<std::map<std::string, std::string>> srcDatas,
                          std::map<std::string, std::shared_ptr<Table>> &destTables);
private:
    uint32_t exceptionTotal_ = 0;
    std::map<std::string, int> exceptionTypeCount_;
    std::vector<std::string> crashTypes_;
    std::vector<std::string> headers_ = {"type", "times", "proportion"};
    std::vector<std::vector<std::string>> record_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif
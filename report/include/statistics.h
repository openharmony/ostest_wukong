/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022.
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

#ifndef TEST_WUKONG_STATISTICS_H
#define TEST_WUKONG_STATISTICS_H

#include <map>
#include <memory>
#include <string>

#include "table.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class Statistics {
public:
    Statistics();
    virtual ~Statistics();
    /*
     * @brief statistics algorithm implementation
     * @param dataVector filtered data vector
     * @param tables store statistics processing data
     * @return
     */
    virtual void StatisticsDetail(std::vector<std::map<std::string, std::string>> dataVector,
                                  std::map<std::string, std::shared_ptr<Table>> &tables);
};
}  // namespace WuKong
}  // namespace OHOS
#endif

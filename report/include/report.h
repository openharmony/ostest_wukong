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

#ifndef TEST_WUKONG_REPORT
#define TEST_WUKONG_REPORT

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "data_set.h"
#include "singleton.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
class Report final : public DelayedSingleton<Report> {
    DECLARE_DELAYED_SINGLETON(Report)
public:
    void Finish();
    void SetSeed(std::string seed);
    /*
     * @brief  Synchronous inputed information
     * @return void
     */
    void SyncInputInfo();
    /*
     * @brief Write the content of the test process segmented to the storage file
     * @return void
     */
    void SegmentedWrite();

private:
    // csv filename
    std::string reportCsvFileName_ = "";
    std::string seed_ = "";
    int taskCount_ = 0;
    time_t startTime_;
    // multimodal random input data set
    std::shared_ptr<DataSet> eventDataSet_ = std::make_shared<DataSet>();
    // componment input data set
    std::shared_ptr<DataSet> componmentDataSet_ = std::make_shared<DataSet>();
    // app set
    std::vector<std::string> apps_;
    std::vector<std::string>::iterator appsIter_;
};
}  // namespace WuKong
}  // namespace OHOS

#endif
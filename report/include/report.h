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

#ifndef TEST_WUKONG_REPORT_H
#define TEST_WUKONG_REPORT_H

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <mutex>

#include "data_set.h"
#include "input_msg_object.h"
#include "singleton.h"
#include "sysevent_listener.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
namespace {
struct componmentRecord {
    std::map<std::string, uint32_t> componmentTypeCount;
    std::map<uint32_t, std::vector<std::string>> pageIdComponments;
};
}  // namespace
class Report final : public DelayedSingleton<Report> {
    DECLARE_DELAYED_SINGLETON(Report)
    friend class SysEventListener;

public:
    void Finish();
    void SetSeed(std::string seed);
    /*
     * @brief  Synchronous inputted information
     * @return void
     */
    void SyncInputInfo(std::shared_ptr<InputedMsgObject> inputedMsgObject);
    /*
     * @brief Write the content of the test process segmented to the storage csvfile
     * @return void
     */
    void SegmentedWriteCSV();

    /*
     * @brief Write the content of the test process segmented to the storage jsonfile
     * @return void
     */
    void SegmentedWriteJson();

    /*
     * @brief recor screen path to report
     * @return void
     */
    void RecordScreenPath(const std::string &screenPath);

    /*
    * @brief get report exception dir
    * @return void
    */
    std::string GetReportExceptionDir();

    /*
     * @brief find Exception Type by crash file name
     * @param exceptionFilename
     * @return void
     */
    void ExceptionRecord(const std::string &exceptionFilename);
private:
    /*
     * @brief dependent environment init, include create file,dir, setting start time
     * @return void
     */
    void EnvInit();

    /*
     * @brief dataSet init, include  event input, componment input, ability, exception
     * @return void
     */
    void DataSetInit();

    /*
     * @brief When a crash occurs check /data/log/hilog/ dir is exist new hilog file then copy
     * @return void
     */
    void HilogFileRecord();

    /*
     * @brief componment information arrange
     * @param bundle bundle name
     * @param inputCompMsgPtr input componment msg ptr
     * @param data out data
     * @return void
     */
    void ComponmentInfoArrange(const std::string &bundle, std::shared_ptr<ComponmentInputMsg> inputCompMsgPtr,
                               std::map<std::string, std::string> &data);
    // csv filename
    std::string reportCsvFileName_ = "";
    std::string reportJsonFileName_ = "";
    std::string reportExceptionDir_ = "";
    std::string currentTestDir_ = "";
    std::string startRunTime_ = "";
    std::string crashDir_ = "/data/log/faultlog/faultlogger/";
    std::vector<std::string> hilogFiles_;
    std::string seed_ = "";
    int taskCount_ = 0;
    bool isFirstAppSwitch_ = false;
    time_t startTime_ = time(0);
    std::mutex crashMtx_;
    std::string hilogDirs_ = "/data/log/hilog/";
    // multimodal input data set
    std::shared_ptr<DataSet> eventDataSet_ = std::make_shared<DataSet>();
    // componment input data set
    std::shared_ptr<DataSet> componmentDataSet_ = std::make_shared<DataSet>();
    // ability data set
    std::shared_ptr<DataSet> abilityDataSet_ = std::make_shared<DataSet>();
    // exception data set
    std::shared_ptr<DataSet> exceptionDataSet_ = std::make_shared<DataSet>();
    // app set
    std::vector<std::string> bundles_;
    std::map<std::string, componmentRecord> bundleComponmentRecord_;

    // screen store path vector
    std::vector<std::string> screenPaths_;
};
}  // namespace WuKong
}  // namespace OHOS

#endif

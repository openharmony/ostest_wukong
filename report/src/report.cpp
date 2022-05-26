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

#include "report.h"

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include "exception_manager.h"
#include "filter_category.h"
#include "format_csv.h"
#include "input_info.h"
#include "statistics_componment.h"
#include "statistics_event.h"
#include "string_ex.h"
#include "wukong_define.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string DEFAULT_DIR = "/data/local/wukong/report/";
}  // namespace

Report::Report()
{
    DIR* rootDir = nullptr;
    std::string dirStr = "/";
    std::vector<std::string> strs;
    OHOS::SplitStr(DEFAULT_DIR, "/", strs);
    bool dirStatus = true;
    for (auto str : strs) {
        dirStr.append(str);
        dirStr.append("/");
        if ((rootDir = opendir(dirStr.c_str())) == nullptr) {
            int ret = mkdir(dirStr.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
            if (ret != 0) {
                dirStatus = false;
                std::cerr << "failed to create dir: " << DEFAULT_DIR << std::endl;
                break;
            }
        }
    }
    if (dirStatus) {
        reportCsvFileName_.append(DEFAULT_DIR);
        reportCsvFileName_ += "wukong_report_" + WuKongUtil::GetInstance()->GetStartRunTime() + ".csv";
        INFO_LOG_STR("Report CSV: (%s)", reportCsvFileName_.c_str());
    }

    time_t currentTime = time(0);
    if (currentTime > 0) {
        startTime_ = currentTime;
    }
    ExceptionManager::GetInstance()->StartCatching();
    // set event filter,statistics,format
    std::shared_ptr<Filter> inputFilter = std::make_shared<FilterCategory>();
    eventDataSet_->SetFilterStragety(inputFilter);
    eventDataSet_->SetFilterType("event");
    std::shared_ptr<Statistics> eventSatistics = std::make_shared<StatisticsEvent>();
    eventDataSet_->SetStatisticsStragety(eventSatistics);
    std::shared_ptr<FormatCSV> formatCSV = std::make_shared<FormatCSV>();

    eventDataSet_->SetFormatCSVStragety(formatCSV);
    // set componment filter,statistics,format
    componmentDataSet_->SetFilterStragety(inputFilter);
    componmentDataSet_->SetFilterType("componment");
    std::shared_ptr<Statistics> componmentSatistics = std::make_shared<StatisticsComponment>();
    componmentDataSet_->SetStatisticsStragety(componmentSatistics);
    componmentDataSet_->SetFormatCSVStragety(formatCSV);
}

void Report::SyncInputInfo()
{
    std::shared_ptr<InputInfo> inputInfo = InputInfo::GetInstance();
    std::map<std::string, std::string> data;
    data["bundleName"] = inputInfo->GetBundleName();
    // record app used to control data display
    appsIter_ = std::find(apps_.begin(), apps_.end(), data["bundleName"]);
    if (appsIter_ == apps_.end()) {
        apps_.push_back(data["bundleName"]);
    }
    // inputInfo conversion `k => v`
    data["abilityName"] = inputInfo->GetAbilityName();
    data["event"] = inputInfo->InputTypeToString();
    data["componment"] = inputInfo->GetComponmentName();
    data["inputedTimes"] = inputInfo->GetComponmentInputedTimes();
    data["componmentTotals"] = inputInfo->GetComponmentTotals();
    // send `k => v` to filter
    eventDataSet_->FilterData(data);
    componmentDataSet_->FilterData(data);
    taskCount_++;
    // statistics and storage every 100 data
    if ((taskCount_ % 100) == 0) {
        SegmentedWrite();
    }
}

Report::~Report()
{
}

void Report::SegmentedWrite()
{
    // csv report format
    if (reportCsvFileName_.empty()) {
        return;
    }
    std::stringstream modules;
    modules << "moudule    , Base Info" << std::endl;
    modules << "task status, success" << std::endl;
    modules << "task time  , " << time(0) - startTime_ << std::endl;
    if (!seed_.empty()) {
        modules << "seed , " << seed_ << std::endl;
    }
    modules << "task count , " << taskCount_ << std::endl;
    eventDataSet_->StatisticsData();
    componmentDataSet_->StatisticsData();
    std::string moduleInput;
    modules << "module, Input Message Statistics" << std::endl;
    modules << "name, all";
    // show all app and detail
    for (appsIter_ = apps_.begin(); appsIter_ != apps_.end(); appsIter_++) {
        modules << ", " << *appsIter_;
    }
    modules << std::endl;
    eventDataSet_->FormatCSVData("all", moduleInput);
    modules << "detail, event, componment" << std::endl;
    // loop app show name-type statistics content
    for (appsIter_ = apps_.begin(); appsIter_ != apps_.end(); appsIter_++) {
        eventDataSet_->FormatCSVData(*appsIter_, moduleInput);
        componmentDataSet_->FormatCSVData(*appsIter_, moduleInput);
    }

    modules << moduleInput;
    std::string csvContent = modules.str();
    std::fstream csvFileStream(reportCsvFileName_, std::ios::out | std::ios::trunc);
    csvFileStream << csvContent << std::endl;
    csvFileStream.close();
}

void Report::Finish()
{
    SegmentedWrite();
    ExceptionManager::GetInstance()->StopCatching();
}

void Report::SetSeed(std::string seed)
{
    seed_ = seed;
}
}  // namespace WuKong
}  // namespace OHOS
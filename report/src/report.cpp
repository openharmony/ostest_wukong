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
#include <unistd.h>

#include "ability_manager_client.h"
#include "element_name.h"
#include "exception_manager.h"
#include "filter_category.h"
#include "format_csv.h"
#include "format_json.h"
#include "statistics_ability.h"
#include "statistics_componment.h"
#include "statistics_event.h"
#include "statistics_exception.h"
#include "string_ex.h"
#include "wukong_define.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
const uint32_t SEGMENT_STATISTICS_LENGTH = 10;
}  // namespace
using namespace OHOS::AAFwk;
Report::Report()
{
    EnvInit();
    DataSetInit();
}

void Report::EnvInit()
{
    // setting filename
    currentTestDir_ = WuKongUtil::GetInstance()->GetCurrentTestDir();
    INFO_LOG_STR("Report currentTestDir: (%s)", currentTestDir_.c_str());
    // setting filename
    reportCsvFileName_ = currentTestDir_ + "wukong_report.csv";
    reportJsonFileName_ = currentTestDir_ + "data.js";

    INFO_LOG_STR("Report CSV: (%s)", reportCsvFileName_.c_str());
    INFO_LOG_STR("Report JSON: (%s)", reportJsonFileName_.c_str());

    reportExceptionDir_ = currentTestDir_ + "exception/";
    INFO_LOG_STR("Report exception dir: (%s)", reportExceptionDir_.c_str());
    int dirExist = access(reportExceptionDir_.c_str(), F_OK);
    if (dirExist != 0) {
        int dirStatus = mkdir((reportExceptionDir_).c_str(), 0777);
        if (dirStatus == -1) {
            ERROR_LOG("exception dir create fail");
        }
    }

    // clear crash dir file
    CrashFileClear();
    // register crash catcher
    ExceptionManager::GetInstance()->StartCatching();
}

void Report::DataSetInit()
{
    std::shared_ptr<Filter> categoryFilter = std::make_shared<FilterCategory>();
    eventDataSet_->SetFilterStragety(categoryFilter);
    eventDataSet_->SetFilterType("event");
    std::shared_ptr<Statistics> eventSatistics = std::make_shared<StatisticsEvent>();
    eventDataSet_->SetStatisticsStragety(eventSatistics);

    // set componment filter,statistics,format
    componmentDataSet_->SetFilterStragety(categoryFilter);
    componmentDataSet_->SetFilterType("componment");
    std::shared_ptr<Statistics> componmentSatistics = std::make_shared<StatisticsComponment>();
    componmentDataSet_->SetStatisticsStragety(componmentSatistics);

    // set ability filter,statistics,format
    abilityDataSet_->SetFilterStragety(categoryFilter);
    abilityDataSet_->SetFilterType("abilityName");
    std::shared_ptr<Statistics> abilitySatistics = std::make_shared<StatisticsAbility>();
    abilityDataSet_->SetStatisticsStragety(abilitySatistics);

    // set exception filter,statistics,format
    exceptionDataSet_->SetFilterStragety(categoryFilter);
    exceptionDataSet_->SetFilterType("exception");
    std::shared_ptr<Statistics> exceptionSatistics = std::make_shared<StatisticsException>();
    exceptionDataSet_->SetStatisticsStragety(exceptionSatistics);
}

void Report::SyncInputInfo(std::shared_ptr<InputedMsgObject> inputedMsgObject)
{
    TRACK_LOG_STD();
    std::shared_ptr<AbilityManagerClient> abilityManagerClient = AbilityManagerClient::GetInstance();
    OHOS::AppExecFwk::ElementName elementName = abilityManagerClient->GetTopAbility();
    std::map<std::string, std::string> data;
    data["bundleName"] = elementName.GetBundleName();
    data["abilityName"] = elementName.GetAbilityName();
    DEBUG_LOG_STR("bundleName{%s} abilityName{%s} ", data["bundleName"].c_str(), data["abilityName"].c_str());
    inputedMode inputMode = inputedMsgObject->GetInputedMode();
    switch (inputMode) {
        case multimodeInput: {
            auto inputMutlMsgPtr = std::static_pointer_cast<MultimodeInputMsg>(inputedMsgObject);
            data["event"] = inputMutlMsgPtr->GetInputType();
            DEBUG_LOG_STR("eventType{%s}", data["event"].c_str());
            break;
        }

        case componmentInput: {
            auto inputCompMsgPtr = std::static_pointer_cast<ComponmentInputMsg>(inputedMsgObject);
            ComponmentInfoArrange(data["bundleName"], inputCompMsgPtr, data);
            DEBUG_LOG("componmentType map");
            break;
        }
        default:
            break;
    }

    // first appswitch abandon
    std::map<std::string, std::string>::iterator it = data.find("event");
    if (it != data.end() && (data["event"] == "appswitch") && (isFirstAppSwitch_ == false)) {
        DEBUG_LOG("first appswitch abandon");
        isFirstAppSwitch_ = true;
        return;
    }
    // record app used to control data display
    std::vector<std::string>::iterator bundleIter = std::find(bundles_.begin(), bundles_.end(), data["bundleName"]);
    if (bundleIter == bundles_.end()) {
        DEBUG_LOG_STR("push apps item{%s}", data["bundleName"].c_str());
        bundles_.push_back(data["bundleName"]);
    }
    // send `k => v` to filter
    eventDataSet_->FilterData(data);
    componmentDataSet_->FilterData(data);
    abilityDataSet_->FilterData(data);
    taskCount_++;
    DEBUG_LOG_STR("taskCount{%d}", taskCount_);
    // statistics and storage every 10 data
    if ((taskCount_ % SEGMENT_STATISTICS_LENGTH) == 0) {
        SegmentedWriteCSV();
        SegmentedWriteJson();
    }
    TRACK_LOG_END();
}

Report::~Report()
{
}

void Report::SegmentedWriteCSV()
{
    TRACK_LOG_STD();
    // csv report format
    if (reportCsvFileName_.empty()) {
        return;
    }
    std::shared_ptr<FormatCSV> formatCSV = std::make_shared<FormatCSV>();
    eventDataSet_->SetFormatStragety(formatCSV);
    componmentDataSet_->SetFormatStragety(formatCSV);
    abilityDataSet_->SetFormatStragety(formatCSV);
    exceptionDataSet_->SetFormatStragety(formatCSV);
    std::stringstream modules;
    modules << "moudule    , Base Info" << std::endl;
    modules << "task status, success" << std::endl;
    modules << "task time  , " << time(0) - startTime_ << std::endl;
    if (!seed_.empty()) {
        modules << "seed , " << seed_ << std::endl;
    }
    modules << "task count , " << taskCount_ << std::endl;
    DEBUG_LOG("start event statistics");
    eventDataSet_->StatisticsData();
    DEBUG_LOG("end event statistics");
    DEBUG_LOG("start componment statistics");
    componmentDataSet_->StatisticsData();
    DEBUG_LOG("end componment statistics");
    std::string moduleInput;
    modules << "module, Input Message Statistics" << std::endl;
    modules << "name, all";
    // show all app and detail
    for (auto bundleIter : bundles_) {
        modules << ", " << bundleIter;
    }
    modules << std::endl;
    modules << "detail, event, componment" << std::endl;
    eventDataSet_->FormatData("all", moduleInput);
    componmentDataSet_->FormatData("all", moduleInput);
    // loop app show name-type statistics content
    for (auto bundleIter : bundles_) {
        eventDataSet_->FormatData(bundleIter, moduleInput);
        componmentDataSet_->FormatData(bundleIter, moduleInput);
    }
    modules << moduleInput;
    modules << "module, ability Statistics" << std::endl;
    modules << "name, all";
    modules << "detail, ability" << std::endl;
    moduleInput = "";
    abilityDataSet_->StatisticsData();
    abilityDataSet_->FormatData("all", moduleInput);
    modules << moduleInput;

    std::unique_lock<std::mutex> locker(crashMtx_);
    modules << "module, Exception Message Statistics" << std::endl;
    modules << "name, exception" << std::endl;
    modules << "detail, statistics" << std::endl;
    moduleInput = "";
    exceptionDataSet_->StatisticsData();
    exceptionDataSet_->FormatData("exception", moduleInput);
    modules << moduleInput;
    locker.unlock();
    std::string csvContent = modules.str();
    std::fstream csvFileStream(reportCsvFileName_, std::ios::out | std::ios::trunc);
    csvFileStream << csvContent << std::endl;
    csvFileStream.close();
    TRACK_LOG_END();
}

void Report::SegmentedWriteJson()
{
    TRACK_LOG_STD();
    DEBUG_LOG("SegmentedWriteJson start");
    // csv report format
    if (reportCsvFileName_.empty()) {
        return;
    }
    std::shared_ptr<FormatJSON> formatJSON = std::make_shared<FormatJSON>();
    eventDataSet_->SetFormatStragety(formatJSON);
    componmentDataSet_->SetFormatStragety(formatJSON);
    abilityDataSet_->SetFormatStragety(formatJSON);
    exceptionDataSet_->SetFormatStragety(formatJSON);
    std::stringstream modules;
    std::string moduleInput;
    modules << "var reportJson = {" << std::endl;
    modules << "base: [" << std::endl;
    modules << "{ item: \"tast status\", detail: \" success \"}," << std::endl;
    modules << "{ item: \"tast time\", detail: \" " << time(0) - startTime_ << "s\"}," << std::endl;
    modules << "{ item: \"tast cout\", detail: \" " << taskCount_ << "\"}," << std::endl;
    if (!seed_.empty()) {
        modules << "{ item: \"seed\", detail: \" " << seed_ << "\"}," << std::endl;
    }
    modules << "]," << std::endl;
    modules << "detailApps:{" << std::endl;
    modules << "names:[ \"all\"";
    // show all app and detail
    for (auto bundleIter : bundles_) {
        modules << ", \"" << bundleIter << " \"";
    }
    modules << "]," << std::endl;
    modules << "details: [" << std::endl;
    modules << "{" << std::endl;
    modules << "eventStatistics:" << std::endl;
    eventDataSet_->FormatData("all", moduleInput);
    modules << moduleInput;
    modules << "controlStatistics:";
    componmentDataSet_->FormatData("all", moduleInput);
    modules << moduleInput;
    modules << "},";
    // loop app show name-type statistics content
    for (auto bundleIter : bundles_) {
        modules << "{" << std::endl;
        modules << "eventStatistics:";
        eventDataSet_->FormatData(bundleIter, moduleInput);
        modules << moduleInput;
        modules << "controlStatistics:";
        componmentDataSet_->FormatData(bundleIter, moduleInput);
        modules << moduleInput;
        modules << "},";
    }
    modules << "]" << std::endl;
    modules << "}," << std::endl;
    modules << "abilityStatistics:";
    abilityDataSet_->FormatData("all", moduleInput);
    modules << moduleInput;
    modules << "detailException: {" << std::endl;
    modules << "names: [\"exception statistics\", \"cpp crash statistics\", \"js crash statistics\"]," << std::endl;
    modules << "details: [" << std::endl;
    modules << "{" << std::endl;
    modules << "exception_statistics: {" << std::endl;
    modules << "header: [\"Type\", \"Times\", \"Proportion\"]," << std::endl;
    modules << "content: " << std::endl;
    exceptionDataSet_->FormatData("exception", moduleInput);
    modules << moduleInput;
    modules << "}," << std::endl;
    modules << "}," << std::endl;
    modules << "]" << std::endl;
    modules << "}," << std::endl;
    unsigned int index = 0;
    modules << "screens:[" << std::endl;
    for (auto srceen : screenPaths_) {
        modules << "{index:\"" << index << "\","
                << "path:\"" << srceen << "\"}," << std::endl;
        index++;
    }
    modules << "]," << std::endl;
    modules << "};" << std::endl;
    std::string jsonContent = modules.str();
    std::fstream jsonFileStream(reportJsonFileName_, std::ios::out | std::ios::trunc);
    jsonFileStream << jsonContent << std::endl;
    jsonFileStream.close();
    DEBUG_LOG("SegmentedWriteJson end");
    TRACK_LOG_END();
}

void Report::CrashFileRecord()
{
    std::unique_lock<std::mutex> locker(crashMtx_);
    struct dirent *dp;
    DIR *dirpCrash = nullptr;
    DIR *dirpHilog = nullptr;
    std::shared_ptr<WuKongUtil> utilPtr = WuKongUtil::GetInstance();
    dirpCrash = opendir(crashDir_.c_str());
    if (dirpCrash == nullptr) {
        ERROR_LOG_STR("dir{%s} opendir error", crashDir_.c_str());
        return;
    }
    DIR *crashDir = nullptr;
    DIR *reportExceptionDir = nullptr;
    while ((dp = readdir(dirpCrash)) != NULL) {
        std::string targetFile(dp->d_name);
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
            std::vector<std::string>::iterator iterDir = find(crashFiles_.begin(), crashFiles_.end(), targetFile);
            if (iterDir == crashFiles_.end()) {
                if (utilPtr->CheckFileStatus(crashDir_.c_str(), crashDir) &&
                    utilPtr->CheckFileStatus(reportExceptionDir_.c_str(), reportExceptionDir)) {
                    DEBUG_LOG("copy action");
                    std::string destLocation = reportExceptionDir_ + targetFile;
                    std::string srcFilePath = crashDir_ + targetFile;
                    utilPtr->CopyFile(srcFilePath.c_str(), destLocation.c_str());
                    crashFiles_.push_back(std::string(dp->d_name));
                    ExceptionRecord(targetFile);
                } else {
                    ERROR_LOG("failede to get crash file path");
                    (void)closedir(dirpCrash);
                    return;
                }
            }
        }
        if (crashDir == nullptr) {
            TRACK_LOG("crashDir is nullptr");
            (void)closedir(dirpCrash);
            return;
        }
        closedir(crashDir);
        if (reportExceptionDir == nullptr) {
            TRACK_LOG("reportExceptionDir is nullptr");
            (void)closedir(dirpCrash);
            return;
        }
        closedir(reportExceptionDir);
    }
    (void)closedir(dirpCrash);
    dirpHilog = opendir(hilogDirs_.c_str());
    if (dirpHilog == nullptr) {
        ERROR_LOG_STR("dir{%s} opendir error", hilogDirs_.c_str());
        return;
    }
    DIR *hilogptr = nullptr;
    memset_s(reportExceptionDir, sizeof(reportExceptionDir), 0x00, sizeof(reportExceptionDir));
    while ((dp = readdir(dirpHilog)) != NULL) {
        std::string targetFile(dp->d_name);
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
            if (utilPtr->CheckFileStatus(reportExceptionDir_.c_str(), reportExceptionDir) &&
                utilPtr->CheckFileStatus(hilogDirs_.c_str(), hilogptr)) {
                DEBUG_LOG("copy action");
                std::string destLocation = reportExceptionDir_ + targetFile;
                std::string srcFilePath = hilogDirs_ + targetFile;
                utilPtr->CopyFile(srcFilePath.c_str(), destLocation.c_str());
            } else {
                ERROR_LOG("failed to get hilog file path");
                (void)closedir(dirpHilog);
                return;
            }
        }
        if (hilogptr == nullptr) {
            TRACK_LOG("hilogptr is nullptr");
            (void)closedir(dirpHilog);
            return;
        }
        closedir(hilogptr);
        if (reportExceptionDir == nullptr) {
            TRACK_LOG("reportExceptionDir is nullptr");
            (void)closedir(dirpHilog);
            return;
        }
        closedir(reportExceptionDir);
    }
    (void)closedir(dirpHilog);
}

void Report::ExceptionRecord(const std::string &exceptionFilename)
{
    std::map<std::string, std::string> data;
    std::string exceptionType;
    if (exceptionFilename.find("cppcrash") != std::string::npos) {
        exceptionType = "cppcrash";
    }

    if (exceptionFilename.find("appfreeze") != std::string::npos) {
        exceptionType = "appfreeze";
    }

    if (exceptionFilename.find("jscrash") != std::string::npos) {
        exceptionType = "jscrash";
    }

    if (exceptionFilename.find("serviceblock") != std::string::npos) {
        exceptionType = "serviceblock";
    }

    data["exception"] = exceptionType;
    exceptionDataSet_->FilterData(data);
}

void Report::CrashFileClear()
{
    DIR *dirp;
    struct dirent *dp;
    dirp = opendir(crashDir_.c_str());
    while ((dp = readdir(dirp)) != NULL) {
        std::string targetFile(dp->d_name);
        if ((strcmp(dp->d_name, ".") != 0) && (strcmp(dp->d_name, "..") != 0)) {
            std::string srcFilePath = crashDir_ + targetFile;
            int ret = unlink(srcFilePath.c_str());
            if (ret != 0) {
                std::cout << "file clear error" << std::endl;
            }
        }
    }
    (void)closedir(dirp);
}

void Report::Finish()
{
    SegmentedWriteCSV();
    SegmentedWriteJson();
    ExceptionManager::GetInstance()->StopCatching();
}

void Report::SetSeed(std::string seed)
{
    seed_ = seed;
}

void Report::ComponmentInfoArrange(const std::string &bundle, std::shared_ptr<ComponmentInputMsg> inputCompMsgPtr,
                                   std::map<std::string, std::string> &data)
{
    std::map<std::string, componmentRecord>::iterator bundleComponmentRecordIter;
    componmentRecord componmentRecord;
    bundleComponmentRecordIter = bundleComponmentRecord_.find(bundle);
    if (bundleComponmentRecordIter != bundleComponmentRecord_.end()) {
        componmentRecord = bundleComponmentRecordIter->second;
    }
    componmentRecord.pageIdComponments[inputCompMsgPtr->pageId_] = inputCompMsgPtr->pageComponments;
    std::map<std::string, uint32_t>::iterator componmentTypeCountIter;
    uint32_t componmentTypeInputedCount = 0, componmentTypeTotal = 0;
    componmentTypeCountIter = componmentRecord.componmentTypeCount.find(inputCompMsgPtr->componmentType_);
    if (componmentTypeCountIter != componmentRecord.componmentTypeCount.end()) {
        componmentTypeInputedCount = componmentTypeCountIter->second;
    }
    componmentTypeInputedCount++;

    for (auto pageIdComponmentsIter : componmentRecord.pageIdComponments) {
        for (auto componmentVectorIter : pageIdComponmentsIter.second) {
            if (componmentVectorIter.compare(inputCompMsgPtr->componmentType_) == 0) {
                componmentTypeTotal++;
            }
        }
    }
    if (componmentTypeInputedCount > componmentTypeTotal) {
        componmentTypeInputedCount = componmentTypeTotal;
    }

    componmentRecord.componmentTypeCount[inputCompMsgPtr->componmentType_] = componmentTypeInputedCount;
    data["componment"] = inputCompMsgPtr->componmentType_;
    data["inputedTimes"] = std::to_string(componmentTypeInputedCount);
    data["componmentTotals"] = std::to_string(componmentTypeTotal);
    DEBUG_LOG_STR("componmentType{%s} inputedTimes{%s} componmentTotals{%s}", data["componment"].c_str(),
                  data["inputedTimes"].c_str(), data["componmentTotals"].c_str());
    bundleComponmentRecord_[bundle] = componmentRecord;
}

void Report::RecordScreenPath(const std::string &screenPath)
{
    screenPaths_.push_back(screenPath);
}
}  // namespace WuKong
}  // namespace OHOS

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

#include "exception_manager.h"

#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>

#include "csv_utils.h"
#include "hisysevent_manager.h"
#include "sysevent_listener.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
using HiviewDFX::HiSysEventManager;
using HiviewDFX::ListenerRule;
namespace {
std::string DEFAULT_DIR = "/data/local/tmp/wukong";

bool InitReportFolder()
{
    DIR *rootDir = nullptr;
    if ((rootDir = opendir(DEFAULT_DIR.c_str())) == nullptr) {
        int ret = mkdir(DEFAULT_DIR.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
        if (ret != 0) {
            ERROR_LOG_STR("failed to create dir: %s", DEFAULT_DIR.c_str());
            return false;
        }
    } else {
        closedir(rootDir);
    }
    return true;
}

bool InitCsvFile(std::ofstream &csvFile, std::string &filePath)
{
    TRACK_LOG_STD();
    if (!InitReportFolder()) {
        return false;
    }

    filePath = DEFAULT_DIR + "/wukong_" + WuKongUtil::GetInstance()->GetStartRunTime() + ".csv";
    csvFile.open(filePath, std::ios_base::out | std::ios_base::trunc);
    if (!csvFile) {
        ERROR_LOG_STR("Failed to create csv file at: %s", filePath.c_str());
        return false;
    }
    CsvUtils::WriteHeader(csvFile);
    INFO_LOG_STR("CSV: (%s)", filePath.c_str());
    TRACK_LOG_END();
    return true;
}
}  // namespace
bool ExceptionManager::StartCatching()
{
    if (!InitCsvFile(*csvFile, currentCsvFilePath)) {
        return false;
    }
    std::vector<ListenerRule> sysRules;
    std::string domain = "";
    std::string eventName = "";
    sysRules.emplace_back(domain, eventName);
    auto onEnter = [this] {
        std::lock_guard<std::mutex> lk(cbMtx_);
        inFlightCallbacks_.fetch_add(1, std::memory_order_relaxed);
    };
    auto onExit = [this] {
        std::lock_guard<std::mutex> lk(cbMtx_);
        inFlightCallbacks_.fetch_sub(1, std::memory_order_relaxed);
        cbCv_.notify_one();
    };
    toolListener = std::make_shared<SysEventListener>(csvFile, csvMutex, onEnter, onExit);
    if (toolListener == nullptr) {
        ERROR_LOG("toolListener is nullptr and please check the csvFile is ok");
        return false;
    }
    return HiSysEventManager::AddListener(toolListener, sysRules) == 0;
}

void ExceptionManager::StopCatching()
{
    int32_t result = HiSysEventManager::RemoveListener(toolListener);
    DEBUG_LOG_STR("remove listener result: %d", result);
    {
        std::unique_lock<std::mutex> lk(cbMtx_);
        cbCv_.wait(lk, [this] { return inFlightCallbacks_.load(std::memory_order_relaxed) <= 0; });
    }
    std::lock_guard<std::mutex> lock(*csvMutex);
    if (csvFile->is_open()) {
        csvFile->flush();
        csvFile->close();
    }
    toolListener.reset();
    TEST_RUN_LOG("catching stopped");
}
}  // namespace WuKong
}  // namespace OHOS

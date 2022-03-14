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

#include "wukong_exception_manager.h"

#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "hisysevent_manager.h"
#include "wukong_sysevent_listener.h"
#include "wukong_csv_utils.h"

namespace OHOS {
    namespace AppExecFwk {
        using HiviewDFX::ListenerRule;
        using HiviewDFX::HiSysEventManager;
        namespace {
            std::string DEFAULT_DIR = "/data/local/wukong";

            bool InitReportFolder()
            {
                DIR *rootDir = nullptr;
                if ((rootDir = opendir(DEFAULT_DIR.c_str())) == nullptr) {
                    int ret = mkdir(DEFAULT_DIR.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
                    if (ret != 0) {
                        std::cerr << "failed to create dir: " << DEFAULT_DIR << std::endl;
                        return false;
                    }
                }
            return true;
            }

            bool initCsvFile(std::ofstream &csvFile, std::string &filePath)
            {
                if (!InitReportFolder()) {
                    return false;
                }
                time_t currentTime = time(0);
                char buf[32] = {0};
                int copiedSize = strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", localtime(&currentTime));
                if (copiedSize <= 0) {
                    return false;
                }
                filePath = DEFAULT_DIR + "/wukong_" + buf + ".csv";
                csvFile.open(filePath, std::ios_base::out | std::ios_base::trunc);
                if (!csvFile) {
                    std::cerr << "Failed to create csv file at:" << filePath << std::endl;
                    return false;
                }
                WuKongCsvUtils::WriteHeader(csvFile);
                std::cout << "The result will be written in csv file at location: " << filePath << std::endl;
                return true;
            }
        }
        bool WuKongExceptionManager::StartCatching()
        {
            if (!initCsvFile(csvFile, currentCsvFilePath)) {
                return false;
            }
            std::vector<ListenerRule> sysRules;
            std::string domain = "";
            std::string eventName = "";
            sysRules.emplace_back(domain, eventName);
            toolListener = std::make_shared<WuKongSysEventListener>(csvFile);
            return HiSysEventManager::AddEventListener(toolListener, sysRules);
        }

        void WuKongExceptionManager::StopCatching()
        {
            HiSysEventManager::RemoveListener(toolListener);
            if (csvFile.is_open()) {
                csvFile.flush();
                csvFile.close();
            }
            std::cout << "catching stopped" << std::endl;
            std::cout << "The result file location:" << GetCurrentCsvFilePath() << std::endl;
        }
    } // namespace AppExecFwk
} // namespace OHOS

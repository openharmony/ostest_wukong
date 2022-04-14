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


#ifndef WUKONG_WUKONG_CSV_UTILS_H
#define WUKONG_WUKONG_CSV_UTILS_H

#include <fstream>
#include <iostream>

namespace OHOS {
namespace AppExecFwk {
class WuKongCsvUtils {
public:
    struct OneLineData {
        std::string domain;
        std::string name;
        std::string type;
        uint64_t time;
        std::string timeZone;
        uint64_t pid;
        uint64_t tid;
        uint64_t uid;
        uint64_t traceId;
        uint64_t spanId;
        uint64_t pspanId;
        uint64_t traceFlag;
    };
    //all those are the properties of the csvfile.

    /**
     * @brief writeheader of csvFile
     * @param csvFile the file that save the information when exceptions occur. 
     * @return -
     */ 
    static void WriteHeader(std::ofstream &csvFile)
    {
        csvFile << "Domain" << ',';
        csvFile << "EventName" << ',';
        csvFile << "Type" << ',';
        csvFile << "Time" << ',';
        csvFile << "TimeZone" << ',';
        csvFile << "ProcessId" << ',';
        csvFile << "ThreadId" << ',';
        csvFile << "UserId" << ',';
        csvFile << "TraceId" << ',';
        csvFile << "SpanId" << ',';
        csvFile << "PSpanid" << ',';
        csvFile << "TraceFlag" << std::endl;
    }

    /**
     * @brief context of csvFile
     * @param csvFile the file that save the information when exceptions occur. 
     * @param data the data which needs to be writen in the csvfile.
     * @return -
     */ 
    static void WriteOneLine(std::ofstream &csvFile, const OneLineData &data)
    {
        csvFile << data.domain << ',';
        csvFile << data.name << ',';
        csvFile << data.type << ',';
        csvFile << data.time << ',';
        csvFile << data.timeZone << ',';
        csvFile << data.pid << ',';
        csvFile << data.tid << ',';
        csvFile << data.uid << ',';
        csvFile << data.traceId << ',';
        csvFile << data.spanId << ',';
        csvFile << data.pspanId << ',';
        csvFile << data.traceFlag << std::endl;
    }
};
} // namespace AppExecFwk
} // namespace OHOS

#endif // WUKONG_WUKONG_CSV_UTILS_H

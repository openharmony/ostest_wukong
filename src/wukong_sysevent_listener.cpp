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

#include <iostream>
#include <nlohmann/json.hpp>
#include "wukong_csv_utils.h"
#include "hisysevent.h"
#include "wukong_sysevent_listener.h"

namespace OHOS {
    namespace AppExecFwk {
        using nlohmann::json;
        using OHOS::HiviewDFX::HiSysEvent;

        namespace {
            template<typename ValueType>
            void ValueGet(const json& jsonData, std::string&& key, ValueType& data)
            {
                const ValueType* valuePtr = jsonData[key].get<const ValueType *>();
                if (valuePtr != nullptr) {
                    data = *valuePtr;
                }
            }
        }

        void WuKongSysEventListener::OnHandle(const std::string& domain,
                                              const std::string& eventName,
                                              const int eventType,
                                              const std::string& eventDetail)
        {
            std::cout << "----------Exception caught----------" << std::endl;
            std::cout << "domain:" << domain << std::endl;
            std::cout << "eventName:" << eventName << std::endl;
            std::cout << "eventType:" << eventType << std::endl;
            std::cout << "eventDetail:" << eventDetail << std::endl;
            std::cout << "------------------------------------" << std::endl;
            WuKongCsvUtils::OneLineData data {};
            data.domain = domain;
            data.name = eventName;
            switch (eventType) {
                case HiSysEvent::EventType::FAULT:
                    data.type = "FAULT";
                    break;
                case HiSysEvent::EventType::STATISTIC:
                    data.type = "STATISTIC";
                    break;
                case HiSysEvent::EventType::SECURITY:
                    data.type = "SECURITY";
                    break;
                case HiSysEvent::EventType::BEHAVIOR:
                    data.type = "BEHAVIOR";
                    break;
                default:
                    data.type = "UNKNOWN";
            }

            json jsonData = json::parse(eventDetail, nullptr, false);
            if (jsonData == json::value_t::discarded) {
                std::cerr << "event detail parse error, the content:" << eventDetail << std::endl;
            } else {
                ValueGet<uint64_t>(jsonData, "time_", data.time);
                ValueGet<std::string>(jsonData, "tz_", data.timeZone);
                ValueGet<uint64_t>(jsonData, "pid_", data.pid);
                ValueGet<uint64_t>(jsonData, "tid_", data.tid);
                ValueGet<uint64_t>(jsonData, "uid_", data.uid);
                ValueGet<uint64_t>(jsonData, "traceid_", data.traceId);
                ValueGet<uint64_t>(jsonData, "spanid_", data.spanId);
                ValueGet<uint64_t>(jsonData, "pspanid_", data.pspanId);
                ValueGet<uint64_t>(jsonData, "trace_flag_", data.traceFlag);
            }
            WuKongCsvUtils::WriteOneLine(csvFile, data);
        }

        void WuKongSysEventListener::OnServiceDied()
        {
            std::cout << "Listener service Died" << std::endl;
        }
    } // namespace AppExecFwk
} // namespace OHOS
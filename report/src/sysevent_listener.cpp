/*
 * @Description:
 * @Date: 2022-04-21 19:40:58
 * @LastEditTime: 2022-04-21 19:54:19
 * @FilePath: /wukong/report/src/sysevent_listener.cpp
 * @author: lemon
 */
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

#include "sysevent_listener.h"

#include <nlohmann/json.hpp>

#include "csv_utils.h"
#include "hisysevent.h"

namespace OHOS {
namespace WuKong {
using nlohmann::json;
using OHOS::HiviewDFX::HiSysEvent;

namespace {
template <typename ValueType>
void ValueGet(const json& jsonData, std::string&& key, ValueType& data)
{
    const ValueType* valuePtr = jsonData[key].get<const ValueType*>();
    if (valuePtr != nullptr) {
        data = *valuePtr;
    }
}
}  // namespace

void SysEventListener::OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
                                const std::string& eventDetail)
{
    TRACK_LOG("----------Exception caught----------");
    WARN_LOG_STR("domain: %s", domain.c_str());
    TRACK_LOG_STR("eventName: %s", eventName.c_str());
    TRACK_LOG_STR("eventType: %s", std::to_string(eventType).c_str());
    TRACK_LOG_STR("eventDetail: %s", eventDetail.c_str());
    TRACK_LOG("------------------------------------");
    CsvUtils::OneLineData data;
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
        ERROR_LOG_STR("event detail parse error, the content: %s", eventDetail.c_str());
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
    CsvUtils::WriteOneLine(csvFile, data);
}
void SysEventListener::OnServiceDied()
{
    ERROR_LOG("Listener service Died");
}
}  // namespace WuKong
}  // namespace OHOS
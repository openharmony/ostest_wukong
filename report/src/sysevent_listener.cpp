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

#include "csv_utils.h"
#include "hisysevent.h"
#include "report.h"

namespace OHOS {
namespace WuKong {
using OHOS::HiviewDFX::HiSysEvent;

void SysEventListener::OnEvent(std::shared_ptr<HiviewDFX::HiSysEventRecord> sysEvent)
{
    if (sysEvent == nullptr) {
        return;
    }
    std::string domain = sysEvent->GetDomain();
    std::string eventName = sysEvent->GetEventName();
    OHOS::HiviewDFX::HiSysEvent::EventType eventType = sysEvent->GetEventType();
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
    data.time = sysEvent->GetTime();
    data.timeZone = sysEvent->GetTimeZone();
    data.pid = static_cast<uint64_t>(sysEvent->GetPid());
    data.tid = static_cast<uint64_t>(sysEvent->GetTid());
    data.uid = static_cast<uint64_t>(sysEvent->GetUid());
    CsvUtils::WriteOneLine(csvFile, data);
}
void SysEventListener::OnServiceDied()
{
    ERROR_LOG("Listener service Died");
}
}  // namespace WuKong
}  // namespace OHOS

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

#ifndef WUKONG_WUKONG_SYSEVENT_LISTENER_H
#define WUKONG_WUKONG_SYSEVENT_LISTENER_H

#include "hisysevent_subscribe_callback.h"

namespace OHOS {
    namespace AppExecFwk {
        class WuKongSysEventListener : public OHOS::HiviewDFX::HiSysEventSubscribeCallBack {
        public:
            /**
            * @brief to save targetCsvFile into csvFile.
            * @param targetCsvFile
            */
            explicit WuKongSysEventListener(std::ofstream& targetCsvFile) : csvFile(targetCsvFile) {}

            /**
            * @brief to export the exception information, such as domain, eventname, eventType, eventDetail.
            * @param domain the location of the event when the exception occurs.
            * @param eventName the eventname of the event when the exception occurs.
            * @param eventType the event type of the event when the exception occurs.
            * @param eventDetail  the detail of the event when the exception occurs.
            * @return -
            */
            void OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
                          const std::string& eventDetail) override;

            /**
            * @brief to export a setence: "Listener service Died"
            * @return -
            */ 
            void OnServiceDied() override;
            ~WuKongSysEventListener() override {}
        private:
            // the file that save the information when exceptions occur. 
            std::ofstream& csvFile;
        };
    } // namespace AppExecFwk
} // namespace OHOS

#endif // WUKONG_WUKONG_SYSEVENT_LISTENER_H

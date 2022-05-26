/*
 * @Description:
 * @Date: 2022-04-21 19:39:15
 * @LastEditTime: 2022-04-21 19:54:16
 * @FilePath: /wukong/report/include/sysevent_listener.h
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

#ifndef TEST_WUKONG_SYSEVENT_LISTENER_H
#define TEST_WUKONG_SYSEVENT_LISTENER_H

#include "hisysevent_subscribe_callback.h"

namespace OHOS {
namespace WuKong {
class SysEventListener : public OHOS::HiviewDFX::HiSysEventSubscribeCallBack {
public:
    explicit SysEventListener(std::ofstream& targetCsvFile) : csvFile(targetCsvFile)
    {
    }
    void OnHandle(const std::string& domain, const std::string& eventName, const int eventType,
                  const std::string& eventDetail) override;
    void OnServiceDied() override;
    ~SysEventListener() override
    {
    }

private:
    std::ofstream& csvFile;
};
}  // namespace WuKong
}  // namespace OHOS

#endif  // TEST_WUKONG_SYSEVENT_LISTENER_H
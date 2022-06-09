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

#ifndef TEST_WUKONG_EXCEPTION_MANAGER_H
#define TEST_WUKONG_EXCEPTION_MANAGER_H

#include <fstream>

#include "errors.h"
#include "singleton.h"
#include "sysevent_listener.h"

namespace OHOS {
namespace WuKong {
class ExceptionManager : public DelayedSingleton<ExceptionManager> {
public:
    bool StartCatching();
    void StopCatching();
    void SetCppCrashCatching(bool isEnable)
    {
        enableCppCrashCatching = isEnable;
    }

    void SetJsCrashCatching(bool isEnable)
    {
        enableJsCrashCatching = isEnable;
    }

    void SetAppFreezeCatching(bool isEnable)
    {
        enableAppFreezeCatching = isEnable;
    }

    std::string GetCurrentCsvFilePath()
    {
        return currentCsvFilePath;
    }

private:
    std::shared_ptr<SysEventListener> toolListener;
    std::string currentCsvFilePath;
    std::ofstream csvFile;
    bool enableCppCrashCatching;
    bool enableJsCrashCatching;
    bool enableAppFreezeCatching;
};
}  // namespace WuKong
}  // namespace OHOS

#endif  // WUKONG_WUKONG_EXCEPTION_MANAGER_H

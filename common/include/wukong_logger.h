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
#ifndef TEST_WUKONG_WUKONG_LOGGER_H
#define TEST_WUKONG_WUKONG_LOGGER_H

#include <condition_variable>
#include <mutex>
#include <queue>
#include <stdio.h>
#include <string>
#include <thread>

#include "singleton.h"
#include "thread_ex.h"

namespace OHOS {
namespace WuKong {
enum LOG_LEVEL {
    LOG_LEVEL_TRACK = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
};

enum WK_LOG_OUTPUT_TYPE {
    STD_OUTPUT = 0x0001,
    FILE_OUTPUT = 0x0002,
    HILOG_OUTPUT = 0x0004,
};

class WuKongLogger : public DelayedSingleton<WuKongLogger> {
public:
    /**
     * @brief logger start
     */
    void SetLevel(LOG_LEVEL level)
    {
        outputLevel_ = level;
    }
    /**
     * @brief logger start
     */
    bool Start();
    /**
     * @brief logger stop
     */
    void Stop();
    /**
     * @brief  print log
     * @param level logger level
     * @param format log string format
     */
    void Print(LOG_LEVEL level, const char *format, ...);

    LOG_LEVEL GetLogLevel()
    {
        return outputLevel_;
    }

    DECLARE_DELAYED_SINGLETON(WuKongLogger);

private:
    class PrinterThread : public OHOS::Thread {
        /**
         * @brief read queue and print log to file
         */
        bool Run() override;
    };

    class LogInfo {
    public:
        std::string logStr_;
        LOG_LEVEL level_;
    };

    // current logger level
    LOG_LEVEL outputLevel_ = LOG_LEVEL_INFO;

    // current output setting
    uint32_t outputType_ = STD_OUTPUT | FILE_OUTPUT | HILOG_OUTPUT;

    // current process disk filename
    std::string logFileName_ = "";

    // Queue lock
    std::mutex mtxQueue_;
    std::queue<LogInfo> bufferQueue_;

    // log printer thread.
    bool printerRunning_ = false;
    PrinterThread logPrinter_;
    std::mutex mtxThreadWait_;
    std::condition_variable cvWaitPrint_;
};
}  // namespace WuKong
}  // namespace OHOS

#endif  // TEST_WUKONG_WUKONG_LOGGER_H

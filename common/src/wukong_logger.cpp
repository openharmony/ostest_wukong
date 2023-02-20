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

#include "wukong_logger.h"

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <hilog/log_c.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "securec.h"
#include "string_ex.h"
#include "wukong_define.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
namespace {
const std::string DEFAULT_DIR = "/data/local/tmp/wukong/report/";
const std::string LOGGER_THREAD_NAME = "wukong_logger";
const int LOG_CONTENT_LENGTH = 256;
const int LOG_PRINTER_TIMEOUT = 500;
std::mutex LOCK_PRINT_BUFFER;
}  // namespace

WuKongLogger::WuKongLogger() : logPrinter_()
{ /* init buf */
}

/**
 * @brief: release logfile fd
 */
WuKongLogger::~WuKongLogger()
{
    if (outputLevel_ <= LOG_LEVEL_TRACK) {
        std::cout << "Logger::~Logger" << std::endl;
    }

    // check logPrinter thread is running, and stop
    if (printerRunning_ && logPrinter_.IsRunning()) {
        Stop();
    }
}

bool WuKongLogger::Start()
{
    if (outputLevel_ <= LOG_LEVEL_TRACK) {
        std::cout << "Logger::Start" << std::endl;
    }

    logFileName_ = WuKongUtil::GetInstance()->GetCurrentTestDir() + "wukong.log";

    if (logPrinter_.IsRunning()) {
        DEBUG_LOG("Logger already started");
        return true;
    }

    /* start read Queue Thread */
    printerRunning_ = true;
    logPrinter_.Start(LOGGER_THREAD_NAME);

    // wait print thread started.
    do {
        std::unique_lock<std::mutex> lk(mtxThreadWait_);
        cvWaitPrint_.wait(lk);
    } while (false);
    return true;
}

void WuKongLogger::Stop()
{
    /* release readQueueThread */
    if (outputLevel_ <= LOG_LEVEL_TRACK) {
        std::cout << "Logger::Stop" << std::endl;
    }
    if (printerRunning_ && logPrinter_.IsRunning()) {
        printerRunning_ = false;
        cvWaitPrint_.notify_all();
        logPrinter_.NotifyExitSync();
    }
}

void WuKongLogger::Print(LOG_LEVEL level, const char *format, ...)
{
    if (!printerRunning_) {
        return;
    }
    LOCK_PRINT_BUFFER.lock();
    char writeBuf[LOG_CONTENT_LENGTH] = {0};
    /* check logger_level */
    if (level < outputLevel_ && level < LOG_LEVEL_DEBUG) {
        LOCK_PRINT_BUFFER.unlock();
        return;
    }
    /* format output content */
    va_list args;
    va_start(args, format);
    int ret = vsnprintf_s(writeBuf, LOG_CONTENT_LENGTH, LOG_CONTENT_LENGTH, format, args);
    if (ret < 0) {
        va_end(args);
        LOCK_PRINT_BUFFER.unlock();
        return;
    }
    va_end(args);

    // write lock avoid write conflicts
    LogInfo logInfo;
    if (outputLevel_ <= LOG_LEVEL_TRACK) {
        time_t currentTime = time(0);
        char *timeChar = ctime(&currentTime);
        logInfo.logStr_.append(timeChar, strlen(timeChar) - 1);
        logInfo.logStr_.append(" : ");
    }
    logInfo.logStr_.append(writeBuf, strlen(writeBuf));
    logInfo.level_ = level;
    LOCK_PRINT_BUFFER.unlock();
    // if log level is less than LOG_LEVEL_DEBUG, cout print.
    if (outputLevel_ <= LOG_LEVEL_TRACK) {
        std::cout << logInfo.logStr_ << std::endl;
    }

    // push log to buffer queue.
    mtxQueue_.lock();
    bufferQueue_.push(logInfo);
    mtxQueue_.unlock();

    // notify print log to printer thread.
    cvWaitPrint_.notify_all();
}

/**
 * @brief read queue and print log to file
 */
bool WuKongLogger::PrinterThread::Run()
{
    std::shared_ptr<WuKongLogger> self = WuKongLogger::GetInstance();
    if (!self->printerRunning_) {
        return false;
    }
    self->cvWaitPrint_.notify_all();
    std::unique_lock<std::mutex> lk(self->mtxThreadWait_);
    std::queue<LogInfo> tmpBuffer;
    std::ofstream printer(self->logFileName_);
    if (!printer.is_open()) {
        std::cout << "ERR: Logger printer file cannot open" << std::endl;
        return false;
    }
    /* read form queue output target fd */
    printer << "WuKongLogger::PrinterThread::Run start" << std::endl;
    const auto timeout = std::chrono::milliseconds(LOG_PRINTER_TIMEOUT);
    while (true) {
        // wait new log
        if (self->printerRunning_) {
            if (self->outputLevel_ <= LOG_LEVEL_TRACK) {
                printer << "WuKongLogger::PrinterThread::Run wait start" << std::endl;
            }
            self->cvWaitPrint_.wait_for(lk, timeout);
            if (self->outputLevel_ <= LOG_LEVEL_TRACK) {
                printer << "WuKongLogger::PrinterThread::Run wait end" << std::endl;
            }
        }
        // read queue buffer to thread buffer.
        self->mtxQueue_.lock();
        // the buffer queue is empty and main wait stop, return this thread.
        if (self->bufferQueue_.empty() && !self->printerRunning_) {
            self->mtxQueue_.unlock();
            break;
        }
        while (!self->bufferQueue_.empty()) {
            tmpBuffer.push(self->bufferQueue_.front());
            self->bufferQueue_.pop();
        }
        self->mtxQueue_.unlock();
        // print log to file and std::cout and hilog
        while (!tmpBuffer.empty()) {
            auto logInfo = tmpBuffer.front();
            tmpBuffer.pop();
            // output file fd
            if (self->outputType_ & FILE_OUTPUT) {
                printer << logInfo.logStr_ << std::endl;
            }
            // doesn't print STDOUT and HILOG, when log level less than output level.
            if (logInfo.level_ < self->outputLevel_) {
                continue;
            }
            // output STDOUT
            if ((self->outputType_ & STD_OUTPUT) && (self->outputLevel_ > LOG_LEVEL_TRACK)) {
                std::cout << logInfo.logStr_ << std::endl;
            }
            // output HILOG
            if (self->outputType_ & HILOG_OUTPUT) {
                HILOG_INFO(LOG_CORE, "%{public}s", logInfo.logStr_.c_str());
            }
        }
    }
    printer << "WuKongLogger::PrinterThread::Run end" << std::endl;
    printer.close();
    return false;
}
}  // namespace WuKong
}  // namespace OHOS

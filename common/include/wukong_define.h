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

#ifndef TEST_WUKONG_COMMON_H
#define TEST_WUKONG_COMMON_H

#include "errors.h"
#include "wukong_logger.h"
#include "wukong_util.h"

namespace OHOS {
namespace WuKong {
const uint32_t INVALIDVALUE = 0xFFFFFFFF;
const uint32_t WAIT_TIME = 1000000;
#define COMPONENT_LEFT_SWAP 0X10000000

#define PARAM_COUNT_TIME_ERROR "has set '-T', can not set '-T' and '-c' at the same time"
#define PARAM_TIME_COUNT_ERROR "has set '-c', can not set '-c' and '-T' at the same time"

#define SEMPHORE_RUN_NAME "wukong_sem_run"
#define SEMPHORE_STOP_NAME "wukong_sem_stop"

#define TRACK_LOG_STD() WuKongLogger::GetInstance()->Print(LOG_LEVEL_TRACK, "TRK : (%5d) %s START", __LINE__, __func__)
#define TRACK_LOG_END() WuKongLogger::GetInstance()->Print(LOG_LEVEL_TRACK, "TRK : (%5d) %s END", __LINE__, __func__)
#define TRACK_LOG_STR(format, ...) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_TRACK, "TRK : (%5d) %s : " #format, __LINE__, __func__, __VA_ARGS__)
#define DEBUG_LOG_STR(format, ...) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_DEBUG, "DBG : (%5d) %s : " #format, __LINE__, __func__, __VA_ARGS__)
#define INFO_LOG_STR(format, ...) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_INFO, "INF : (%5d) %s : " #format, __LINE__, __func__, __VA_ARGS__)
#define WARN_LOG_STR(format, ...) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_WARN, "WRN : (%5d) %s : " #format, __LINE__, __func__, __VA_ARGS__)
#define ERROR_LOG_STR(format, ...) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_ERROR, "ERR : (%5d) %s : " #format, __LINE__, __func__, __VA_ARGS__)
#define TRACK_LOG(str) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_TRACK, "TRK : (%5d) %s : %s", __LINE__, __func__, str)
#define DEBUG_LOG(str) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_DEBUG, "DBG : (%5d) %s : %s", __LINE__, __func__, str)
#define INFO_LOG(str) WuKongLogger::GetInstance()->Print(LOG_LEVEL_INFO, "INF : (%5d) %s : %s", __LINE__, __func__, str)
#define WARN_LOG(str) WuKongLogger::GetInstance()->Print(LOG_LEVEL_WARN, "WRN : (%5d) %s : %s", __LINE__, __func__, str)
#define ERROR_LOG(str) \
    WuKongLogger::GetInstance()->Print(LOG_LEVEL_ERROR, "ERR : (%5d) %s : %s", __LINE__, __func__, str)
#define TEST_RUN_LOG(str) WuKongLogger::GetInstance()->Print(LOG_LEVEL_INFO, "RUN : %s", str)
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_COMMON_H

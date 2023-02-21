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

#include "record_input.h"

#include <ctime>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <sys/stat.h>
#include <typeinfo>

#include "ability_manager_client.h"
#include "multimode_manager.h"

namespace OHOS {
namespace WuKong {
namespace {
const int INTERVALTIME = 1000;
const int NUMTWO = 2;
std::string g_defaultDir = "/data/local/tmp/wukong/record";
std::ofstream g_outFile;
int64_t g_timeTemp = -1;
struct EventData {
    int xPosi;
    int yPosi;
    int interval = 1;
};

int64_t GetMillisTime()
{
    auto timeNow = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(timeNow.time_since_epoch());
    return tmp.count();
}

static std::vector<std::string> split(const std::string &in, const std::string &delim)
{
    std::regex reg(delim);
    std::vector<std::string> res = {std::sregex_token_iterator(in.begin(), in.end(), reg, -1),
                                    std::sregex_token_iterator()};
    return res;
}

static void WriteEventHead(std::ofstream &outFile)
{
    outFile << "x" << ',';
    outFile << "y" << ',';
    outFile << "interval" << std::endl;
}

static void WriteEventData(std::ofstream &outFile, const EventData &data)
{
    outFile << data.xPosi << ',';
    outFile << data.yPosi << ',';
    outFile << data.interval << std::endl;
}

bool InitReportFolder()
{
    DIR *rootDir = nullptr;
    if ((rootDir = opendir(g_defaultDir.c_str())) == nullptr) {
        int ret = mkdir(g_defaultDir.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
        if (ret != 0) {
            std::cerr << "failed to create dir: " << g_defaultDir << std::endl;
            return false;
        }
    } else {
        closedir(rootDir);
    }
    return true;
}

bool InitEventRecordFile(std::ofstream &outFile, const std::string &recordName)
{
    if (!InitReportFolder()) {
        ERROR_LOG("init folder failed");
        return false;
    }
    std::string filePath = g_defaultDir + "/" + recordName + ".csv";
    outFile.open(filePath, std::ios_base::out | std::ios_base::trunc);
    if (!outFile) {
        ERROR_LOG_STR("Failed to create csv file at: %s", filePath.c_str());
        return false;
    }
    WriteEventHead(outFile);
    INFO_LOG_STR("The result will be written in csv file at location: %s", filePath.c_str());
    return true;
}

ErrCode ReadEventLine(std::ifstream &inFile)
{
    ErrCode result = OHOS::ERR_OK;
    char buffer[50];
    int xPosi = -1;
    int yPosi = -1;
    int interval = -1;
    bool jumpFlag = true;
    while (!inFile.eof()) {
        inFile >> buffer;
        if (jumpFlag) {
            jumpFlag = !jumpFlag;
            continue;
        }
        jumpFlag = !jumpFlag;
        std::string delim = ",";
        auto caseInfo = split(buffer, delim);
        xPosi = std::stoi(caseInfo[0]);
        yPosi = std::stoi(caseInfo[1]);
        interval = std::stoi(caseInfo[NUMTWO]);
        INFO_LOG_STR("Position: (%d,%d)  interval: %d", xPosi, yPosi, interval);
        auto recordTouchInput = MultimodeManager::GetInstance();
        result = recordTouchInput->PointerInput(xPosi, yPosi, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                                MMI::PointerEvent::POINTER_ACTION_DOWN);
        if (result != OHOS::ERR_OK) {
            ERROR_LOG("input failed");
            return result;
        }
        result = recordTouchInput->PointerInput(xPosi, yPosi, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                                MMI::PointerEvent::POINTER_ACTION_UP);
        usleep(interval * INTERVALTIME);
    }
    return result;
}

class InputEventCallback : public MMI::IInputEventConsumer {
public:
    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override
    {
        INFO_LOG_STR("keyCode: %d", keyEvent->GetKeyCode());
    }
    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override
    {
        MMI::PointerEvent::PointerItem item;
        bool result = pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
        if (!result) {
            ERROR_LOG("GetPointerItem Fail");
        }
        EventData data {};
        int64_t currentTime = GetMillisTime();
        if (g_timeTemp == -1) {
            g_timeTemp = currentTime;
            data.interval = INTERVALTIME;
        } else {
            data.interval = currentTime - g_timeTemp;
            g_timeTemp = currentTime;
        }
        data.xPosi = item.GetDisplayX();
        data.yPosi = item.GetDisplayY();
        WriteEventData(g_outFile, data);
        INFO_LOG_STR("PointerEvent received. interval: %d xPosi: %d yPosi: %d", data.interval, data.xPosi, data.yPosi);
    }
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override
    {
    }
    static std::shared_ptr<InputEventCallback> GetPtr();
};

std::shared_ptr<InputEventCallback> InputEventCallback::GetPtr()
{
    return std::make_shared<InputEventCallback>();
}
}  // namespace
RecordInput::RecordInput() : InputAction()
{
}

RecordInput::~RecordInput()
{
}

ErrCode RecordInput::OrderInput(const std::shared_ptr<SpcialTestObject> &specialTestObject)
{
    int result = ERR_OK;
    auto recordPtr = std::static_pointer_cast<RecordParam>(specialTestObject);
    if (recordPtr->recordStatus_) {
        if (!InitEventRecordFile(g_outFile, recordPtr->recordName_)) {
            ERROR_LOG("init file failed");
            specialTestObject->isAllFinished_ = true;
            return OHOS::ERR_INVALID_VALUE;
        }
        auto callBackPtr = InputEventCallback::GetPtr();
        if (callBackPtr == nullptr) {
            ERROR_LOG("input callback is nullptr");
            specialTestObject->isAllFinished_ = true;
            return OHOS::ERR_INVALID_VALUE;
        }
        int32_t id1 = MMI::InputManager::GetInstance()->AddMonitor(callBackPtr);
        if (id1 == -1) {
            ERROR_LOG("Startup Failed!");
            specialTestObject->isAllFinished_ = true;
            return OHOS::ERR_INVALID_VALUE;
        }
        INFO_LOG("Started Recording Successfully...");
        int flag = getc(stdin);
        specialTestObject->isAllFinished_ = true;
        TRACK_LOG_STR("flag: %d", flag);
    } else {
        std::ifstream inFile(g_defaultDir + "/" + recordPtr->recordName_ + ".csv");
        result = ReadEventLine(inFile);
        if (result != ERR_OK) {
            WARN_LOG("this input failed");
            return result;
        }
        specialTestObject->isAllFinished_ = true;
    }
    return result;
}
}  // namespace WuKong
}  // namespace OHOS

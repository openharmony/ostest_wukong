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

#include <iostream>
#include <fstream>
#include <ctime>
#include <dirent.h>
#include <sys/stat.h>
#include <regex>
#include <typeinfo>

#include "multimode_manager.h"
#include "ability_manager_client.h"

namespace OHOS {
namespace WuKong {
namespace {
const int INTERVALTIME = 1000;
const int NUMTWO = 2;
std::string DEFAULT_DIR = "/data/local/wukong/record";
std::ofstream outFile;
int64_t timeTemp = -1;
struct eventData {
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
    std::vector<std::string> res = {
    std::sregex_token_iterator(in.begin(), in.end(), reg, -1), std::sregex_token_iterator()};
    return res;
}

static void WriteEventHead(std::ofstream &outFile)
{
    outFile << "x" << ',';
    outFile << "y" << ',';
    outFile << "interval" << std::endl;
}

static void WriteEventData(std::ofstream &outFile, const eventData &data)
{
    outFile << data.xPosi << ',';
    outFile << data.yPosi << ',';
    outFile << data.interval << std::endl;
}

bool InitReportFolder()
{
    DIR *rootDir = nullptr;
    if ((rootDir = opendir(DEFAULT_DIR.c_str())) == nullptr) {
        int ret = mkdir(DEFAULT_DIR.c_str(), S_IROTH | S_IRWXU | S_IRWXG);
        if (ret != 0) {
            std::cerr << "failed to create dir: " << DEFAULT_DIR << std::endl;
            return false;
        }
    }
    return true;
}

bool InitEventRecordFile(std::ofstream &outFile, std::string recordName_)
{
    if (!InitReportFolder()) {
        return false;
    }
    std::string filePath = DEFAULT_DIR + "/" + recordName_ + ".csv";
    outFile.open(filePath, std::ios_base::out | std::ios_base::trunc);
    if (!outFile) {
        std::cerr << "Failed to create csv file at:" << filePath << std::endl;
        return false;
    }
    WriteEventHead(outFile);
    std::cout << "The result will be written in csv file at location: " << filePath << std::endl;
    return true;
}

ErrCode ReadEventLine(std::ifstream &inFile)
{
    int result = ERR_OK;
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

        std::cout << xPosi << ";"
                  << yPosi << ";"
                  << interval << std::endl;
        auto recordTouchInput = MultimodeManager::GetInstance();
        result = recordTouchInput->PointerInput(xPosi, yPosi, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                                MMI::PointerEvent::POINTER_ACTION_DOWN);
        result = recordTouchInput->PointerInput(xPosi, yPosi, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN,
                                                MMI::PointerEvent::POINTER_ACTION_UP);
        usleep(interval * INTERVALTIME);
    }
    return result;
}

class InputEventCallback : public MMI::IInputEventConsumer {
public:
    virtual void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override
    {
        std::cout << "keyCode" << keyEvent->GetKeyCode() << std::endl;
    }
    virtual void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override
    {
        MMI::PointerEvent::PointerItem item;
        bool result = pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), item);
        if (!result) {
            std::cout << "GetPointerItem Fail" << std::endl;
        }
        eventData data {};
        int64_t currentTime = GetMillisTime();
        if (timeTemp == -1) {
            timeTemp = currentTime;
            data.interval = INTERVALTIME;
        } else {
            data.interval = currentTime - timeTemp;
            timeTemp = currentTime;
        }
        data.xPosi = item.GetGlobalX();
        data.yPosi = item.GetGlobalY();
        WriteEventData(outFile, data);
        std::cout << " PointerEvent received."
                  << " interval: " << data.interval
                  << " xPosi:" << data.xPosi
                  << " yPosi:" << data.yPosi << std::endl;
    }
    virtual void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override {}
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

ErrCode RecordInput::OrderInput(const std::shared_ptr<SpcialTestObject>& specialTestObject)
{
    int result = ERR_OK;
    auto recordPtr = std::static_pointer_cast<RecordParam>(specialTestObject);
    if (recordPtr->recordStatus_ == true) {
        if (!InitEventRecordFile(outFile, recordPtr->recordName_)) {
        return OHOS::ERR_INVALID_VALUE;
        }
        auto callBackPtr = InputEventCallback::GetPtr();
        if (callBackPtr == nullptr) {
            std::cout << "nullptr" << std::endl;
        }
        int32_t id1 = MMI::InputManager::GetInstance()->AddMonitor(callBackPtr);
        if (id1 == -1) {
            std::cout << "Startup Failed!" << std::endl;
        }
        std::cout << "Started Recording Successfully..." << std::endl;
        int flag = getc(stdin);
        std::cout << flag << std::endl;
    } else {
        std::ifstream inFile(DEFAULT_DIR + "/" + recordPtr->recordName_ + ".csv");
        result = ReadEventLine(inFile);
    }
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

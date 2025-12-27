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
#include <algorithm>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <map>
#include <vector>
#include <random>

#include "multimode_manager.h"
#include "input_action.h"

namespace OHOS {
namespace WuKong {
namespace {
const int SHORTEST_LEN = 2;
const int LONGEST_LEN = 20;
const int DEFAULT_PRESSURE = 5;
const int SLEEP_TIME = 16000;
const int DEFAULT_PRESS_TIME = 50;
const int DEFAULT_INTERVAL_TIME = 300;
const int DEFAULT_SCROLL_SCALE = 15;
const int DEFALUT_PORT_NUM = 8012;
}  // namespace
MultimodeManager::MultimodeManager()
{
    for (int i = OHOS::MMI::KeyEvent::KEYCODE_CALL; i <= OHOS::MMI::KeyEvent::KEYCODE_ENDCALL; i++) {
        keycodelist_.push_back(i);
    }

    for (int j = OHOS::MMI::KeyEvent::KEYCODE_0; j <= OHOS::MMI::KeyEvent::KEYCODE_NUMPAD_RIGHT_PAREN; j++) {
        keycodelist_.push_back(j);
    }
}

MultimodeManager::~MultimodeManager()
{
}

ErrCode MultimodeManager::SingleKeyCodeInput(int keycode, int downtime)
{
    ErrCode result = ERR_OK;
    std::string keycodeType = OHOS::MMI::KeyEvent::KeyCodeToString(keycode);
    INFO_LOG_STR("keycodeType: %s", keycodeType.c_str());
    auto keyKeyboardEvent = OHOS::MMI::KeyEvent::Create();
    if (!keyKeyboardEvent) {
        WARN_LOG("keyKeyboardEvent is nullptr");
        return OHOS::ERR_NO_INIT;
    }
    MMI::KeyEvent::KeyItem item;
    item.SetKeyCode(keycode);
    item.SetPressed(true);
    item.SetDownTime(downtime);
    keyKeyboardEvent->SetKeyCode(keycode);
    keyKeyboardEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_DOWN);
    keyKeyboardEvent->AddPressedKeyItems(item);
    // check if KeyEvent is valid
    if (keyKeyboardEvent->IsValid()) {
        MMI::InputManager::GetInstance()->SimulateInputEvent(keyKeyboardEvent);
    } else {
        WARN_LOG("keyevent down is invalid");
        return OHOS::ERR_NO_INIT;
    }
    keyKeyboardEvent->RemoveReleasedKeyItems(item);
    item.SetKeyCode(keycode);
    item.SetPressed(false);
    item.SetDownTime(downtime);
    keyKeyboardEvent->SetKeyCode(keycode);
    keyKeyboardEvent->SetKeyAction(MMI::KeyEvent::KEY_ACTION_UP);
    keyKeyboardEvent->AddPressedKeyItems(item);
    // check if KeyEvent is valid
    if (keyKeyboardEvent->IsValid()) {
        MMI::InputManager::GetInstance()->SimulateInputEvent(keyKeyboardEvent);
    } else {
        WARN_LOG("keyevent up is invalid");
        return OHOS::ERR_NO_INIT;
    }
    keyKeyboardEvent->RemoveReleasedKeyItems(item);

    return result;
}

ErrCode MultimodeManager::MultiKeyCodeInput(int downtime)
{
    ErrCode result = OHOS::ERR_OK;
    // generate the length of string randomly
    int stringLen = SHORTEST_LEN + random() % (LONGEST_LEN - 1);
    if (keycodelist_.size() > 0) {
        for (int i = 0; i < stringLen; i++) {
            int keycode = keycodelist_[(uint32_t)(rand()) % keycodelist_.size()];
            result = MultimodeManager::GetInstance()->SingleKeyCodeInput(keycode, downtime);
        }
    } else {
        return OHOS::ERR_NO_INIT;
    }
    return result;
}

void MultimodeManager::GetKeycodeList(std::vector<int> &keycodelist)
{
    keycodelist = keycodelist_;
}

ErrCode MultimodeManager::PointerInput(int x, int y, int pointertype, int actiontype)
{
    ErrCode result = OHOS::ERR_OK;
    auto pointerEvent = MMI::PointerEvent::Create();
    if (!pointerEvent) {
        WARN_LOG("pointerEvent is nullptr");
        return OHOS::ERR_NO_INIT;
    }
    MMI::PointerEvent::PointerItem item;

    item.SetPointerId(0);
    item.SetDisplayX(x);
    item.SetDisplayY(y);
    item.SetPressure(DEFAULT_PRESSURE);
    pointerEvent->AddPointerItem(item);

    pointerEvent->SetPointerAction(actiontype);
    pointerEvent->SetSourceType(pointertype);
    pointerEvent->SetPointerId(0);

    MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

    return result;
}

ErrCode MultimodeManager::IntervalSwap(int xSrcPosition, int ySrcPosition, int xDstPosition, int yDstPosition)
{
    auto multiinput = MultimodeManager::GetInstance();
    ErrCode result = multiinput->PointerInput(
        xSrcPosition, ySrcPosition, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN, MMI::PointerEvent::POINTER_ACTION_DOWN);
    if (result != OHOS::ERR_OK) {
        return result;
    }
    usleep(SLEEP_TIME);
    int segment = 50;
    float secX = (xDstPosition - xSrcPosition) / (float)segment;
    float secY = (yDstPosition - ySrcPosition) / (float)segment;

    for (int i = 1; i < segment; ++i) {
        int mPosX = int(xSrcPosition + secX * i);
        int mPosY = int(ySrcPosition + secY * i);
        result = multiinput->PointerInput(
            mPosX, mPosY, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN, MMI::PointerEvent::POINTER_ACTION_MOVE);
        if (result != OHOS::ERR_OK) {
            return result;
        }
        usleep(SLEEP_TIME);
    }
    result = multiinput->PointerInput(
        xDstPosition, yDstPosition, MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN, MMI::PointerEvent::POINTER_ACTION_UP);
    return result;
}

ErrCode MultimodeManager::KnuckleSimulate(int px1, int py1, int interval)
{
    INFO_LOG_STR("single finger knuckle: %d, %d", px1, py1);
    if (WuKongUtil::GetInstance()->GetDeviceType() == "2in1" || WuKongUtil::GetInstance()->GetDeviceType() == "PC") {
        INFO_LOG("single finger knuckle not supported.");
    } else {
        WuKongUtil::GetInstance()->ExecuteCmd("uinput -T -k -s " + WuKongUtil::GetInstance()->GetPositionStr(px1, py1) +
                                              " " + WuKongUtil::GetInstance()->GetPositionStr(px1, py1) + " " +
                                              std::to_string(interval));
    }
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::DoubleKnuckleSimulate(int px1, int py1, int px2, int py2, int interval)
{
    INFO_LOG_STR("double fingers knuckle: (%d, %d), (%d, %d)", px1, py1, px2, py2);
    if (WuKongUtil::GetInstance()->GetDeviceType() == "2in1" || WuKongUtil::GetInstance()->GetDeviceType() == "PC") {
        INFO_LOG("double finger knuckle not supported.");
    } else {
        WuKongUtil::GetInstance()->ExecuteCmd("uinput -T -k -d " + WuKongUtil::GetInstance()->GetPositionStr(px1, py1) +
                                              " " + WuKongUtil::GetInstance()->GetPositionStr(px2, py2) + " " +
                                              std::to_string(interval));
    }
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::PinchSimulate(
    std::string position1, std::string position2, bool isIntoCenter, int totalTimeMs)
{
    INFO_LOG_STR("pinch to: %s", isIntoCenter ? "center" : "border");
    std::istringstream issP1(position1);
    int px1;
    int py1;
    int px2;
    int py2;
    issP1 >> px1 >> py1;
    std::istringstream issP2(position2);
    issP2 >> px2 >> py2;

    int32_t pxc = std::abs(px2 - px1) / ONE_HALF + std::min(px1, px2);
    int32_t pyc = std::abs(py2 - py1) / ONE_HALF + std::min(py1, py2);
    int32_t fx1 = isIntoCenter ? px1 : pxc;
    int32_t fy1 = isIntoCenter ? py1 : pyc;
    int32_t fx2 = isIntoCenter ? px2 : pxc;
    int32_t fy2 = isIntoCenter ? py2 : pyc;
    int32_t tx1 = isIntoCenter ? pxc : px1;
    int32_t ty1 = isIntoCenter ? pyc : py1;
    int32_t tx2 = isIntoCenter ? pxc : px2;
    int32_t ty2 = isIntoCenter ? pyc : py2;

    // 取默认值和-i参数的最小值
    totalTimeMs = std::min(totalTimeMs, DEFAULT_DRAG_TOTAL_TIME_MS);

    std::string fromPos1 = WuKongUtil::GetInstance()->GetPositionStr(fx1, fy1);
    std::string fromPos2 = WuKongUtil::GetInstance()->GetPositionStr(fx2, fy2);
    std::string toPos1 = WuKongUtil::GetInstance()->GetPositionStr(tx1, ty1);
    std::string toPos2 = WuKongUtil::GetInstance()->GetPositionStr(tx2, ty2);
    INFO_LOG_STR("the first finger from %s to %s", fromPos1.c_str(), toPos1.c_str());
    INFO_LOG_STR("the second finger from %s to %s", fromPos2.c_str(), toPos2.c_str());
    INFO_LOG_STR("pinch time : %d(ms).", totalTimeMs);

    WuKongUtil::GetInstance()->ExecuteCmd("uinput -T -m " + fromPos1 + " " + toPos1 + " " + fromPos2 + " " + toPos2 +
                                          " -k 0 " + std::to_string(totalTimeMs));
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::SwipeForFingersSimulate(
    int srcPx, int srcPy, int fingerNum, bool enablePause, char &direction)
{
    std::string str(1, direction);
    INFO_LOG_STR("the start point is: (%d, %d), the direction is: %s, finger number is: %d.",
        srcPx,
        srcPy,
        str.c_str(),
        fingerNum);

    // 根据滑动方向，计算终点坐标
    int dstPx = (direction == 'u' || direction == 'd')
                    ? srcPx
                    : (direction == 'l' ? srcPx - DEFAULT_SWIPE_MOVE_LENGTH : srcPx + DEFAULT_SWIPE_MOVE_LENGTH);

    int dstPy = (direction == 'l' || direction == 'r')
                    ? srcPy
                    : (direction == 'u' ? srcPy - DEFAULT_SWIPE_MOVE_LENGTH : srcPy + DEFAULT_SWIPE_MOVE_LENGTH);
    WuKongUtil::GetInstance()->SetSwipeEnablePause(enablePause);
    TouchPadSwipeActionEvent(srcPx, srcPy, dstPx, dstPy, fingerNum);
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::TouchPadSwipeActionEvent(int startX, int startY, int endX, int endY, int fingerCount)
{
    constexpr int32_t times = 10;
    int32_t disY = static_cast<int32_t>(static_cast<double>(endY - startY) / times);
    int32_t disX = static_cast<int32_t>(static_cast<double>(endX - startX) / times);
    int32_t actionType[10] = {MMI::PointerEvent::POINTER_ACTION_SWIPE_BEGIN};
    int64_t actionTimeBase =
        WuKongUtil::GetInstance()->GetSysClockTime() - times * DEFAULT_DRAG_TOTAL_TIME_MS * DEFAULT_DRAG_TOTAL_TIME_MS;
    int64_t actionTimeStartTimeDis = fingerCount * DEFAULT_DRAG_TOTAL_TIME_MS;
    int64_t actionStartTime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int64_t actionTime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int32_t sourceType = MMI::PointerEvent::SOURCE_TYPE_TOUCHPAD;
    actionTime[0] = actionTimeBase;
    actionStartTime[0] = (actionTimeBase - actionTimeStartTimeDis) / DEFAULT_DRAG_TOTAL_TIME_MS;
    for (int32_t i = 1; i < times; i++) {
        actionStartTime[i] = actionStartTime[i - 1] + times - fingerCount;
        actionTime[i] = actionTime[i - 1] + times - fingerCount;
        actionType[i] = MMI::PointerEvent::POINTER_ACTION_SWIPE_UPDATE;
    }

    actionType[times - 1] = MMI::PointerEvent::POINTER_ACTION_SWIPE_END;
    for (int32_t i = 0; i < times; i++) {
        auto pointerEvent = CreateEvent(0, actionType[i], 0, sourceType, fingerCount);
        if (pointerEvent == nullptr) {
            ERROR_LOG("create pointer event failed.");
            return OHOS::ERR_NO_INIT;
        }
        pointerEvent->SetActionTime(actionTime[i]);
        pointerEvent->SetActionStartTime(actionStartTime[i]);
        MMI::PointerEvent::PointerItem item;
        item.SetDownTime(pointerEvent->GetActionStartTime());
        item.SetDisplayX(startX + disX * i);
        item.SetDisplayY(startY + disY * i);
        item.SetPointerId(0);
        pointerEvent->SetSourceType(sourceType);
        pointerEvent->AddPointerItem(item);
        std::this_thread::sleep_for(std::chrono::microseconds(SWIPE_PAUSE_TIME));
        // 三指支持停顿，在结束之前，多停留一会
        if (fingerCount == OHOS::WuKong::FingerNumber::F_THREE && WuKongUtil::GetInstance()->GetSwipeEnablePause()) {
            std::this_thread::sleep_for(std::chrono::microseconds(SLEEP_TIME * DEFAULT_PRESSURE));
        }

        MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
    }
    return OHOS::ERR_OK;
}

std::shared_ptr<MMI::PointerEvent> MultimodeManager::CreateEvent(
    int id, int type, int pointerId, int sourceType, int fingerCount)
{
    auto pointerEvent = MMI::PointerEvent::Create();
    pointerEvent->SetId(id);
    pointerEvent->SetOriginPointerAction(type);
    pointerEvent->SetPointerAction(type);
    pointerEvent->SetPointerId(pointerId);
    pointerEvent->SetSourceType(sourceType);
    pointerEvent->SetFingerCount(fingerCount);
    return pointerEvent;
}

ErrCode MultimodeManager::MouseActionSimulate(
    int buttonId, char mouseAction, std::string position1, std::string position2, int number)
{
    std::string actionStr(1, mouseAction);
    std::string inputCommand = "uinput -M -" + actionStr + " ";
    switch (mouseAction) {
        case 'c':
            INFO_LOG_STR("mouse click, buttonId = %d", buttonId);
            inputCommand += std::to_string(buttonId).c_str();
            break;
        case 'b':
            INFO_LOG_STR("mouse double click, buttonId = %d, position = %s.", buttonId, position1.c_str());
            inputCommand += position1 + " " + std::to_string(buttonId) + " " + std::to_string(DEFAULT_PRESS_TIME) +
                            " " + std::to_string(DEFAULT_INTERVAL_TIME);
            break;
        case 'g':
            INFO_LOG_STR("mouse drag, buttonId = %d, start position = %s, end position = %s.",
                buttonId,
                position1.c_str(),
                position2.c_str());
            inputCommand +=
                position1 + " " + position2 + " " + std::to_string(DEFAULT_KNUCKLE_INTERVAL_TIME_MS * ONE_HALF);
            break;
        case 'm':
            INFO_LOG_STR(
                "mouse scroll, buttonId = %d, position = %s, number = %d.", buttonId, position1.c_str(), number);
            inputCommand += position1 + " -s " + std::to_string(DEFAULT_SCROLL_SCALE * number);
            break;
        default:
            ERROR_LOG_STR("wukong does not support this mouse action, action = %s.", actionStr.c_str());
            break;
    }
    WuKongUtil::GetInstance()->ExecuteCmd(inputCommand);
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::WatchSwipeSimulate(std::string position1, std::string position2, int timeMs)
{
    INFO_LOG_STR("watch swipe, start position = %s, end position = %s.", position1.c_str(), position2.c_str());
    return TouchMoveSimulate(position1, position2, 0, timeMs);
}

ErrCode MultimodeManager::DoublePowerKeySimulate()
{
    INFO_LOG("Double Press Power Key");
    WuKongUtil::GetInstance()->ExecuteCmd("uinput -K -d " + std::to_string(MMI::KeyEvent::KEYCODE_POWER) + " -u " +
                                          std::to_string(MMI::KeyEvent::KEYCODE_POWER) + " -d " +
                                          std::to_string(MMI::KeyEvent::KEYCODE_POWER) + " -u " +
                                          std::to_string(MMI::KeyEvent::KEYCODE_POWER));
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::PowerKeySimulate()
{
    INFO_LOG("press Power Key");
    WuKongUtil::GetInstance()->ExecuteCmd("uinput -K -d " + std::to_string(MMI::KeyEvent::KEYCODE_POWER) + " -u " +
                                          std::to_string(MMI::KeyEvent::KEYCODE_POWER));
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::LongPressKeySimulate(int keyCode, int pressTime)
{
    INFO_LOG_STR("start press key for long time, keyCode = %d.", keyCode);
    WuKongUtil::GetInstance()->ExecuteCmd("uinput -K -l " + std::to_string(keyCode) + " " + std::to_string(pressTime));
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::WatchCrownRotateSimulate(int x, int y, int rotateCount)
{
    INFO_LOG("watch crown rotate simulate");
    uitestSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    memset_s(&serv_addr, sizeof(serv_addr), 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(DEFALUT_PORT_NUM);

    WuKongUtil::GetInstance()->ExecuteCmd("/system/bin/uitest start-daemon singleness");

    int linkrst = connect(uitestSocketFd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    INFO_LOG_STR("socket connection result: %d, socketId: %d.", linkrst, uitestSocketFd);
    if (linkrst < 0) {
        ERROR_LOG("socket connection failed...");
        return OHOS::ERR_OK;
    }

    std::string coordinate = "{\"x\":" + std::to_string(x) + ",\"y\":" + std::to_string(y) + "}";
    std::string sendData = "{"
                           "\"module\":\"com.ohos.devicetest.hypiumApiHelper\","
                           "\"method\":\"callHypiumApi\","
                           "\"params\":{"
                           "\"api\":\"Driver.mouseScroll\","
                           "\"this\":\"Driver#0\","
                           "\"args\":[" + coordinate + ",true," + std::to_string(rotateCount) +
                           "],"
                           "\"message_type\":\"hypium\""
                           "},"
                           "\"request_id\":\"2025102811540000\","
                           "\"call\":\"xdevice\""
                           "}";
    char sendBuf[sendData.size()];
    sendData.copy(sendBuf, sendData.size());
    int sendNum = send(uitestSocketFd, sendBuf, sizeof(sendBuf), 0);
    INFO_LOG_STR("socket message to server: %s length: %lu result: %d.", sendBuf, sizeof(sendBuf), sendNum);
    std::this_thread::sleep_for(std::chrono::milliseconds(DEFAULT_DRAG_TOTAL_TIME_MS));
    char recvBuf[200];
    int recvNum = recv(uitestSocketFd, recvBuf, sizeof(recvBuf), 0);
    INFO_LOG_STR("socket message from server: %s length: %lu result: %d." recvBuf, sizeof(recvBuf), recvNum);
    close(uitestSocketFd);
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::LongTouchSimulate(int x, int y, int totalTimeMs)
{
    INFO_LOG_STR("long touch (%d, %d) %dms", x, y, totalTimeMs);
    std::string pos = WuKongUtil::GetInstance()->GetPositionStr(x, y);
    WuKongUtil::GetInstance()->ExecuteCmd("uinput -T -d " + pos + " -i " + std::to_string(totalTimeMs) + " -u " + pos);
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::TouchSimulate(int x, int y, int totalTimeMs)
{
    INFO_LOG_STR("touch (%d, %d).", x, y);
    std::string pos = WuKongUtil::GetInstance()->GetPositionStr(x, y);
    WuKongUtil::GetInstance()->ExecuteCmd("uinput -T -c " + pos + " " + std::to_string(totalTimeMs));
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::PowerSuspendSimulate()
{
    INFO_LOG("Power Suspend Simulate");
    WuKongUtil::GetInstance()->ExecuteCmd("power-shell suspend");
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::PowerWakeupSimulate()
{
    INFO_LOG("Power Wakeup Simulate");
    WuKongUtil::GetInstance()->ExecuteCmd("power-shell wakeup");
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::FloatOrSplitSimulate(std::string srcPosition, std::string dstPosition, int keepTime)
{
    INFO_LOG("Float or Split Simulate");
    return TouchMoveSimulate(srcPosition, dstPosition, keepTime);
}

ErrCode MultimodeManager::TouchMoveSimulate(
    std::string srcPosition, std::string dstPosition, int keepTime, int smoothTime)
{
    std::string inputCommand = "uinput -T -m " + srcPosition + " " + dstPosition + " -k " + std::to_string(keepTime) +
                               " " + std::to_string(smoothTime);
    WuKongUtil::GetInstance()->ExecuteCmd(inputCommand);
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::KeyCombinationSimulate(std::vector<int> keyCodes)
{
    std::string inputCommand = "uinput -K";

    for (int keyCode : keyCodes) {
        inputCommand += " -d ";
        inputCommand += std::to_string(keyCode);
    }

    for (int keyCode : keyCodes) {
        inputCommand += " -u ";
        inputCommand += std::to_string(keyCode);
    }

    WuKongUtil::GetInstance()->ExecuteCmd(inputCommand);
    return OHOS::ERR_OK;
}

ErrCode MultimodeManager::CollapseSimulate(
    std::string &currentStatus, std::string &dstStatus, std::string &foldScreenType)
{
    INFO_LOG_STR("start simulating collapse, source status: %s, dest status: %s, foldScreenType: %s",
        currentStatus.c_str(),
        dstStatus.c_str(),
        foldScreenType.c_str());
    auto util = WuKongUtil::GetInstance();
    bool isThreeFoledProduct = util->StartWith(foldScreenType, "6");
    bool isPcDoubleFoledProduct = util->StartWith(foldScreenType, "5");

    std::string command;
    if (isThreeFoledProduct) {
        command = (dstStatus == StatusToDesc.at(CollapseStatus::C_EXPANDED)) ? THREE_EXPANDED
                  : (dstStatus == StatusToDesc.at(CollapseStatus::C_FOLDED)) ? THREE_FOLDED
                                                                             : THREE_HALF_FOLDED;
    } else if (isPcDoubleFoledProduct) {
        if (currentStatus == StatusToDesc.at(CollapseStatus::C_EXPANDED)) {
            if (dstStatus == StatusToDesc.at(CollapseStatus::C_FOLDED)) {
                util->ExecuteCmd(DOUBLE_HALF_FOLDED);
                util->ExecuteCmd(DOUBLE_FOLDED);
            } else {
                util->ExecuteCmd(DOUBLE_HALF_FOLDED);
            }
        } else if (currentStatus == StatusToDesc.at(CollapseStatus::C_FOLDED)) {
            if (dstStatus == StatusToDesc.at(CollapseStatus::C_EXPANDED)) {
                util->ExecuteCmd(DOUBLE_HALF_FOLDED);
                util->ExecuteCmd(DOUBLE_EXPANDED);
            } else {
                util->ExecuteCmd(DOUBLE_HALF_FOLDED);
            }
        } else {
            command = dstStatus == StatusToDesc.at(CollapseStatus::C_EXPANDED) ? DOUBLE_EXPANDED : DOUBLE_FOLDED;
            util->ExecuteCmd(command);
        }

        return OHOS::ERR_OK;
    } else {
        command = dstStatus == StatusToDesc.at(CollapseStatus::C_EXPANDED)
                      ? COMMON_EXPANDED
                      : (dstStatus == StatusToDesc.at(CollapseStatus::C_FOLDED) ? COMMON_FOLDED : COMMON_HALF_FOLDED);
    }

    util->ExecuteCmd(command);
    return OHOS::ERR_OK;
}

}  // namespace WuKong
}  // namespace OHOS

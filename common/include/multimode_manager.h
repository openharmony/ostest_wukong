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

#ifndef TEST_WUKONG_MULTIMODE_MANAGER_H
#define TEST_WUKONG_MULTIMODE_MANAGER_H

#include <iostream>
#include <string>

#include "input_manager.h"
#include "pointer_event.h"
#include "singleton.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {

const int DEFAULT_DRAG_TOTAL_TIME_MS = 1000;
const int DEFAULT_KNUCKLE_INTERVAL_TIME_MS = 200;
static const int ONE_HALF = 2;
const int DEFAULT_SWIPE_MOVE_LENGTH = 200;
const int SWIPE_PAUSE_TIME = 20;
const int PORT = 4607;

const std::string THREE_FOLDED = "hidumper -s " + std::to_string(PORT) + " -a -secondary,f";
const std::string THREE_HALF_FOLDED = "hidumper -s " + std::to_string(PORT) + " -a -secondary,m";
const std::string THREE_EXPANDED = "hidumper -s " + std::to_string(PORT) + " -a -secondary,g";
const std::string DOUBLE_FOLDED = "hidumper -s " + std::to_string(PORT) + " -a -supertrans,3";
const std::string DOUBLE_HALF_FOLDED = "hidumper -s " + std::to_string(PORT) + " -a -supertrans,2";
const std::string DOUBLE_EXPANDED = "hidumper -s " + std::to_string(PORT) + " -a -supertrans,1";
const std::string COMMON_FOLDED = "hidumper -s " + std::to_string(PORT) + " -a -p";
const std::string COMMON_HALF_FOLDED = "hidumper -s " + std::to_string(PORT) + " -a -z";
const std::string COMMON_EXPANDED = "hidumper -s " + std::to_string(PORT) + " -a -y";

class MultimodeManager : public DelayedSingleton<MultimodeManager> {
public:
    MultimodeManager();
    virtual ~MultimodeManager();

    /**
     * @brief the function of pointer input.
     * @param x x coordinate.
     * @param y y coordinate.
     * @param pointertype pointer type,refer to [AxisType].
     * @param actiontype action type.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode PointerInput(int x, int y, int pointertype, int actiontype);

    /**
     * @brief input single keycode.
     * @param keycode keycode type.
     * @param downtime pressed time.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode SingleKeyCodeInput(int keycode, int downtime);

    /**
     * @brief get the list of valid keycode.
     * @param keycodelist  the list of valid keycode.
     */
    void GetKeycodeList(std::vector<int> &keycodelist);

    /**
     * @brief input multiple keycodes randomly.
     * @param downtime pressed time.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode MultiKeyCodeInput(int downtime);

    /**
     * @brief segmented input swap event
     * @param xSrcPosition the x coordinate of the start of the swap
     * @param ySrcPosition the y coordinate of the start of the swap
     * @param xDstPosition the x coordinate of the end of the swap
     * @param yDstPosition the y coordinate of the end of the swap
     * @return Return ERR_OK on success， others on failure.
     */
    ErrCode IntervalSwap(int xSrcPosition, int ySrcPosition, int xDstPosition, int yDstPosition);
    ErrCode KnuckleSimulate(int xPosition, int yPosition, int interval = DEFAULT_KNUCKLE_INTERVAL_TIME_MS);
    ErrCode DoubleKnuckleSimulate(int px1, int py1, int px2, int py2, int interval = DEFAULT_KNUCKLE_INTERVAL_TIME_MS);
    ErrCode PinchSimulate(
        std::string position1, std::string position2, bool isIntoCenter, int totalTimeMs = DEFAULT_DRAG_TOTAL_TIME_MS);
    ErrCode SwipeForFingersSimulate(int srcPx, int srcPy, int fingerNum, bool enablePause, char &direction);
    ErrCode TouchPadSwipeActionEvent(int startX, int startY, int endX, int endY, int fingerCount);
    std::shared_ptr<MMI::PointerEvent> CreateEvent(int id, int type, int pointerId, int sourceType, int fingerCount);
    ErrCode MouseActionSimulate(
        int buttonId, char mouseAction, std::string position1, std::string position2, int number);
    ErrCode WatchSwipeSimulate(
        std::string position1, std::string position2, int timeMs = DEFAULT_KNUCKLE_INTERVAL_TIME_MS);
    ErrCode DoublePowerKeySimulate();
    ErrCode PowerKeySimulate();
    ErrCode LongPressKeySimulate(int keyCode, int pressTime);
    ErrCode WatchCrownRotateSimulate(int x, int y, int rotateCount);
    ErrCode LongTouchSimulate(int x, int y, int totalTimeMs = DEFAULT_DRAG_TOTAL_TIME_MS);
    ErrCode TouchSimulate(int x, int y, int totalTimeMs = SWIPE_PAUSE_TIME * ONE_HALF);
    ErrCode PowerSuspendSimulate();
    ErrCode PowerWakeupSimulate();
    ErrCode FloatOrSplitSimulate(std::string srcPosition, std::string dstPosition, int keepTime);
    ErrCode TouchMoveSimulate(std::string srcPosition, std::string dstPosition, int keepTime = 0,
        int smoothTime = DEFAULT_DRAG_TOTAL_TIME_MS);
    ErrCode KeyCombinationSimulate(std::vector<int> keyCodes);
    ErrCode CollapseSimulate(std::string &currentStatus, std::string &dstStatus, std::string &foldScreenType);

private:
    // keycodelist
    std::vector<int> keycodelist_;
    int uitestSocketFd = -1;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

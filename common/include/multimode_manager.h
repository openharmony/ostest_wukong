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
#include "singleton.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
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

private:
    // keycodelist
    std::vector<int> keycodelist_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

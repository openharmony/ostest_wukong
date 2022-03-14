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

#ifndef WUKONG_WUKONG_EVENT_MANAGER_H
#define WUKONG_WUKONG_EVENT_MANAGER_H


#include "input_manager.h"
#include "singleton.h"

namespace OHOS {
    namespace AppExecFwk {
        class WuKongEventManager : public DelayedSingleton<WuKongEventManager> {
        public:

            ErrCode TouchEvent(int xPosition, int yPosition, int touchTime = 300, int pressure = 5);

            ErrCode MotionEvent(int xSrcPosition, int ySrcPosition,
                                int xDstPosition, int yDstPosition,
                                int pressure = 5);

            ErrCode PowerOnAndOffEvent();

            ErrCode ShellCommandInputEvent(std::string command);
        };
    }
}


#endif // WUKONG_WUKONG_EVENT_MANAGER_H

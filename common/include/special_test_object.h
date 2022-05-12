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

#ifndef TEST_WUKONG_SPECAIL_TEST_OBJECT_H
#define TEST_WUKONG_SPECAIL_TEST_OBJECT_H

#include <string>

namespace OHOS {
namespace WuKong {
class SpcialTestObject {
public:
    SpcialTestObject() {};
    virtual ~SpcialTestObject() {};
    // convert coordinates to string
    virtual std::string toString()
    {
        return std::to_string(objectType_);
    }
    int objectType_;
};
class SwapParam : public SpcialTestObject {
public:
    SwapParam() {};
    virtual ~SwapParam() {};

    virtual std::string toString()
    {
        char buffer[50];
        int result = -1;
        if (isBack_) {
            result = sprintf(buffer, "Swap: (%d, %d) -> (%d, %d)", endX_, endY_, startX_, startY_);
        } else {
            result = sprintf(buffer, "Swap: (%d, %d) -> (%d, %d)", startX_, startY_, endX_, endY_);
        }
        if (result < 0) {
            return SpcialTestObject::toString();
        }
        return std::string(buffer);
    }
    int startX_;
    int startY_;
    int endX_;
    int endY_;
    bool isGoBack_;
    bool isBack_;
};
class TouchParam : public SpcialTestObject {
public:
    TouchParam() {};
    virtual ~TouchParam() {};
    virtual std::string toString()
    {
        char buffer[50];
        int result = sprintf(buffer, "Point: (%d, %d)", x_, y_);
        if (result < 0) {
            return SpcialTestObject::toString();
        }
        return std::string(buffer);
    }
    int x_;
    int y_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif
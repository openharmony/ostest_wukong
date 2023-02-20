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

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include "securec.h"

namespace OHOS {
namespace WuKong {
class SpcialTestObject {
public:
    SpcialTestObject()
    {
    }
    virtual ~SpcialTestObject()
    {
    }
    // convert coordinates to string
    virtual std::string toString()
    {
        return std::to_string(objectType_);
    }
    int objectType_ = -1;
    bool isAllFinished_ = false;
};
class SwapParam : public SpcialTestObject {
public:
    SwapParam()
    {
    }
    virtual ~SwapParam()
    {
    }

    virtual std::string toString()
    {
        char buffer[50];
        int result = -1;
        if (isBack_) {
            result = sprintf_s(buffer, sizeof(buffer), "Swap: (%d, %d) -> (%d, %d)", endX_, endY_, startX_, startY_);
        } else {
            result = sprintf_s(buffer, sizeof(buffer), "Swap: (%d, %d) -> (%d, %d)", startX_, startY_, endX_, endY_);
        }
        if (result < 0) {
            return SpcialTestObject::toString();
        }
        return std::string(buffer);
    }
    int startX_ = -1;
    int startY_ = -1;
    int endX_ = -1;
    int endY_ = -1;
    bool isGoBack_ = false;
    bool isBack_ = false;
};
class TouchParam : public SpcialTestObject {
public:
    TouchParam()
    {
    }
    virtual ~TouchParam()
    {
    }
    virtual std::string toString()
    {
        char buffer[50];
        int result = sprintf_s(buffer, sizeof(buffer), "Point: (%d, %d)", x_, y_);
        if (result < 0) {
            return SpcialTestObject::toString();
        }
        return std::string(buffer);
    }
    int x_ = -1;
    int y_ = -1;
};
class AppSwitchParam : public SpcialTestObject {
public:
    AppSwitchParam()
    {
    }
    virtual ~AppSwitchParam()
    {
    }
    virtual std::string toString()
    {
        char buffer[50];
        int result = sprintf_s(buffer, sizeof(buffer), "Bundlename: (%s)", bundlename_.c_str());
        if (result < 0) {
            return SpcialTestObject::toString();
        }
        return std::string(buffer);
    }
    std::string bundlename_ = " ";
};
class ComponentParam : public SpcialTestObject {
public:
    ComponentParam()
    {
        bundleName_.clear();
        bundleRunning_.clear();
        bundleFinish_.clear();
    }
    virtual ~ComponentParam()
    {
    }
    virtual std::string toString()
    {
        std::stringstream ss;
        ss << "Bundle : ";
        for (uint32_t index = 0; index < bundleName_.size(); index++) {
            ss << "[" << bundleName_[index] << ":" << bundleRunning_[index] << ":" << bundleFinish_[index] << "]";
        }
        ss << "AllFinished: " << isAllFinished_;
        return ss.str();
    }

    void PushBundleName(const std::string& name)
    {
        bundleName_.push_back(name);
        bundleRunning_.push_back(false);
        bundleFinish_.push_back(false);
        pageBack_.push_back(0);
        lanuchCount_.push_back(0);
    }
    std::vector<std::string> bundleName_;
    std::vector<bool> bundleRunning_;
    std::vector<bool> bundleFinish_;
    std::vector<uint32_t> pageBack_;
    std::vector<uint32_t> lanuchCount_;
};
class RecordParam : public SpcialTestObject {
public:
    RecordParam()
    {
    }
    virtual ~RecordParam()
    {
    }
    std::string recordName_;
    bool recordStatus_ = false;
};
}  // namespace WuKong
}  // namespace OHOS
#endif

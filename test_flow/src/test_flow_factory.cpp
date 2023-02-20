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

#include "test_flow_factory.h"

#include "random_test_flow.h"
#include "special_test_flow.h"

namespace OHOS {
namespace WuKong {
std::shared_ptr<TestFlow> TestFlowFactory::GetTestFlow(WuKongShellCommand& shellCommand, const std::string& command)
{
    std::shared_ptr<TestFlow> testFlow = nullptr;
    if (command == "exec") {
        testFlow = std::make_shared<RandomTestFlow>(shellCommand);
    } else if (command == "special") {
        testFlow = std::make_shared<SpecialTestFlow>(shellCommand);
    }
    return testFlow;
}
}  // namespace WuKong
}  // namespace OHOS

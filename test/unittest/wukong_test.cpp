/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <thread>
#include <iostream>
#include <gtest/gtest.h>

using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace WuKong {

bool ExecCmd(const std::string &cmd, std::string &resvec)
{
    resvec.clear();
    FILE *pp = popen(cmd.c_str(), "r");
    if (!pp) {
        return -1;
    }
    char tmp[1024];
    while (fgets(tmp, sizeof(tmp), pp) != nullptr) {
        if (tmp[strlen(tmp) - 1] == '\n') {
            tmp[strlen(tmp) - 1] = '\0';
        }
        resvec.append(tmp);
    }
    pclose(pp);
    return resvec.size() >= 0 ? true : false;
}

class WuKongTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}

    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: RandomTestCase
 * @tc.desc: Test wukong basic function
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, RandomTestCase, TestSize.Level1)
{
    std::string cmd = "wukong exec -T 1";
    std::string result = "";
    bool flag = false;
    auto ret = ExecCmd(cmd, result);
    std::string::size_type strOne = result.find("all test Finished");
    std::string::size_type strTwo = result.find("catching stopped");
    if ((strOne != result.npos) && (strTwo != result.npos)) {
        flag = true;
    }

    EXPECT_EQ(ret, true);
    EXPECT_EQ(flag, true);
}


} // namespace WuKong
} //namespace OHOS
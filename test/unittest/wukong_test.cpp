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

#include "wukong_util.h"

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
    static void TearDownTestCase()
    {
        WuKongUtil::DestroyInstance();
    }

    void SetUp()
    {
        WuKongUtil::GetInstance()->ClearBlockRect();
    }
    void TearDown() {}
};

/**
 * @tc.name: RandomTestCase
 * @tc.desc: Test wukong basic function
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, RandomTestCase, TestSize.Level0)
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

/**
 * @tc.name: WuKongFuncTest1
 * @tc.desc: Test wukong appinfo
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, WuKongFuncTest1, TestSize.Level1)
{
    std::string cmd = "wukong appinfo";
    std::string result = "";
    auto ret = ExecCmd(cmd, result);
    EXPECT_EQ(ret, true);
    EXPECT_EQ(result.size() > 0, true);
}


/**
 * @tc.name: BlockRectStoreTest
 * @tc.desc: AC-1.1 legal -X x1,y1,x2,y2 accepted and stored in WuKongUtil
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectStoreTest, TestSize.Level0)
{
    auto util = WuKongUtil::GetInstance();
    ASSERT_NE(util, nullptr);
    auto ret = util->SetBlockRectList("1,2,3,4");
    EXPECT_EQ(ret, OHOS::ERR_OK);
    const auto &list = util->GetBlockRectList();
    ASSERT_FALSE(list.empty());
    EXPECT_EQ(list[0].x1, 1);
    EXPECT_EQ(list[0].y1, 2);
    EXPECT_EQ(list[0].x2, 3);
    EXPECT_EQ(list[0].y2, 4);
    EXPECT_EQ(util->HasBlockRect(), true);
}

/**
 * @tc.name: BlockRectIllegalFormatTest
 * @tc.desc: AC-1.2 illegal -X formats rejected with ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectIllegalFormatTest, TestSize.Level0)
{
    auto util = WuKongUtil::GetInstance();
    ASSERT_NE(util, nullptr);
    EXPECT_EQ(util->SetBlockRectList("1,2,3"), OHOS::ERR_INVALID_VALUE);            // 3 segments
    EXPECT_EQ(util->SetBlockRectList("1,2,3,4,5"), OHOS::ERR_INVALID_VALUE);         // 5 segments
    EXPECT_EQ(util->SetBlockRectList("a,b,c,d"), OHOS::ERR_INVALID_VALUE);          // non-integer
    EXPECT_EQ(util->SetBlockRectList("-1,2,3,4"), OHOS::ERR_INVALID_VALUE);          // negative
    EXPECT_EQ(util->SetBlockRectList("1.5,2,3,4"), OHOS::ERR_INVALID_VALUE);        // decimal
    EXPECT_EQ(util->SetBlockRectList("99999999999,2,3,4"), OHOS::ERR_INVALID_VALUE); // overflow
}

/**
 * @tc.name: BlockRectClearTest
 * @tc.desc: ClearBlockRect resets block state (test isolation)
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectClearTest, TestSize.Level0)
{
    auto util = WuKongUtil::GetInstance();
    ASSERT_NE(util, nullptr);
    EXPECT_EQ(util->SetBlockRectList("1,2,3,4"), OHOS::ERR_OK);
    EXPECT_EQ(util->HasBlockRect(), true);
    util->ClearBlockRect();
    EXPECT_EQ(util->HasBlockRect(), false);
    EXPECT_TRUE(util->GetBlockRectList().empty());
}

/**
 * @tc.name: BlockRectCalcAvoidPointTest
 * @tc.desc: AC-1.4 random touch point strictly avoids the block rect (interception 100%)
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectCalcAvoidPointTest, TestSize.Level0)
{
    const int screenW = 1000;
    const int screenH = 1000;
    BlockRect block;
    block.x1 = 100;
    block.y1 = 200;
    block.x2 = 300;
    block.y2 = 400;
    for (int i = 0; i < 500; ++i) {
        int x = -1;
        int y = -1;
        auto ret = WuKongUtil::CalcBlockAvoidPoint(screenW, screenH, block, x, y);
        EXPECT_EQ(ret, OHOS::ERR_OK);
        bool inBlock = (x >= block.x1 && x < block.x2 && y >= block.y1 && y < block.y2);
        EXPECT_FALSE(inBlock);
        EXPECT_GE(x, 0);
        EXPECT_LT(x, screenW);
        EXPECT_GE(y, 0);
        EXPECT_LT(y, screenH);
    }
}

/**
 * @tc.name: BlockRectClampTest
 * @tc.desc: AC-1.5 block beyond screen is clamped to screen boundary
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectClampTest, TestSize.Level0)
{
    const int screenW = 1000;
    const int screenH = 1000;
    BlockRect block;
    block.x1 = 100;
    block.y1 = 200;
    block.x2 = 2000;
    block.y2 = 2000;
    for (int i = 0; i < 200; ++i) {
        int x = -1;
        int y = -1;
        auto ret = WuKongUtil::CalcBlockAvoidPoint(screenW, screenH, block, x, y);
        EXPECT_EQ(ret, OHOS::ERR_OK);
        EXPECT_GE(x, 0);
        EXPECT_LT(x, screenW);
        EXPECT_GE(y, 0);
        EXPECT_LT(y, screenH);
    }
}

/**
 * @tc.name: BlockRectFullCoverSkipTest
 * @tc.desc: AC-1.6 block covers whole screen, available area 0, skip this round
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectFullCoverSkipTest, TestSize.Level0)
{
    const int screenW = 1000;
    const int screenH = 1000;
    BlockRect block;
    block.x1 = 0;
    block.y1 = 0;
    block.x2 = screenW;
    block.y2 = screenH;
    int x = -1;
    int y = -1;
    auto ret = WuKongUtil::CalcBlockAvoidPoint(screenW, screenH, block, x, y);
    EXPECT_EQ(ret, OHOS::ERR_INVALID_VALUE);
}

/**
 * @tc.name: BlockRectTouchDepTest
 * @tc.desc: AC-1.3 -X without -t is rejected (binding, no ordering restriction), test must not start
 * @tc.type: FUNC
 */
HWTEST_F(WuKongTest, BlockRectTouchDepTest, TestSize.Level1)
{
    std::string cmd = "wukong exec -X 100,200,300,400 -T 1";
    std::string result = "";
    auto ret = ExecCmd(cmd, result);
    EXPECT_EQ(ret, true);
    std::string::size_type pos = result.find("all test Finished");
    EXPECT_EQ(pos == result.npos, true);
}


} // namespace WuKong
} //namespace OHOS
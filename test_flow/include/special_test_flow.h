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

#ifndef TEST_WUKONG_SPECIAL_TEST_H
#define TEST_WUKONG_SPECIAL_TEST_H

#include <string>

#include "input_factory.h"
#include "special_test_object.h"
#include "test_flow.h"

namespace OHOS {
namespace WuKong {
class SpecialTestFlow : public TestFlow {
public:
    SpecialTestFlow(WuKongShellCommand &shellcommand);
    virtual ~SpecialTestFlow();

    /**
     * @brief set the params of special test to special object
     * @return Return ERR_OK on success, others on failure.
     */
    virtual ErrCode EnvInit() override;

    /**
     * @brief run the test
     * @return Return ERR_OK on success, others on failure.
     */
    virtual ErrCode RunStep() override;

private:
    virtual const struct option *GetOptionArguments(std::string &shortOpts) override;
    virtual ErrCode HandleUnknownOption(const char optopt) override;
    virtual ErrCode HandleNormalOption(const int option) override;

    /**
     * @brief check if the '-c' and 'T' is exist at the same time
     * @param option command letter.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode CheckArgument(const int option);

    /**
     * @brief registered timer to monitor test time
     */
    void RegisterTimer();

    /**
     * @brief the callback function of the timer
     */
    void TestTimeout();

    /**
     * @brief set the type of input accord to arguments
     * @return the type of input,refer to [InputType] .
     */
    InputType DistrbuteInputType();

    /**
     * @brief check if the touch params or swap params is valid
     * @param argumentlist the list of touch params or swap params
     * @return return ERR_OK is valid, others is invalid.
     */
    ErrCode CheckPosition(std::vector<std::string> argumentlist);

    /**
     * @brief launcher app of component event
     * @return return ERR_OK is valid, others is invalid.
     */
    ErrCode LauncherApp();

    uint32_t timerId_ = 0;
    int countArgs_ = 10;

    // the interval time of test
    int intervalArgs_ = 1500;

    // the total count of test
    int totalCount_ = 10;

    // the total time of test
    float totalTime_ = 10.0;

    std::string specialRecordName_;

    // the list of touch params
    std::vector<std::string> touchParam_;

    // the list of the start point of swap
    std::vector<std::string> swapStartPoint_;

    // the bundle of component event
    std::vector<std::string> bundleName_;

    // the list of the end point of swap
    std::vector<std::string> swapEndPoint_;
    std::shared_ptr<Utils::Timer> timer_ = nullptr;
    std::shared_ptr<SpcialTestObject> specialTestObject_ = nullptr;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_SPECIAL_TEST_H

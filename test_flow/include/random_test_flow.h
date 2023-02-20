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

#ifndef TEST_WUKONG_RANDOM_TEST_H
#define TEST_WUKONG_RANDOM_TEST_H

#include <string>

#include "input_factory.h"
#include "test_flow.h"

namespace OHOS {
namespace WuKong {
class RandomTestFlow : public TestFlow {
public:
    RandomTestFlow(WuKongShellCommand &shellcommand);
    virtual ~RandomTestFlow();

    /**
     * @brief to confirm the input of event percent is valid or not
     * and distribute the event count randomly according to possibility.
     * @return Return ERR_OK on success, others on failure.
     */
    virtual ErrCode EnvInit() override;

    /**
     * @brief run the test according to shellCommand, output seed value, exec
     * count and interval value.
     * @return Return ERR_OK on success, others on failure.
     */
    virtual ErrCode RunStep() override;

private:
    virtual const struct option* GetOptionArguments(std::string &shortOpts) override;
    ErrCode HandleUnknownOption(const char optopt) override;
    ErrCode HandleNormalOption(const int option) override;

    /**
     * @brief check if the '-c' and 'T' is exist at the same time
     * @param option command letter.
     * @return Return ERR_OK on success, others on failure.
     */
    ErrCode CheckArgument(const int option);

    /**
     * @brief init event list percent.
     * @return return ERR_OK on success, others on failure.
     */
    ErrCode InitEventPercent();
    /**
     * @brief shuffle the elements in event list
     */
    void RandomShuffle();

    /**
     * @brief registered timer to monitor test time
     */
    void RegisterTimer();

    /**
     * @brief the callback function of the timer
     */
    void TestTimeout();

    /**
     * @brief set number of input type
     * @param option the param of test
     */
    ErrCode SetInputPercent(const int option);

    ErrCode InputScene(std::shared_ptr<InputAction> inputaction, bool inputFlag);

    bool SetBlockPage();

    int countArgs_ = 10;

    // the interval time of test
    int intervalArgs_ = 1500;

    // the seed value of test
    int seedArgs_ = -1;

    // the total count of test
    int totalCount_ = 10;

    // the total time of test
    float totalTime_ = 10.0;

    std::shared_ptr<Utils::Timer> timer_ = nullptr;
    uint32_t timerId_ = 0;
    std::vector<int> inputPercent_;
    std::vector<int> eventList_;
};
}  // namespace WuKong
}  // namespace OHOS
#endif  // TEST_WUKONG_RANDOM_TEST_H

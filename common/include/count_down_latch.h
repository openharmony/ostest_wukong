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

#ifndef COUNT_DOWN_LATCH_H
#define COUNT_DOWN_LATCH_H
#include <mutex>
#include <condition_variable>
#include <chrono>
namespace OHOS {
namespace WuKong {
class CountDownLatch {
    public:
        CountDownLatch(int count);

        void countDown();

        bool await(std::chrono::milliseconds timeout);
  
    private:
        int count_;
        bool done_;
        std::mutex mtx_;
        std::condition_variable cv_;
        CountDownLatch();
    };
}  // namespace WuKong
}  // namespace OHOS
#endif  // COUNT_DOWN_LATCH_H
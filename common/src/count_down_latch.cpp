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
#include "count_down_latch.h"

#include <mutex>
#include <condition_variable>
#include <chrono>

namespace OHOS {
namespace WuKong {
CountDownLatch::CountDownLatch(int count) : count_(count), done_(false), mtx_(), cv_() {}
void CountDownLatch::countDown()
{
        std::unique_lock<std::mutex> lock(mtx_);
        if (--count_ == 0) {
            done_ = true;
            cv_.notify_all();
        }
}

bool CountDownLatch::await(std::chrono::milliseconds timeout)
{
        std::unique_lock<std::mutex> lock(mtx_);
        if (cv_.wait_for(lock, timeout, [this] { return done_ || count_ == 0; })) {
            return true;
        } else {
            return false;
        }
}

}  // namespace WuKong
}  // namespace OHOS
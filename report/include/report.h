/*
 * @Description:
 * @Date: 2022-04-21 19:11:57
 * @LastEditTime: 2022-04-21 20:23:01
 * @FilePath: /wukong/report/include/report.h
 * @author: lemon
 */
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

#ifndef TEST_WUKONG_REPORT
#define TEST_WUKONG_REPORT
#include "data_unit.h"
#include "wukong_define.h"
namespace OHOS {
namespace WuKong {
enum ReportType { RandomTest = 0, SpecialTest };
class Report final {
public:
    static Report &GetInstance();
    ErrCode Init();
    ErrCode Finish();

private:
    Report() = default;
    ~Report() = default;
};
}  // namespace WuKong
}  // namespace OHOS

#endif
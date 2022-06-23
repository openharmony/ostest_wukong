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

#include "filter_category.h"

namespace OHOS {
namespace WuKong {
FilterCategory::FilterCategory() : Filter()
{
}

FilterCategory::~FilterCategory()
{
}

void FilterCategory::FilterDetail(std::string type, std::map<std::string, std::string> data,
                                  std::vector<std::map<std::string, std::string>> &dataVector)
{
    DEBUG_LOG_STR("filter by category{%s}", type.c_str());
    std::map<std::string, std::string>::iterator it = data.find(type);
    if ((it != data.end()) && (!it->second.empty())) {
        DEBUG_LOG("record data");
        dataVector.push_back(data);
    }
}
}  // namespace WuKong
}  // namespace OHOS

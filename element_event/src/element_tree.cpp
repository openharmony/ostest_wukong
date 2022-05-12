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

#include "element_tree.h"

#include "accessibility_ui_test_ability.h"
#include "wukong_define.h"

namespace OHOS {
namespace WuKong {
ElementTree::ElementTree() : WuKongTree() {}
ElementTree::~ElementTree() {}
ErrCode ElementTree::GetInjectTimes()
{
    return OHOS::ERR_OK;
}
ErrCode ElementTree::GetSupportEvents()
{
    return OHOS::ERR_OK;
}
ErrCode ElementTree::AddInjectEventInfoList()
{
    return OHOS::ERR_OK;
}
ErrCode ElementTree::GetRoot()
{
    return OHOS::ERR_OK;
}
ErrCode ElementTree::SetChildList()
{
    return OHOS::ERR_OK;
}
ErrCode ElementTree::GetChildList()
{
    return OHOS::ERR_OK;
}
ErrCode ElementTree::PreOrderList()
{
    return OHOS::ERR_OK;
}
}  // namespace WuKong
}  // namespace OHOS

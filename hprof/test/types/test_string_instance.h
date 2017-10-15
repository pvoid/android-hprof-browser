///
///  Copyright 2017 Dmitry "PVOID" Petukhov
///
///  Licensed under the Apache License, Version 2.0 (the "License");
///  you may not use this file except in compliance with the License.
///  You may obtain a copy of the License at
///
///      http://www.apache.org/licenses/LICENSE-2.0
///
///  Unless required by applicable law or agreed to in writing, software
///  distributed under the License is distributed on an "AS IS" BASIS,
///  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
///  See the License for the specific language governing permissions and
///  limitations under the License.
///
#pragma once

#include <gtest/gtest.h>

#include "mocks.h"
#include "types/string_instance.h"

using namespace hprof;

TEST(string_info_impl_t, When_DefaultValue_Expect_EmptyValue) {
    mock_objects_index_t objects;
    auto text = string_info_impl_t::create(*instance_info_impl_t::create(4, 0xc0f060, 0), objects);
    ASSERT_EQ("", text->value());
}

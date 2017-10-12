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

#include "types/instance.h"

using namespace hprof;

TEST(instance_info_impl_t, When_Always_Expect_TypeIsInstance) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    ASSERT_EQ(object_info_t::TYPE_INSTANCE, instance->type());
}

TEST(instance_info_impl_t, When_ClassIdIsNotSet_Expect_Return0) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    ASSERT_EQ(0, instance->class_id());
}

TEST(instance_info_impl_t, When_SetClassId300_Expect_ReturnSameClassId300) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    instance->set_class_id(300);
    ASSERT_EQ(300, instance->class_id());
}

TEST(class_info_impl_t, When_ClassIsNotSet_Expect_ReturnNull) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    ASSERT_EQ(nullptr, instance->get_class());
}

TEST(class_info_impl_t, When_ClassSuper_Expect_ReturnSameinstance) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    instance->set_class(cls);
    ASSERT_EQ(cls.get(), instance->get_class());
}

TEST(instance_info_impl_t, When_StackTraceIdDefaultValue_Expect_Return0) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    ASSERT_EQ(0, instance->stack_trace_id());
}

TEST(instance_info_impl_t, When_StackTraceIdIsSetted_Expect_ReturnSameValue) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    instance->set_stack_trace_id(2000);
    ASSERT_EQ(2000, instance->stack_trace_id());
}

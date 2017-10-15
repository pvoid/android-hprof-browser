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

#include "types/class.h"
#include "types/instance.h"

#include "mocks.h"

using namespace hprof;
using testing::Return;
using testing::ReturnRef;
using testing::_;

TEST(instance_info_impl_t, When_ClassIdIsNotSet_Expect_Return0) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    ASSERT_EQ(0, instance->class_id());
}

TEST(instance_info_impl_t, When_SetClassId300_Expect_ReturnSameClassId300) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    instance->set_class_id(300);
    ASSERT_EQ(300, instance->class_id());
}

TEST(instance_info_impl_t, When_ClassIsNotSet_Expect_ReturnNull) {
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    ASSERT_EQ(nullptr, instance->get_class());
}

TEST(instance_info_impl_t, When_ClassSuper_Expect_ReturnSameinstance) {
    mock_fields_iterator_t iterator;
    mock_fields_iterator_helper_t helper { &iterator };

    EXPECT_CALL(iterator, not_equals(_)).Times(1).WillOnce(Return(false));

    mock_fields_spec_t fields;
    EXPECT_CALL(fields, begin()).Times(1).WillOnce(Return(helper));
    EXPECT_CALL(fields, end()).Times(1).WillOnce(Return(helper));

    mock_class_info_t cls;
    EXPECT_CALL(cls, fields()).Times(2).WillRepeatedly(ReturnRef(fields));
    EXPECT_CALL(cls, super()).Times(1).WillOnce(Return(nullptr));
    
    auto item = std::make_shared<mock_heap_item_t>();
    EXPECT_CALL(*item, as_class()).Times(2).WillRepeatedly(Return(&cls));
    
    auto instance = instance_info_impl_t::create(4, 0xc0f060, 0);
    instance->set_class(item);
    ASSERT_EQ(&cls, instance->get_class());
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

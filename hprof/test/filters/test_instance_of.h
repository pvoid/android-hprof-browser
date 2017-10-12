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

#include "filters/base.h"
#include "filters/instance_of.h"

#include "mocks.h"

using namespace hprof;

TEST(filter_instance_of_t, When_nullptr_Expect_NoMatch) {
    mock_objects_index_t objects;
    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::NoMatch, filter(nullptr, objects));
}

TEST(filter_instance_of_t, When_NotInstance_Expect_NoMatch) {
    mock_objects_index_t objects;
    mock_primitives_array_t array;
    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, objects));
}

TEST(filter_instance_of_t, When_DirectClassInstance_Expect_Match) {
    mock_objects_index_t objects;
    mock_instance_info_t instance;
    mock_class_info_t cls;
    std::string class_name { "com.android.View" };

    EXPECT_CALL(instance, get_class()).Times(1).WillOnce(Return(&cls));
    EXPECT_CALL(cls, name()).Times(1).WillOnce(ReturnRef(class_name));

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::Match, filter(&instance, objects));
}

TEST(filter_instance_of_t, When_SubClassInstance_Expect_Match) {
    mock_objects_index_t objects;
    mock_instance_info_t instance;
    mock_class_info_t super_cls;
    mock_class_info_t cls;
    std::string super_class_name { "com.android.View" };
    std::string class_name { "com.android.ViewGroup" };

    EXPECT_CALL(cls, name()).Times(1).WillOnce(ReturnRef(class_name));
    EXPECT_CALL(super_cls, name()).Times(1).WillOnce(ReturnRef(super_class_name));

    EXPECT_CALL(instance, get_class()).Times(1).WillOnce(Return(&cls));
    EXPECT_CALL(cls, super()).Times(1).WillOnce(Return(&super_cls));

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::Match, filter(&instance, objects));
}

TEST(filter_instance_of_t, When_NotInstanceOfClass_Expect_NoMatch) {
    mock_objects_index_t objects;
    mock_instance_info_t instance;
    mock_class_info_t cls;
    std::string class_name { "android.app.Context" };

    EXPECT_CALL(instance, get_class()).Times(1).WillOnce(Return(&cls));
    EXPECT_CALL(cls, name()).Times(1).WillOnce(ReturnRef(class_name));
    EXPECT_CALL(cls, super()).Times(1).WillOnce(Return(nullptr));

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, objects));
}

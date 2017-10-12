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
#include <gmock/gmock.h>

#include "filters/classname.h"
#include "mocks.h"

using namespace hprof;
using testing::ReturnRef;
using testing::Return;
using testing::Eq;

TEST(filter_classname_t, When_ClassAndDistanceIsZero_Expect_Match) {
    std::string class_name { "com.android.View" };
    mock_class_info_t cls;
    mock_objects_index_t objects;
    EXPECT_CALL(cls, name_matches(name_tokens("com.android"))).Times(1).WillOnce(Return(0));

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::Match, filter(&cls, objects));
}

TEST(filter_classname_t, When_ClassAndDistanceIsNotZero_Expect_NoMatch) {
    std::string class_name { "com.android.View" };
    mock_class_info_t cls;
    mock_objects_index_t objects;
    EXPECT_CALL(cls, name_matches(name_tokens("com.android"))).Times(1).WillOnce(Return(10));

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&cls, objects));
}

TEST(filter_classname_t, When_PrimitiveArray_Expect_NoMatch) {
    mock_primitives_array_t array {};
    mock_objects_index_t objects;

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, objects));
}

TEST(filter_classname_t, When_ObjectsArray_Expect_NoMatch) {
    mock_objects_array_t array {};
    mock_objects_index_t objects;

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, objects));
}

TEST(filter_classname_t, When_InstanceAndDistanceIsZero_Expect_Match) {
    std::string class_name { "com.android.View" };
    mock_instance_info_t instance;
    mock_class_info_t cls;
    mock_objects_index_t objects;

    EXPECT_CALL(instance, get_class()).Times(1).WillOnce(Return(&cls));
    EXPECT_CALL(cls, name_matches(name_tokens("com.android"))).Times(1).WillOnce(Return(0));

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::Match, filter(&instance, objects));
}

TEST(filter_classname_t, When_InstanceAndDistanceIsNotZero_Expect_Match) {
    std::string class_name { "com.android.View" };
    mock_instance_info_t instance;
    mock_class_info_t cls;
    mock_objects_index_t objects;

    EXPECT_CALL(instance, get_class()).Times(1).WillOnce(Return(&cls));
    EXPECT_CALL(cls, name_matches(name_tokens("com.android"))).Times(1).WillOnce(Return(10));

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, objects));
}

TEST(filter_classname_t, When_NotStringClassName_Expect_NoMatch) {
    mock_string_info_t str;
    mock_objects_index_t objects;

    filter_classname_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&str, objects));
}

TEST(filter_classname_t, When_StringClassName_Expect_Match) {
    mock_string_info_t str;
    mock_objects_index_t objects;

    filter_classname_t filter { "java.lang.String" };
    ASSERT_EQ(filter_t::Match, filter(&str, objects));
}
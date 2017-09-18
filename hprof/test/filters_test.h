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

#include "filters.h"
#include "filter_mocks.h"

using namespace hprof;

TEST(Filters, FilterNot) {
    ASSERT_EQ(filter_t::NoMatch, apply_filter<filter_not_t>(filter_t::Match));
    ASSERT_EQ(filter_t::Match, apply_filter<filter_not_t>(filter_t::NoMatch));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_not_t>(filter_t::Fail));
}

TEST(Filters, FilterAnd) {
    ASSERT_EQ(filter_t::Match, apply_filter<filter_and_t>(filter_t::Match, filter_t::Match));
    ASSERT_EQ(filter_t::NoMatch, apply_filter<filter_and_t>(filter_t::Match, filter_t::NoMatch));
    ASSERT_EQ(filter_t::NoMatch, apply_filter<filter_and_t>(filter_t::NoMatch, filter_t::Match));
    ASSERT_EQ(filter_t::NoMatch, apply_filter<filter_and_t>(filter_t::NoMatch, filter_t::NoMatch));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_and_t>(filter_t::Match, filter_t::Fail));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_and_t>(filter_t::NoMatch, filter_t::Fail));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_and_t>(filter_t::Fail, filter_t::Match));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_and_t>(filter_t::Fail, filter_t::NoMatch));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_and_t>(filter_t::Fail, filter_t::Fail));
}

TEST(Filters, FilterOr) {
    ASSERT_EQ(filter_t::Match, apply_filter<filter_or_t>(filter_t::Match, filter_t::Match));
    ASSERT_EQ(filter_t::Match, apply_filter<filter_or_t>(filter_t::Match, filter_t::NoMatch));
    ASSERT_EQ(filter_t::Match, apply_filter<filter_or_t>(filter_t::NoMatch, filter_t::Match));
    ASSERT_EQ(filter_t::NoMatch, apply_filter<filter_or_t>(filter_t::NoMatch, filter_t::NoMatch));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_or_t>(filter_t::Match, filter_t::Fail));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_or_t>(filter_t::NoMatch, filter_t::Fail));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_or_t>(filter_t::Fail, filter_t::Match));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_or_t>(filter_t::Fail, filter_t::NoMatch));
    ASSERT_EQ(filter_t::Fail, apply_filter<filter_or_t>(filter_t::Fail, filter_t::Fail));
}

TEST(Filters, FilterClassNameSameClass) {
    class_info_t cls;
    cls.tokens.set("com.android.View");

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::Match, filter(&cls, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameNotSameClass) {
    class_info_t cls;
    cls.tokens.set("java.lang.String");

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&cls, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameInstanceSameClass) {
    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->tokens.set("com.android.View");

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::Match, filter(&instance, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameInstanceNotSameClass) {
    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->tokens.set("java.lang.String");

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNamePrimitiveArray) {
    primitive_array_info_t array {0, 0, primitive_array_info_t::TYPE_BOOL, 0};

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameObjectArray) {
    object_array_info_t array {0, 0, 0};

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, mock_filter_helper_t {}));
}

TEST(Filters, FilterInstanceOfDirectClassMatch) {
    mock_filter_helper_t helper {};
    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->name_id = 1;
    helper.add(1, "com.android.View");

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::Match, filter(&instance, helper));
}

TEST(Filters, FilterInstanceOfSubClassMatch) {
    mock_filter_helper_t helper {};
    helper.add(1, "com.android.GroupView");
    helper.add(2, "com.android.View");

    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->class_id = 1;
    instance.class_instance->name_id = 1;
    instance.class_instance->super_id = 2;

    auto super_class = std::make_shared<class_info_t>();
    super_class->class_id = 2;
    super_class->super_id = 0;
    super_class->name_id = 2;
    helper.add(super_class);

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::Match, filter(&instance, helper));
}

TEST(Filters, FilterInstanceOfDirectClassNoMatch) {
    mock_filter_helper_t helper {};
    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->name_id = 1;
    helper.add(1, "com.android.View");

    filter_instance_of_t filter { "java.lang.String" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, helper));
}

TEST(Filters, FilterInstanceOfSubClassNoMatch) {
    mock_filter_helper_t helper {};
    helper.add(1, "com.android.GroupView");
    helper.add(2, "com.android.View");

    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->class_id = 1;
    instance.class_instance->name_id = 1;
    instance.class_instance->super_id = 2;

    auto super_class = std::make_shared<class_info_t>();
    super_class->class_id = 2;
    super_class->super_id = 0;
    super_class->name_id = 2;
    helper.add(super_class);

    filter_instance_of_t filter { "java.lang.String" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, helper));
}
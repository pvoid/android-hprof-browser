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
#include "filters_mocks.h"

using namespace hprof;

TEST(Filters, FilterClassNameSameClass) {
    class_info_impl_t cls { 4 };
    cls.set_name("com.android.View");

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::Match, filter(&cls, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameNotSameClass) {
    class_info_impl_t cls { 4 };
    cls.set_name("java.lang.String");

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&cls, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameInstanceSameClass) {
    instance_info_impl_t instance {4, 0, 0, 0, 0};
    auto class_instance = std::make_shared<class_info_impl_t>(4);
    class_instance->set_name("com.android.View");
    instance.set_class(class_instance);

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::Match, filter(&instance, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameInstanceNotSameClass) {
    instance_info_impl_t instance {4, 0, 0, 0, 0};
    auto class_instance = std::make_shared<class_info_impl_t>(4);
    class_instance->set_name("java.lang.String");
    instance.set_class(class_instance);

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNamePrimitiveArray) {
    primitive_array_info_impl_t array {4, 0, 0, JVM_TYPE_BOOL, 0};

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameObjectArray) {
    object_array_info_impl_t array {4, 0, 0, 0};

    filter_class_name_t filter { "com.android" };
    ASSERT_EQ(filter_t::NoMatch, filter(&array, mock_filter_helper_t {}));
}

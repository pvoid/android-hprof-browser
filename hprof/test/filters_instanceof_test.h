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

#include "filters_base.h"
#include "filters_instanceof.h"

using namespace hprof;

TEST(Filters, FilterInstanceOfDirectClassMatch) {
    mock_filter_helper_t helper {};
    instance_info_impl_t instance {4, 0, 0, 0, 0};
    auto class_instance = std::make_shared<class_info_impl_t>(4);
    class_instance->set_name_id(1);
    instance.set_class(class_instance);
    helper.add(1, "com.android.View");

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::Match, filter(&instance, helper));
}

TEST(Filters, FilterInstanceOfSubClassMatch) {
    mock_filter_helper_t helper {};
    helper.add(1, "com.android.GroupView");
    helper.add(2, "com.android.View");

    instance_info_impl_t instance {4, 0, 1, 0, 0};
    auto class_instance = std::make_shared<class_info_impl_t>(4);
    class_instance->set_class_id(1);
    class_instance->set_name_id(1);
    class_instance->set_super_id(2);
    instance.set_class(class_instance);

    auto super_class = std::make_shared<class_info_impl_t>(4);
    super_class->set_class_id(2);
    super_class->set_super_id(0);
    super_class->set_name_id(2);
    helper.add(super_class);

    filter_instance_of_t filter { "com.android.View" };
    ASSERT_EQ(filter_t::Match, filter(&instance, helper));
}

TEST(Filters, FilterInstanceOfDirectClassNoMatch) {
    mock_filter_helper_t helper {};
    instance_info_impl_t instance {4, 0, 0, 0, 0};
    auto class_instance = std::make_shared<class_info_impl_t>(4);
    class_instance->set_name_id(1);
    instance.set_class(class_instance);
    helper.add(1, "com.android.View");

    filter_instance_of_t filter { "java.lang.String" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, helper));
}

TEST(Filters, FilterInstanceOfSubClassNoMatch) {
    mock_filter_helper_t helper {};
    helper.add(1, "com.android.GroupView");
    helper.add(2, "com.android.View");

    instance_info_impl_t instance {4, 0, 1, 0, 0};
    auto class_instance = std::make_shared<class_info_impl_t>(4);
    class_instance->set_class_id(1);
    class_instance->set_name_id(1);
    class_instance->set_super_id(2);
    instance.set_class(class_instance);

    auto super_class = std::make_shared<class_info_impl_t>(4);
    super_class->set_class_id(2);
    super_class->set_super_id(0);
    super_class->set_name_id(2);
    helper.add(super_class);

    filter_instance_of_t filter { "java.lang.String" };
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, helper));
}

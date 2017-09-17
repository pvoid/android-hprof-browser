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

using namespace hprof;

class filter_mock_t : public filter_t {
public:
    explicit filter_mock_t(filter_t::filter_result_t value) : _value(value) {}
    virtual ~filter_mock_t() {}

    virtual filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
        return _value;
    }
private:
    filter_t::filter_result_t _value;
};

class mock_filter_helper_t : public filter_helper_t {
public:
    virtual class_info_ptr_t get_class_by_id(hprof::id_t id) const override {
        return std::make_shared<class_info_t>();
    }
};

template <typename filter_type>
inline filter_type create_filter(filter_t::filter_result_t base_value) {
    return filter_type { std::move(std::make_unique<filter_mock_t>(base_value)) };
}

template <typename filter_type>
inline filter_type create_filter(filter_t::filter_result_t base_value_left, filter_t::filter_result_t base_value_right) {
    return filter_type { std::move(std::make_unique<filter_mock_t>(base_value_left)),
        std::move(std::make_unique<filter_mock_t>(base_value_right))};
}

template <typename filter_type, typename... mock_results>
inline filter_t::filter_result_t apply_filter(mock_results... base_values) {
    class_info_t instance {};
    return create_filter<filter_type>(base_values...)(&instance, mock_filter_helper_t {});
}

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
    filter_class_name_t filter { "com.android" };
    class_info_t cls;
    cls.tokens.set("com.android.View");
    ASSERT_EQ(filter_t::Match, filter(&cls, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameNotSameClass) {
    filter_class_name_t filter { "com.android" };
    class_info_t cls;
    cls.tokens.set("java.lang.String");
    ASSERT_EQ(filter_t::NoMatch, filter(&cls, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameInstanceSameClass) {
    filter_class_name_t filter { "com.android" };
    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->tokens.set("com.android.View");
    ASSERT_EQ(filter_t::Match, filter(&instance, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameInstanceNotSameClass) {
    filter_class_name_t filter { "com.android" };
    instance_info_t instance {0, 0, 0, 0};
    instance.class_instance = std::make_shared<class_info_t>();
    instance.class_instance->tokens.set("java.lang.String");
    ASSERT_EQ(filter_t::NoMatch, filter(&instance, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNamePrimitiveArray) {
    filter_class_name_t filter { "com.android" };
    primitive_array_info_t array {0, 0, primitive_array_info_t::TYPE_BOOL, 0};
    ASSERT_EQ(filter_t::NoMatch, filter(&array, mock_filter_helper_t {}));
}

TEST(Filters, FilterClassNameObjectArray) {
    filter_class_name_t filter { "com.android" };
    object_array_info_t array {0, 0, 0};
    ASSERT_EQ(filter_t::NoMatch, filter(&array, mock_filter_helper_t {}));
}

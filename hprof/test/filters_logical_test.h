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
#include "filters_logical.h"

#include "filters_mocks.h"

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

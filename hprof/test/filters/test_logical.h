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
#include "filters/logical.h"

#include "mocks.h"

using namespace hprof;

using testing::_;
using testing::Return;

TEST(filter_not_t, When_Match_Expect_NoMatch) {
    auto filter = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));
    filter_not_t filter_not { std::move(filter) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::NoMatch, filter_not(item, objects));
}

TEST(filter_not_t, When_NoMatch_Expect_Match) {
    auto filter = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));
    filter_not_t filter_not { std::move(filter) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Match, filter_not(item, objects));
}

TEST(filter_not_t, When_Fail_Expect_Fail) {
    auto filter = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));
    filter_not_t filter_not { std::move(filter) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_not(item, objects));
}

TEST(filter_and_t, When_MatchAndMatch_Expect_Match) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Match, filter_and(item, objects));
}

TEST(filter_and_t, When_NoMatchAndMatch_Expect_NoMatch) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::NoMatch, filter_and(item, objects));
}

TEST(filter_and_t, When_MatchAndNoMatch_Expect_NoMatch) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::NoMatch, filter_and(item, objects));
}

TEST(filter_and_t, When_NoMatchAndNoMatch_Expect_NoMatch) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::NoMatch, filter_and(item, objects));
}

TEST(filter_and_t, When_FailAndAny_Expect_Fail) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(0);


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_and(item, objects));
}

TEST(filter_and_t, When_MatchAndFail_Expect_Fail) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_and(item, objects));
}

TEST(filter_and_t, When_NoMatchAndFail_Expect_Fail) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));


    filter_and_t filter_and { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_and(item, objects));
}

TEST(filter_or_t, When_MatchOrMatch_Expect_Match) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Match, filter_or(item, objects));
}

TEST(filter_or_t, When_MatchOrNoMatch_Expect_Match) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Match, filter_or(item, objects));
}

TEST(filter_or_t, When_NoMatchOrMatch_Expect_Match) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Match, filter_or(item, objects));
}

TEST(filter_or_t, When_NoMatchOrNoMatch_Expect_NoMatch) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::NoMatch, filter_or(item, objects));
}

TEST(filter_or_t, When_FailOrAny_Expect_Fail) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(0);


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_or(item, objects));
}

TEST(filter_or_t, When_MatchOrFail_Expect_Fail) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Match));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_or(item, objects));
}

TEST(filter_or_t, When_NoMatchOrFail_Expect_Fail) {
    auto filter_left = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_left, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::NoMatch));

    auto filter_right = std::make_unique<mock_filter_t>();
    EXPECT_CALL(*filter_right, apply_filter(_, _)).Times(1).WillOnce(Return(filter_t::Fail));


    filter_or_t filter_or { std::move(filter_left), std::move(filter_right) };
    auto item = std::make_shared<mock_heap_item_t>();
    mock_objects_index_t objects;

    ASSERT_EQ(filter_t::Fail, filter_or(item, objects));
}

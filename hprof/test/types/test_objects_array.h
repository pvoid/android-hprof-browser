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

#include "types/objects_array.h"

using namespace hprof;

TEST(objects_array_info_impl_t, When_CreateWithClassIdc0c1af_Expect_ClassIdReturnsc0c1af) {
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 0, 0);
    ASSERT_EQ(0xc0c1af, instance->class_id());
}

TEST(objects_array_info_impl_t, When_CreateWithLength10_Expect_LenhthReturnsc10) {
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 10, 0);
    ASSERT_EQ(10, instance->length());
}

TEST(objects_array_info_impl_t, When_EmptyArray_Expect_BeginEqualsEns) {
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 0, 0);
    ASSERT_EQ(std::end(*instance), std::begin(*instance));
}

TEST(objects_array_info_impl_t, When_NotEmpty_Expect_BeginAlwaysEquals) {
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, 0);
    ASSERT_EQ(std::begin(*instance), std::begin(*instance));
}

TEST(objects_array_info_impl_t, When_NotEmpty_Expect_EndAlwaysEquals) {
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, 0);
    ASSERT_EQ(std::end(*instance), std::end(*instance));
}

TEST(objects_array_info_impl_t, When_NotEmpty_Expect_BeginIsNotEqualEnd) {
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, 0);
    ASSERT_NE(std::begin(*instance), std::end(*instance));
}

TEST(objects_array_info_impl_t, When_NotEmpty_Expect_IterateOverData) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(0x0F, *it);
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(0x20, *(it.operator->()));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(objects_array_info_impl_t, When_AccessByValidIndex_Expect_ValidIterator) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto item = (*instance)[1];
    ASSERT_NE(std::end(*instance), item);
    ASSERT_EQ(0x20, *item);
}

TEST(objects_array_info_impl_t, When_AccessByInvalidIndex_Expect_EndIterator) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto item = (*instance)[10];
    ASSERT_EQ(std::end(*instance), item);
}

TEST(objects_array_info_impl_t, When_HasLinkToClass_Expect_HasLinksReturnsTypeInstance) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    ASSERT_EQ(link_t::TYPE_INSTANCE, instance->has_link_to(0xc0c1af));
}

TEST(objects_array_info_impl_t, When_HasLinkToObject_Expect_HasLinksReturnsTypeOwnership) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0xc0c1af, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    ASSERT_EQ(link_t::TYPE_OWNERSHIP, instance->has_link_to(0x20));
}

TEST(objects_array_info_impl_t, When_HasLinkToClassAndObject_Expect_HasLinksReturnsTypeOwnershipAndInstance) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = objects_array_info_impl_t::create(4, 0xc0f060, 0x20, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    ASSERT_EQ(link_t::TYPE_OWNERSHIP | link_t::TYPE_INSTANCE, instance->has_link_to(0x20));
}
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

#include "types/primitives_array.h"

using namespace hprof;

TEST(primitives_array_info_impl_t, When_CreateWithTypeByte_Expect_ItemTypeReturnsByte) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 0, 0);
    ASSERT_EQ(jvm_type_t::JVM_TYPE_BYTE, instance->item_type());
}

TEST(primitives_array_info_impl_t, When_CreateWithLength10_Expect_LengthReturns10) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 10, 0);
    ASSERT_EQ(10, instance->length());
}

TEST(primitives_array_info_impl_t, When_Always_Expect_TypeIsPrimitiveArray) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 0, 0);
    ASSERT_EQ(object_info_t::TYPE_PRIMITIVES_ARRAY, instance->type());
}

TEST(primitives_array_info_impl_t, When_Always_Expect_HasLinkIs0) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 0, 0);
    ASSERT_EQ(0, instance->has_link_to(10));
}

TEST(primitives_array_info_impl_t, When_EmptyArray_Expect_BeginEqualsEns) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 0, 0);
    ASSERT_EQ(std::end(*instance), std::begin(*instance));
}

TEST(primitives_array_info_impl_t, When_NotEmpty_Expect_BeginAlwaysEquals) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 2, 0);
    ASSERT_EQ(std::begin(*instance), std::begin(*instance));
}

TEST(primitives_array_info_impl_t, When_NotEmpty_Expect_EndAlwaysEquals) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 2, 0);
    ASSERT_EQ(std::end(*instance), std::end(*instance));
}

TEST(primitives_array_info_impl_t, When_NotEmpty_Expect_BeginIsNotEqualEnd) {
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 2, 0);
    ASSERT_NE(std::begin(*instance), std::end(*instance));
}

TEST(primitives_array_info_impl_t, When_NotEmptyBoolArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x00, 0x01 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BOOL, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(false, static_cast<jvm_bool_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(true, static_cast<jvm_bool_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyByteArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x0F, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_BYTE, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(0x0F, static_cast<jvm_byte_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(0x20, static_cast<jvm_byte_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyCharArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x00, 0x0F, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_CHAR, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(0x000F, static_cast<jvm_char_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(0x0020, static_cast<jvm_char_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyShortArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x00, 0x0F, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_SHORT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(0x000F, static_cast<jvm_short_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(0x0020, static_cast<jvm_short_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyIntArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_INT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(0x0F, static_cast<jvm_int_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(0x20, static_cast<jvm_int_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyLongArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_LONG, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_EQ(0x0F, static_cast<jvm_long_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_EQ(0x20, static_cast<jvm_long_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyDoubleArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0xbF, 0xe2, 0x2A, 0xf1, 0xfe, 0x8a, 0x8a, 0x70, 0x3F, 0xe2, 0x2A, 0xf1, 0xfe, 0x8a, 0x8a, 0x70 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_DOUBLE, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_FLOAT_EQ(-0.567742345, static_cast<jvm_double_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_FLOAT_EQ(0.567742345, static_cast<jvm_double_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}

TEST(primitives_array_info_impl_t, When_NotEmptyFloatArray_Expect_IterateOverData) {
    u_int8_t data[] = { 0x3F, 0xC8, 0xAB, 0xB4, 0xbf, 0xc8, 0xab, 0xb4 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_FLOAT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto it = std::begin(*instance);
    ASSERT_NE(std::end(*instance), it);
    ASSERT_FLOAT_EQ(1.56774, static_cast<jvm_float_t>(*it));
    ASSERT_NE(std::end(*instance), ++it);
    ASSERT_FLOAT_EQ(-1.56774, static_cast<jvm_float_t>(*(it.operator->())));
    ASSERT_EQ(std::end(*instance), ++it);
}


TEST(primitives_array_info_impl_t, When_AccessByValidIndex_Expect_ValidIterator) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_INT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto item = (*instance)[1];
    ASSERT_NE(std::end(*instance), item);
    ASSERT_EQ(0x20, static_cast<jvm_int_t>(*item));
}

TEST(primitives_array_info_impl_t, When_AccessByInvalidIndex_Expect_EndIterator) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_INT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto item = (*instance)[10];
    ASSERT_EQ(std::end(*instance), item);
}

TEST(primitives_array_info_impl_t, When_ElementType_Expect_Int) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_INT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto item = instance->begin();
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, item->type());
}

TEST(primitives_array_info_impl_t, When_ElementOffset_Expect_4) {
    u_int8_t data[] = { 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x20 };
    auto instance = primitives_array_info_impl_t::create(4, 0xc0f060, jvm_type_t::JVM_TYPE_INT, 2, sizeof(data));
    std::memcpy(instance->data(), data, sizeof(data));
    auto item = instance->begin();
    ASSERT_EQ(4, (++item)->offset());
}
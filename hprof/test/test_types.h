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
#include "types.h"

#include "mocks.h"

using namespace hprof;
using ::testing::Return;
using ::testing::ByRef;
using ::testing::Eq;
using ::testing::ReturnRef;
using ::testing::Truly;
using ::testing::_;

using test_iterator_t = iterator_container_t<field_spec_t>;

TEST(jvm_type_t, When_TypeBool_Expect_SizeIs1) {
    ASSERT_EQ(1, jvm_type_t::size(jvm_type_t::JVM_TYPE_BOOL, 16));
}

TEST(jvm_type_t, When_TypeByte_Expect_SizeIs1) {
    ASSERT_EQ(1, jvm_type_t::size(jvm_type_t::JVM_TYPE_BYTE, 16));
}

TEST(jvm_type_t, When_TypeChar_Expect_SizeIs2) {
    ASSERT_EQ(2, jvm_type_t::size(jvm_type_t::JVM_TYPE_CHAR, 16));
}

TEST(jvm_type_t, When_TypeShort_Expect_SizeIs2) {
    ASSERT_EQ(2, jvm_type_t::size(jvm_type_t::JVM_TYPE_SHORT, 16));
}

TEST(jvm_type_t, When_TypeInt_Expect_SizeIs4) {
    ASSERT_EQ(4, jvm_type_t::size(jvm_type_t::JVM_TYPE_INT, 16));
}

TEST(jvm_type_t, When_TypeFloat_Expect_SizeIs4) {
    ASSERT_EQ(4, jvm_type_t::size(jvm_type_t::JVM_TYPE_FLOAT, 16));
}

TEST(jvm_type_t, When_TypeLong_Expect_SizeIs8) {
    ASSERT_EQ(8, jvm_type_t::size(jvm_type_t::JVM_TYPE_LONG, 16));
}

TEST(jvm_type_t, When_TypeDouble_Expect_SizeIs8) {
    ASSERT_EQ(8, jvm_type_t::size(jvm_type_t::JVM_TYPE_DOUBLE, 16));
}

TEST(jvm_type_t, When_TypeObject_Expect_SizeIsIdSize) {
    ASSERT_EQ(16, jvm_type_t::size(jvm_type_t::JVM_TYPE_OBJECT, 16));
}

TEST(jvm_type_t, When_TypeUnknwon_Expect_SizeIs0) {
    ASSERT_EQ(0, jvm_type_t::size(jvm_type_t::JVM_TYPE_UNKNOWN, 16));
}

TEST(jvm_type_t, When_TypeInvalid_Expect_SizeIs0) {
    ASSERT_EQ(0, jvm_type_t::size((jvm_type_t::type_spec) 800, 16));
}

TEST(jvm_type_t, When_TypeJvmBool_Expect_ConstantBool) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_BOOL, jvm_type_t::type<jvm_bool_t>());
}

TEST(jvm_type_t, When_TypeJvmByte_Expect_ConstantByte) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_BYTE, jvm_type_t::type<jvm_byte_t>());
}

TEST(jvm_type_t, When_TypeJvmChar_Expect_ConstantChar) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_CHAR, jvm_type_t::type<jvm_char_t>());
}

TEST(jvm_type_t, When_TypeJvmShort_Expect_ConstantShort) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_SHORT, jvm_type_t::type<jvm_short_t>());
}

TEST(jvm_type_t, When_TypeJvmInt_Expect_ConstantInt) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, jvm_type_t::type<jvm_int_t>());
}

TEST(jvm_type_t, When_TypeJvmFloat_Expect_ConstantFloat) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_FLOAT, jvm_type_t::type<jvm_float_t>());
}

TEST(jvm_type_t, When_TypeJvmLong_Expect_ConstantLong) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_LONG, jvm_type_t::type<jvm_long_t>());
}

TEST(jvm_type_t, When_TypeJvmDouble_Expect_ConstantDouble) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_DOUBLE, jvm_type_t::type<jvm_double_t>());
}

TEST(jvm_type_t, When_TypeJvmId_Expect_ConstantObject) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_OBJECT, jvm_type_t::type<jvm_id_t>());
}

TEST(jvm_type_t, When_NotJvmType_Expect_ConstantUnknown) {
    ASSERT_EQ(jvm_type_t::JVM_TYPE_UNKNOWN, jvm_type_t::type<hprof::time_t>());
}

TEST(field_iterator, When_AccessReference_Expect_CallMockRef) {
    mock_fields_iterator_t iter_mock {};
    mock_field_spec_t field {};
    test_iterator_t iter { mock_fields_iterator_helper_t { &iter_mock } };
    EXPECT_CALL(iter_mock, ref()).Times(1).WillOnce(ReturnRef(field));
    ASSERT_EQ(&field, &(*iter));
}

TEST(field_iterator, When_AccessLink_Expect_CallMockLink) {
    mock_fields_iterator_t iter_mock {};
    mock_field_spec_t field {};
    test_iterator_t iter { mock_fields_iterator_helper_t { &iter_mock } };
    EXPECT_CALL(iter_mock, link()).Times(1).WillOnce(Return(&field));
    ASSERT_EQ(&field, iter.operator->());
}

TEST(field_iterator, When_PreIncrement_Expect_CallMockIncrease) {
    mock_fields_iterator_t iter_mock {};
    test_iterator_t iter { mock_fields_iterator_helper_t { &iter_mock } };
    EXPECT_CALL(iter_mock, increment()).Times(1);
    ASSERT_EQ(&iter, &(++iter));
}

TEST(field_iterator, When_PostIncrement_Expect_CallMockIncrease) {
    mock_fields_iterator_t iter_mock {};
    test_iterator_t iter { mock_fields_iterator_helper_t { &iter_mock } };
    EXPECT_CALL(iter_mock, increment()).Times(1);
    auto new_iter = iter++;
    ASSERT_NE(&iter, &(new_iter));
}

TEST(field_iterator, When_CopyIterator_Expect_CallMockAssign) {
    mock_fields_iterator_t iter_mock {};
    mock_fields_iterator_t iter_copy_mock {};
    test_iterator_t iter { mock_fields_iterator_helper_t { &iter_mock }};
    test_iterator_t iter_copy { mock_fields_iterator_helper_t { &iter_copy_mock } };
    EXPECT_CALL(iter_copy_mock, assign(Truly(iterator_impl_matcher_t { &iter_mock }))).Times(1);
    iter_copy = iter;
}

TEST(field_iterator, When_CompareEqualsIterator_Expect_CallMockEquals) {
    mock_fields_iterator_t iter_mock_1 {};
    mock_fields_iterator_t iter_mock_2 {};
    test_iterator_t iter_1 { mock_fields_iterator_helper_t { &iter_mock_1 } };
    test_iterator_t iter_2 { mock_fields_iterator_helper_t { &iter_mock_2 } };
    EXPECT_CALL(iter_mock_1, equals(Truly(iterator_impl_matcher_t { &iter_mock_2 }))).Times(1).WillOnce(Return(true));
    ASSERT_TRUE(iter_1 == iter_2);
}

TEST(field_iterator, When_CompareNotEqualsIterator_Expect_CallMockNotEquals) {
    mock_fields_iterator_t iter_mock_1 {};
    mock_fields_iterator_t iter_mock_2 {};
    test_iterator_t iter_1 { mock_fields_iterator_helper_t { &iter_mock_1 } };
    test_iterator_t iter_2 { mock_fields_iterator_helper_t { &iter_mock_2 } };
    EXPECT_CALL(iter_mock_1, not_equals(Truly(iterator_impl_matcher_t { &iter_mock_2 }))).Times(1).WillOnce(Return(true));
    ASSERT_TRUE(iter_1 != iter_2);
}

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
#include "types/fields.h"

using namespace hprof;

TEST(field_spec_impl_t, When_CreateWithNameId10_Expect_NameIdReturns10) {
    field_spec_impl_t field { 10, jvm_type_t::JVM_TYPE_INT, 100 };
    ASSERT_EQ(10, field.name_id());
}

TEST(field_spec_impl_t, When_CreateWithType8_Expect_TypeReturns8) {
    field_spec_impl_t field { 10, jvm_type_t::JVM_TYPE_INT, 100 };
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, field.type());
}

TEST(field_spec_impl_t, When_CreateWithOffset100_Expect_OffsetReturns100) {
    field_spec_impl_t field { 10, jvm_type_t::JVM_TYPE_INT, 100 };
    ASSERT_EQ(100, field.offset());
}

TEST(field_spec_impl_t, When_NameWasNotSet_Expect_NameReturnsEmptyString) {
    field_spec_impl_t field { 10, jvm_type_t::JVM_TYPE_INT, 100 };
    ASSERT_EQ("", field.name());
}

TEST(field_spec_impl_t, When_NameIsSet_Expect_NameReturnsSameValue) {
    field_spec_impl_t field { 10, jvm_type_t::JVM_TYPE_INT, 100 };
    field.set_name("mContext");
    ASSERT_EQ("mContext", field.name());
}

TEST(fields_spec_impl_t, When_CollectionWith2Items_Expect_CountEquals2) {
    fields_spec_impl_t fields { 4 };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    fields.add(field_spec_impl_t {1, jvm_type_t::JVM_TYPE_INT, 1});
    ASSERT_EQ(2, fields.count());
}

TEST(fields_spec_impl_t, When_SameCollection_Expect_BeginsAreQuals) {
    fields_spec_impl_t fields { 4 };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    ASSERT_EQ(std::begin(fields), std::begin(fields));
}

TEST(fields_spec_impl_t, When_DifferentCollections_Expect_BeginsAreNotEqual) {
    fields_spec_impl_t fields_1 { 4 };
    fields_1.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    fields_spec_impl_t fields_2 { 4 };
    fields_2.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    
    ASSERT_NE(std::begin(fields_1), std::begin(fields_2));
}

TEST(fields_spec_impl_t, When_SameCollection_Expect_EndsAreQuals) {
    fields_spec_impl_t fields { 4 };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    ASSERT_EQ(std::end(fields), std::end(fields));
}

TEST(fields_spec_impl_t, When_DifferentCollections_Expect_EndsAreNotEqual) {
    fields_spec_impl_t fields_1 { 4 };
    fields_1.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    fields_spec_impl_t fields_2 { 4 };
    fields_2.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    
    ASSERT_NE(std::end(fields_1), std::end(fields_2));
}

TEST(fields_spec_impl_t, When_EmptyCollection_Expect_BeginEqualsEnd) {
    fields_spec_impl_t fields { 4 };

    ASSERT_EQ(std::begin(fields), std::end(fields));
}

TEST(fields_spec_impl_t, When_ValidIndex_Expect_ItemByIndex) {
    fields_spec_impl_t fields { 4 };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    fields.add(field_spec_impl_t {1, jvm_type_t::JVM_TYPE_INT, 1});

    auto item = fields[1];
    ASSERT_NE(std::end(fields), item);
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, item->type());
}

TEST(fields_spec_impl_t, When_InvalidIndex_Expect_EndIterator) {
    fields_spec_impl_t fields { 4 };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    fields.add(field_spec_impl_t {1, jvm_type_t::JVM_TYPE_INT, 1});

    auto item = fields[3];
    ASSERT_EQ(std::end(fields), item);
}

TEST(fields_spec_impl_t, When_ValidName_Expect_FieldWithName) {
    fields_spec_impl_t fields { 4 };
    field_spec_impl_t field_name {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_spec_impl_t field_count {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_name.set_name("mName");
    field_count.set_name("mCount");

    fields.add(field_name);
    fields.add(field_count);

    auto item = fields.find("mCount");
    ASSERT_NE(std::end(fields), item);
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, item->type());
}

TEST(fields_spec_impl_t, When_InvalidName_Expect_EndIterator) {
    fields_spec_impl_t fields { 4 };
    field_spec_impl_t field_name {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_spec_impl_t field_count {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_name.set_name("mName");
    field_count.set_name("mCount");

    fields.add(field_name);
    fields.add(field_count);

    auto item = fields.find("mSize");
    ASSERT_EQ(std::end(fields), item);
}

TEST(fields_spec_impl_t, When_AddField_Expect_CountDataSize) {
    fields_spec_impl_t fields { 4 };
    fields.add( field_spec_impl_t { 0, jvm_type_t::JVM_TYPE_BOOL, 0 });
    fields.add( field_spec_impl_t { 0, jvm_type_t::JVM_TYPE_INT,  0 });
    ASSERT_EQ(5, fields.data_size());
}


TEST(field_value_impl_t, When_CreateFromFieldSpec_Expect_SaveFieldName) {
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_info.set_name("mName");
    field_value_impl_t field_value { field_info, 4, nullptr };
    ASSERT_EQ("mName", field_value.name());
}

TEST(field_value_impl_t, When_CreateFromFieldSpec_Expect_SaveFieldType) {
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_info.set_name("mName");
    field_value_impl_t field_value { field_info, 4, nullptr };
    ASSERT_EQ(jvm_type_t::JVM_TYPE_BOOL, field_value.type());
}

TEST(field_value_impl_t, When_ZeroByte_Expect_ReadBoolFalse) {
    u_int8_t data[] = { 0x00 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(false, static_cast<jvm_bool_t>(field_value));
}

TEST(field_value_impl_t, When_NonZeroByte_Expect_ReadBoolTrue) {
    u_int8_t data[] = { 0x01 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(true, static_cast<jvm_bool_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveByteValue_Expect_ReadByteValue) {
    u_int8_t data[] = { 0x0f };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_BYTE, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0f, static_cast<jvm_byte_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeByteValue_Expect_ReadByteValue) {
    u_int8_t data[] = { (u_int8_t) -0x0f };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_BYTE, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(-0x0f, static_cast<jvm_byte_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveShortValue_Expect_ReadShortValue) {
    u_int8_t data[] = { 0x0f, 0xfc };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_SHORT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0ffc, static_cast<jvm_short_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeShortValue_Expect_ReadShortValue) {
    u_int8_t data[] = { 0xf2, 0x75 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_SHORT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(-3467, static_cast<jvm_short_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveCharValue_Expect_ReadCharValue) {
    u_int8_t data[] = { 0x0f, 0xfc };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_CHAR, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0ffc, static_cast<jvm_char_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeCharValue_Expect_ReadCharValue) {
    u_int8_t data[] = { 0xf2, 0x75 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_CHAR, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0xf275, static_cast<jvm_char_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveIntValue_Expect_ReadIntValue) {
    u_int8_t data[] = { 0x0f, 0xfc, 0x00, 0x01 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0ffc0001, static_cast<jvm_int_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeIntValue_Expect_ReadIntValue) {
    u_int8_t data[] = { 0xf2, 0x75, 0x00, 0x01 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(-227213311, static_cast<jvm_int_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveFloatValue_Expect_ReadFloatValue) {
    u_int8_t data[] = { 0x3F, 0xC8, 0xAB, 0xB4 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_FLOAT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_FLOAT_EQ(1.56774, static_cast<jvm_float_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeFloatValue_Expect_ReadFloatValue) {
    u_int8_t data[] = { 0xbf, 0xc8, 0xab, 0xb4 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_FLOAT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_FLOAT_EQ(-1.56774, static_cast<jvm_float_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveIdValue_Expect_ReadIdValue) {
    u_int8_t data[] = { 0x0f, 0xfc, 0x00, 0x01 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_OBJECT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0ffc0001, static_cast<jvm_id_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeIdValue_Expect_ReadIdValue) {
    u_int8_t data[] = { 0xf2, 0x75, 0x00, 0x01 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_OBJECT, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0xf2750001, static_cast<jvm_id_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveLongValue_Expect_ReadIntValue) {
    u_int8_t data[] = { 0x0f, 0xfc, 0x00, 0x01, 0x10, 0x78, 0xA0, 0xE1 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_LONG, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0ffc00011078A0E1, static_cast<jvm_long_t>(field_value));
}

TEST(field_value_impl_t, When_NegativeLongValue_Expect_ReadIntValue) {
    u_int8_t data[] = { 0xf2, 0x75, 0x00, 0x01, 0x10, 0x78, 0xA0, 0xE1 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_LONG, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(-975873739684536095, static_cast<jvm_long_t>(field_value));
}

TEST(field_value_impl_t, When_PositiveDoubleValue_Expect_ReadDoubleValue) {
    u_int8_t data[] = { 0x3F, 0xe2, 0x2A, 0xf1, 0xfe, 0x8a, 0x8a, 0x70 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_DOUBLE, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_FLOAT_EQ(0.567742345, static_cast<jvm_double_t>(field_value));
}


TEST(field_value_impl_t, When_NegativeDoubleValue_Expect_ReadDoubleValue) {
    u_int8_t data[] = { 0xbF, 0xe2, 0x2A, 0xf1, 0xfe, 0x8a, 0x8a, 0x70 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_DOUBLE, 0};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_FLOAT_EQ(-0.567742345, static_cast<jvm_double_t>(field_value));
}

TEST(field_value_impl_t, When_DataIsNull_Expect_ReadZero) {
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_value_impl_t field_value { field_info, 4, nullptr };
    ASSERT_EQ(0, static_cast<jvm_int_t>(field_value));
}

TEST(field_value_impl_t, When_IntValueWithOffset_Expect_ReadIntValue) {
    u_int8_t data[] = { 0xFF, 0xFF, 0x0f, 0xfc, 0x00, 0x01 };
    field_spec_impl_t field_info {0, jvm_type_t::JVM_TYPE_INT, 2};
    field_value_impl_t field_value { field_info, 4, data };
    ASSERT_EQ(0x0ffc0001, static_cast<jvm_int_t>(field_value));
}

TEST(fields_values_impl_t, When_CollectionWith2Items_Expect_CountEquals2) {
    fields_values_impl_t fields { 4, nullptr };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    fields.add(field_spec_impl_t {1, jvm_type_t::JVM_TYPE_INT, 1});
    ASSERT_EQ(2, fields.count());
}

TEST(fields_values_impl_t, When_SameCollection_Expect_BeginsAreQuals) {
    fields_values_impl_t fields { 4, nullptr };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    ASSERT_EQ(std::begin(fields), std::begin(fields));
}

TEST(fields_values_impl_t, When_DifferentCollections_Expect_BeginsAreNotEqual) {
    fields_values_impl_t fields_1 { 4, nullptr };
    fields_1.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    fields_values_impl_t fields_2 { 4, nullptr };
    fields_2.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    
    ASSERT_NE(std::begin(fields_1), std::begin(fields_2));
}

TEST(fields_values_impl_t, When_AddWithValues_Expect_EmplaceNewField) {
    fields_values_impl_t fields { 4, nullptr };
    fields.add(10, "mInitialized", jvm_type_t::JVM_TYPE_BOOL, 1000);
    auto field = std::begin(fields);
    ASSERT_NE(std::end(fields), field);
    ASSERT_EQ("mInitialized", field->name());
    ASSERT_EQ(jvm_type_t::JVM_TYPE_BOOL, field->type());
    ASSERT_EQ(1000, field->offset());
}

TEST(fields_values_impl_t, When_SameCollection_Expect_EndsAreQuals) {
    fields_values_impl_t fields { 4, nullptr };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    ASSERT_EQ(std::end(fields), std::end(fields));
}

TEST(fields_values_impl_t, When_DifferentCollections_Expect_EndsAreNotEqual) {
    fields_values_impl_t fields_1 { 4, nullptr };
    fields_1.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});

    fields_values_impl_t fields_2 { 4, nullptr };
    fields_2.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    
    ASSERT_NE(std::end(fields_1), std::end(fields_2));
}

TEST(fields_values_impl_t, When_EmptyCollection_Expect_BeginEqualsEnd) {
    fields_values_impl_t fields { 4, nullptr };

    ASSERT_EQ(std::begin(fields), std::end(fields));
}

TEST(fields_values_impl_t, When_ValidIndex_Expect_ItemByIndex) {
    fields_values_impl_t fields { 4, nullptr };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    fields.add(field_spec_impl_t {1, jvm_type_t::JVM_TYPE_INT, 1});

    auto item = fields[1];
    ASSERT_NE(std::end(fields), item);
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, item->type());
}

TEST(fields_values_impl_t, When_InvalidIndex_Expect_EndIterator) {
    fields_values_impl_t fields { 4, nullptr };
    fields.add(field_spec_impl_t {0, jvm_type_t::JVM_TYPE_BOOL, 0});
    fields.add(field_spec_impl_t {1, jvm_type_t::JVM_TYPE_INT, 1});

    auto item = fields[3];
    ASSERT_EQ(std::end(fields), item);
}


TEST(fields_values_impl_t, When_ValidName_Expect_FieldWithName) {
    fields_values_impl_t fields { 0, nullptr };
    field_spec_impl_t field_name {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_spec_impl_t field_count {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_name.set_name("mName");
    field_count.set_name("mCount");

    fields.add(field_name);
    fields.add(field_count);

    auto item = fields.find("mCount");
    ASSERT_NE(std::end(fields), item);
    ASSERT_EQ(jvm_type_t::JVM_TYPE_INT, item->type());
}

TEST(fields_values_impl_t, When_InvalidName_Expect_EndIterator) {
    fields_values_impl_t fields { 0, nullptr };
    field_spec_impl_t field_name {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_spec_impl_t field_count {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_name.set_name("mName");
    field_count.set_name("mCount");

    fields.add(field_name);
    fields.add(field_count);

    auto item = fields.find("mSize");
    ASSERT_EQ(std::end(fields), item);
}

TEST(fields_values_impl_t, When_IncremetIterator_Expect_IteraateToNext) {
    fields_values_impl_t fields { 0, nullptr };
    field_spec_impl_t field_name {0, jvm_type_t::JVM_TYPE_BOOL, 0};
    field_spec_impl_t field_count {0, jvm_type_t::JVM_TYPE_INT, 0};
    field_name.set_name("mName");
    field_count.set_name("mCount");

    fields.add(field_name);
    fields.add(field_count);

    auto it = std::begin(fields);

    ASSERT_EQ("mName", (*it).name());
    ASSERT_NE(std::end(fields), ++it);
    ASSERT_EQ("mCount", (*it).name());
    ASSERT_EQ(std::end(fields), ++it);
}
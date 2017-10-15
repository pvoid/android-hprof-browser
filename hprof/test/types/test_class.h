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
#include "types/class.h"

#include "mocks.h"

using namespace hprof;

using testing::Return;

TEST(class_info_impl_t, When_SetSuperId300_Expect_ReturnSameSuperId300) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_super_id(300);
    ASSERT_EQ(300, cls->super_id());
}

TEST(class_info_impl_t, When_SuperIdIsNotSet_Expect_Return0) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->super_id());
}

TEST(class_info_impl_t, When_SetSuper_Expect_ReturnSameSuper) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    mock_class_info_t super;
    auto item = std::make_shared<mock_heap_item_t>();
    EXPECT_CALL(*item, as_class()).Times(1).WillOnce(Return(&super));
    cls->set_super_class(item);
    ASSERT_EQ(&super, cls->super());
}

TEST(class_info_impl_t, When_SuperIsNotSet_Expect_ReturnNull) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(nullptr, cls->super());
}

TEST(class_info_impl_t, When_ClassLoaderIdIsNotSet_Expect_Return0) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->class_loader_id());
}

TEST(class_info_impl_t, When_ClassLoaderId_Expect_ReturnClassLoaderId) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_class_loader_id(0x4000);
    ASSERT_EQ(0x4000, cls->class_loader_id());
}

TEST(class_info_impl_t, When_SetNameId103_Expect_ReturnSameNameId103) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_name_id(103);
    ASSERT_EQ(103, cls->name_id());
}

TEST(class_info_impl_t, When_NameIdIsNotSet_Expect_Return0) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->name_id());
}

TEST(class_info_impl_t, When_SetInstanceSize103_Expect_ReturnInstanceSize103) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_instance_size(103);
    ASSERT_EQ(103, cls->instance_size());
}

TEST(class_info_impl_t, When_InstanceSizeIsNotSet_Expect_Return0) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->instance_size());
}

TEST(class_info_impl_t, When_SetName_Expect_ReturnSameName) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_name("java.lang.String");
    ASSERT_EQ("java.lang.String", cls->name());
}

TEST(class_info_impl_t, When_NameIsNotSet_Expect_ReturnEmptyString) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ("", cls->name());
}

TEST(class_info_impl_t, When_AddField_Expect_SaveFieldInfo) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->add_field( field_spec_impl_t { 0, jvm_type_t::JVM_TYPE_BYTE, 0 } );
    ASSERT_EQ(1, cls->fields().count());
}

TEST(class_info_impl_t, When_NoFields_Expect_EmptyFieldsInfo) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->fields().count());
}

TEST(class_info_impl_t, When_AddStaticField_Expect_SaveFieldInfo) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->add_static_field( field_spec_impl_t { 0, jvm_type_t::JVM_TYPE_BYTE, 0 } );
    ASSERT_EQ(1, cls->static_fields().count());
}

TEST(class_info_impl_t, When_NoStaticFields_Expect_EmptyFieldsInfo) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->static_fields().count());
}

TEST(class_info_impl_t, When_HasNoLinks_Expect_HasLinksReturns0) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    ASSERT_EQ(0, cls->has_link_to(0x1000));
}

TEST(class_info_impl_t, When_HasLinkToClassLoader_Expect_HasLinksReturnsTypeClassloader) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_class_loader_id(0xf000);
    ASSERT_EQ(link_t::TYPE_CLASS_LOADER, cls->has_link_to(0xf000));
}

TEST(class_info_impl_t, When_HasLinkToSuper_Expect_HasLinksReturnsTypeSuper) {
    auto cls = class_info_impl_t::create(4, 1000, 0);
    cls->set_super_id(0xf000);
    ASSERT_EQ(link_t::TYPE_SUPER, cls->has_link_to(0xf000));
}
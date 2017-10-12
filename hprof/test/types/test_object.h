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
#include "types/object.h"

using namespace hprof;

class tested_object_info_impl_t : public object_info_impl_t {
public:
    tested_object_info_impl_t (u_int8_t id_size, jvm_id_t id) : object_info_impl_t(id_size, id) {}
    virtual ~tested_object_info_impl_t() {}

    virtual object_type_t type() const override { return TYPE_CLASS; }
    virtual int32_t has_link_to(jvm_id_t) const override { return false; }
};

TEST(object_info_impl_t, When_CreateWithIdSize4_Expect_IdSizeReturns4) {
    tested_object_info_impl_t obj { 4, 0xf0f0f0 };
    ASSERT_EQ(4, obj.id_size());
}

TEST(object_info_impl_t, When_CreateWithId1002_Expect_IdReturns1002) {
    tested_object_info_impl_t obj { 4, 1002 };
    ASSERT_EQ(1002, obj.id());
}

TEST(object_info_impl_t, When_DoNotSetHeapType_Expect_HeapTypeReturnsUnknown) {
    tested_object_info_impl_t obj { 4, 0xf0f0f0 };
    ASSERT_EQ(heap_info_t::HEAP_UNKNOWN, obj.heap_type());
}

TEST(object_info_impl_t, When_SetHeapType300_Expect_HeapTypeReturnsUnknown300) {
    tested_object_info_impl_t obj { 4, 0xf0f0f0 };
    obj.set_heap_type(300);
    ASSERT_EQ(300, obj.heap_type());
}

TEST(object_info_impl_t, When_AddRoots_Expect_AllRootsAreAdded) {
    tested_object_info_impl_t obj { 4, 0xf0f0f0 };
    obj.add_root(gc_root_impl_t::create<gc_root_t::VM_INTERNAL>(1007));
    obj.add_root(gc_root_impl_t::create<gc_root_t::UNREACHABLE>());

    auto& roots = obj.gc_roots();
    ASSERT_EQ(2, roots.size());
    ASSERT_NE(nullptr, roots[0]);
    ASSERT_EQ(gc_root_t::VM_INTERNAL, roots[0]->type());
    ASSERT_NE(nullptr, roots[1]);
    ASSERT_EQ(gc_root_t::UNREACHABLE, roots[1]->type());
}

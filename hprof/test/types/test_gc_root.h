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
#include "types/gc_root.h"

using namespace hprof;

TEST(gc_root_impl_t, When_CreateUnknwon_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::UNKNOWN>(1001);
    ASSERT_EQ(gc_root_t::UNKNOWN, root.type());
    ASSERT_EQ(1001, root.object_id());
}

TEST(gc_root_impl_t, When_CreateJniGlobal_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::JNI_GLOBAL>(1000, 0xf0f0f0);
    ASSERT_EQ(gc_root_t::JNI_GLOBAL, root.type());
    ASSERT_EQ(1000, root.object_id());
    ASSERT_EQ(0xf0f0f0, root.jni_global_info().jni_ref);
}

TEST(gc_root_impl_t, When_CreateJniLocal_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::JNI_LOCAL>(1001, 6789, 0xf0f0f0);
    ASSERT_EQ(gc_root_t::JNI_LOCAL, root.type());
    ASSERT_EQ(1001, root.object_id());
    ASSERT_EQ(6789, root.jni_local_info().thread_seq_num);
    ASSERT_EQ(0xf0f0f0, root.jni_local_info().stack_frame_id);
}

TEST(gc_root_impl_t, When_CreateJavaFrame_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::JAVA_FRAME>(1001, 6789, 0xf0f0f0);
    ASSERT_EQ(gc_root_t::JAVA_FRAME, root.type());
    ASSERT_EQ(1001, root.object_id());
    ASSERT_EQ(6789, root.java_frame_info().thread_seq_num);
    ASSERT_EQ(0xf0f0f0, root.java_frame_info().stack_frame_id);
}

TEST(gc_root_impl_t, When_CreateNativeStack_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::NATIVE_STACK>(1001, 6789);
    ASSERT_EQ(gc_root_t::NATIVE_STACK, root.type());
    ASSERT_EQ(1001, root.object_id());
    ASSERT_EQ(6789, root.native_stack_info().thread_seq_num);
}

TEST(gc_root_impl_t, When_CreateStickyClass_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::STICKY_CLASS>(1001);
    ASSERT_EQ(gc_root_t::STICKY_CLASS, root.type());
    ASSERT_EQ(1001, root.object_id());
}

TEST(gc_root_impl_t, When_CreateThreadBlock_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::THREAD_BLOCK>(1001, 6789);
    ASSERT_EQ(gc_root_t::THREAD_BLOCK, root.type());
    ASSERT_EQ(1001, root.object_id());
    ASSERT_EQ(6789, root.thread_block_info().thread_seq_num);
}

TEST(gc_root_impl_t, When_CreateMonitorUsed_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::MONITOR_USED>(1001);
    ASSERT_EQ(gc_root_t::MONITOR_USED, root.type());
    ASSERT_EQ(1001, root.object_id());
}

TEST(gc_root_impl_t, When_CreateThreadObject_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::THREAD_OBJECT>(1001, 6789, 0xf0f0f0);
    ASSERT_EQ(gc_root_t::THREAD_OBJECT, root.type());
    ASSERT_EQ(1001, root.object_id());
    ASSERT_EQ(6789, root.thread_object_info().thread_seq_num);
    ASSERT_EQ(0xf0f0f0, root.thread_object_info().stack_frame_id);
}

TEST(gc_root_impl_t, When_CreateInternalString_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::INTERNED_STRING>(1001);
    ASSERT_EQ(gc_root_t::INTERNED_STRING, root.type());
    ASSERT_EQ(1001, root.object_id());
}

TEST(gc_root_impl_t, When_CreateFinalizing_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::FINALIZING>();
    ASSERT_EQ(gc_root_t::FINALIZING, root.type());
}

TEST(gc_root_impl_t, When_CreateDebugger_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::DEBUGGER>(1001);
    ASSERT_EQ(gc_root_t::DEBUGGER, root.type());
    ASSERT_EQ(1001, root.object_id());
}

TEST(gc_root_impl_t, When_CreatereferenceCleanup_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::REFERENCE_CLEANUP>();
    ASSERT_EQ(gc_root_t::REFERENCE_CLEANUP, root.type());
}

TEST(gc_root_impl_t, When_CreateVmInternal_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::VM_INTERNAL>(1001);
    ASSERT_EQ(gc_root_t::VM_INTERNAL, root.type());
    ASSERT_EQ(1001, root.object_id());
}

TEST(gc_root_impl_t, When_CreateJniMonitor_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::JNI_MONITOR>(1001, 6789, 0xf0f0f0);
    ASSERT_EQ(gc_root_t::JNI_MONITOR, root.type());
    ASSERT_EQ(1001, root.object_id());
    ASSERT_EQ(6789, root.jni_monitor_info().thread_seq_num);
    ASSERT_EQ(0xf0f0f0, root.jni_monitor_info().stack_frame_id);
}

TEST(gc_root_impl_t, When_CreateUnreachable_Expect_SetValues) {
    auto root =  gc_root_impl_t::create<gc_root_t::UNREACHABLE>();
    ASSERT_EQ(gc_root_t::UNREACHABLE, root.type());
}

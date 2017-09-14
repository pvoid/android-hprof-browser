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

namespace hprof {
    struct gc_root_t {
        enum root_type_t {
            INVALID,
            UNKNOWN,
            JNI_GLOBAL,
            JNI_LOCAL,
            JAVA_FRAME,
            NATIVE_STACK,
            STICKY_CLASS,
            THREAD_BLOCK,
            MONITOR_USED,
            THREAD_OBJECT,
            INTERNED_STRING,
            FINALIZING,
            DEBUGGER,
            REFERENCE_CLEANUP,
            VM_INTERNAL,
            JNI_MONITOR,
            UNREACHABLE
        };

        root_type_t root_type;
        int32_t heap_type;
        id_t object_id;

        union {
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t jni_ref;
            } root_jni_global;
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t thread_seq_num;
                // cppcheck-suppress unusedStructMember
                int32_t stack_frame_id;
            } root_jni_local;
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t thread_seq_num;
                // cppcheck-suppress unusedStructMember
                int32_t stack_frame_id;
            } java_frame;
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t thread_seq_num;
            } native_stack;
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t thread_seq_num;
            } thread_block;
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t thread_seq_num;
                // cppcheck-suppress unusedStructMember
                int32_t stack_trace_id;
            } thread_object;
            struct {
                // cppcheck-suppress unusedStructMember
                int32_t thread_seq_num;
                // cppcheck-suppress unusedStructMember
                int32_t stack_trace_id;
            } jni_monitor;
        };

        explicit gc_root_t(root_type_t type = INVALID) : root_type(type), object_id(0) {
        }
    };
}

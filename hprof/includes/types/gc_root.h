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

#include "types.h"

namespace hprof {
    class gc_root_impl_t : public gc_root_t {
    public:
        virtual ~gc_root_impl_t() override;
        virtual root_type_t type() const override { return _root_type; }
        virtual jvm_id_t object_id() const override { return _object_id; }
        virtual const jni_global_info_t& jni_global_info() const override { return _jni_global; }
        virtual const jni_local_info_t& jni_local_info() const override { return _jni_local; }
        virtual const java_frame_info_t& java_frame_info() const override { return _java_frame; }
        virtual const native_stack_info_t& native_stack_info() const override { return _native_stack; }
        virtual const thread_block_info_t& thread_block_info() const override { return _thread_block; }
        virtual const thread_object_info_t& thread_object_info() const override { return _thread_object; }
        virtual const jni_monitor_info_t& jni_monitor_info() const override { return _jni_monitor; }
    public:
        template<root_type_t type>
        static gc_root_impl_t create() = delete;

        template<root_type_t type>
        static gc_root_impl_t create(jvm_id_t) = delete;

        template<root_type_t type>
        static gc_root_impl_t create(jvm_id_t, int32_t) = delete;

        template<root_type_t type>
        static gc_root_impl_t create(jvm_id_t, int32_t, int32_t) = delete;
    private:
        explicit gc_root_impl_t(root_type_t type = INVALID, jvm_id_t object_id = 0) : _root_type(type), _object_id(object_id) {}
    private:
        root_type_t _root_type;
        int32_t _heap_type;
        jvm_id_t _object_id;
        union {
            jni_global_info_t _jni_global;
            jni_local_info_t _jni_local;
            java_frame_info_t _java_frame;
            native_stack_info_t _native_stack;
            thread_block_info_t _thread_block;
            thread_object_info_t _thread_object;
            jni_monitor_info_t _jni_monitor;
        };
    };

    using gc_root_impl_ptr_t = std::unique_ptr<gc_root_impl_t>;

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::UNKNOWN>(jvm_id_t object_id) {
        return gc_root_impl_t { gc_root_t::UNKNOWN, object_id };
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::JNI_GLOBAL>(jvm_id_t object_id, int32_t ref) {
        gc_root_impl_t root { gc_root_impl_t::JNI_GLOBAL, object_id };
        root._jni_global.jni_ref = ref;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::JNI_LOCAL>(jvm_id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_impl_t root { gc_root_impl_t::JNI_LOCAL, object_id };
        root._jni_local.thread_seq_num = thread_seq_num;
        root._jni_local.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::JAVA_FRAME>(jvm_id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_impl_t root { gc_root_t::JAVA_FRAME, object_id };
        root._java_frame.thread_seq_num = thread_seq_num;
        root._java_frame.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::NATIVE_STACK>(jvm_id_t object_id, int32_t thread_seq_num) {
        gc_root_impl_t root { gc_root_t::NATIVE_STACK, object_id };
        root._native_stack.thread_seq_num = thread_seq_num;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::STICKY_CLASS>(jvm_id_t object_id) {
        gc_root_impl_t root { gc_root_t::STICKY_CLASS, object_id };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::THREAD_BLOCK>(jvm_id_t object_id, int32_t thread_seq_num) {
        gc_root_impl_t root { gc_root_impl_t::THREAD_BLOCK, object_id };
        root._thread_block.thread_seq_num = thread_seq_num;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::MONITOR_USED>(jvm_id_t object_id) {
        gc_root_impl_t root { gc_root_t::MONITOR_USED, object_id };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_impl_t::THREAD_OBJECT>(jvm_id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_impl_t root { gc_root_impl_t::THREAD_OBJECT, object_id };
        root._thread_object.thread_seq_num = thread_seq_num;
        root._thread_object.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::INTERNED_STRING>(jvm_id_t object_id) {
        gc_root_impl_t root { gc_root_t::INTERNED_STRING, object_id };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::FINALIZING>() {
        gc_root_impl_t root { gc_root_t::FINALIZING };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::DEBUGGER>(jvm_id_t object_id) {
        gc_root_impl_t root { gc_root_t::DEBUGGER, object_id };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::REFERENCE_CLEANUP>() {
        gc_root_impl_t root { gc_root_t::REFERENCE_CLEANUP };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::VM_INTERNAL>(jvm_id_t object_id) {
        gc_root_impl_t root { gc_root_t::VM_INTERNAL, object_id };
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_impl_t::JNI_MONITOR>(jvm_id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_impl_t root { gc_root_impl_t::JNI_MONITOR, object_id };
        root._jni_monitor.thread_seq_num = thread_seq_num;
        root._jni_monitor.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_impl_t gc_root_impl_t::create<gc_root_t::UNREACHABLE>() {
        gc_root_impl_t root { gc_root_t::UNREACHABLE };
        return root;
    }
}

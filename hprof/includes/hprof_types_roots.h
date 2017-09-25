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
    class gc_root_t {
    public:
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
    public:
        root_type_t type() const { return _root_type; }
        id_t object_id() const { return _object_id; }

    public:
        template<root_type_t type>
        static gc_root_t create(id_t object_id = 0) {
            return gc_root_t { type, object_id };
        }

        template<root_type_t type>
        static gc_root_t create(id_t object_id, int32_t) = delete;

        template<root_type_t type>
        static gc_root_t create(id_t object_id, int32_t, int32_t) = delete;

    private:
        explicit gc_root_t(root_type_t type = INVALID, id_t object_id = 0) : _root_type(type), _object_id(object_id) {}

    private:
        root_type_t _root_type;
        int32_t _heap_type;
        id_t _object_id;

        union {
            struct {
                int32_t jni_ref;
            } _root_jni_global;
            struct {
                int32_t thread_seq_num;
                int32_t stack_frame_id;
            } _root_jni_local;
            struct {
                int32_t thread_seq_num;
                int32_t stack_frame_id;
            } _java_frame;
            struct {
                int32_t thread_seq_num;
            } _native_stack;
            struct {
                int32_t thread_seq_num;
            } _thread_block;
            struct {
                int32_t thread_seq_num;
                int32_t stack_frame_id;
            } _thread_object;
            struct {
                int32_t thread_seq_num;
                int32_t stack_frame_id;
            } _jni_monitor;
        };
    };

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::JNI_GLOBAL>(id_t object_id, int32_t ref) {
        gc_root_t root { gc_root_t::JNI_GLOBAL, object_id };
        root._root_jni_global.jni_ref = ref;
        return root;
    }

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::JNI_LOCAL>(id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_t root { gc_root_t::JNI_LOCAL, object_id };
        root._root_jni_local.thread_seq_num = thread_seq_num;
        root._root_jni_local.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::JAVA_FRAME>(id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_t root { gc_root_t::JNI_LOCAL, object_id };
        root._java_frame.thread_seq_num = thread_seq_num;
        root._java_frame.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::NATIVE_STACK>(id_t object_id, int32_t thread_seq_num) {
        gc_root_t root { gc_root_t::NATIVE_STACK, object_id };
        root._native_stack.thread_seq_num = thread_seq_num;
        return root;
    }

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::THREAD_BLOCK>(id_t object_id, int32_t thread_seq_num) {
        gc_root_t root { gc_root_t::THREAD_BLOCK, object_id };
        root._thread_block.thread_seq_num = thread_seq_num;
        return root;
    }

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::THREAD_OBJECT>(id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_t root { gc_root_t::THREAD_OBJECT, object_id };
        root._thread_object.thread_seq_num = thread_seq_num;
        root._thread_object.stack_frame_id = stack_frame_id;
        return root;
    }

    template<>
    inline gc_root_t gc_root_t::create<gc_root_t::JNI_MONITOR>(id_t object_id, int32_t thread_seq_num, int32_t stack_frame_id) {
        gc_root_t root { gc_root_t::JNI_MONITOR, object_id };
        root._jni_monitor.thread_seq_num = thread_seq_num;
        root._jni_monitor.stack_frame_id = stack_frame_id;
        return root;
    }
}

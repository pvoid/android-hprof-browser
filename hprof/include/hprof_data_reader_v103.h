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

#include "hprof_data_reader.h"

#include <memory>

namespace hprof {
    class data_reader_v103_t : public data_reader_t {
    private:
        enum parse_result_t {
            HAS_NEXT_TOKEN,
            DONE,
            FAILED
        };
    public:
        data_reader_v103_t() {}
        virtual ~data_reader_v103_t() {}
        virtual std::unique_ptr<dump_data_t> build(hprof_istream& in) const override;
    private:
        enum hprof_tag_t : u_int8_t {
            TAG_UTF8_STRING = 0x01,
            TAG_LOAD_CLASS = 0x02,
            TAG_UNLOAD_CLASS = 0x03,
            TAG_STACK_FRAME = 0x04,
            TAG_STACK_TRACE = 0x05,
            TAG_ALLOC_SITES = 0x06,
            TAG_HEAP_SUMMARY = 0x07,
            TAG_START_THREAD = 0x0a,
            TAG_END_THREAD = 0x0b,
            TAG_HEAP_DUMP= 0x0c,
            TAG_HEAP_DUMP_SEGMENT = 0x1c,
            TAG_HEAP_DUMP_END = 0x2c,
            TAG_CPU_SAMPLES = 0x0d,
            TAG_CONTROL_SETTINGS = 0x0e,
        };

        enum hprof_gc_tag_t : u_int8_t {
            DUMP_ROOT_UNKNOWN = 0xff,
            DUMP_ROOT_JNI_GLOBAL = 0x01,
            DUMP_ROOT_JNI_LOCAL = 0x02,
            DUMP_ROOT_JAVA_FRAME = 0x03,
            DUMP_ROOT_NATIVE_STACK = 0x04,
            DUMP_ROOT_STICKY_CLASS = 0x05,
            DUMP_ROOT_THREAD_BLOCK = 0x06,
            DUMP_ROOT_MONITOR_USED  = 0x07,
            DUMP_ROOT_THREAD_OBJECT  = 0x08,
            DUMP_CLASS_DUMP  = 0x20,
            DUMP_INSTANCE_DUMP  = 0x21,
            DUMP_OBJECT_ARRAY_DUMP = 0x22,
            DUMP_PRIMITIVE_ARRAY_DUMP = 0x23,
            DUMP_HEAP_DUMP_INFO = 0xfe,
            DUMP_ROOT_INTERNED_STRING = 0x89,
            DUMP_ROOT_FINALIZING = 0x8a,
            DUMP_ROOT_DEBUGGER = 0x8b,
            DUMP_ROOT_REFERENCE_CLEANUP = 0x8c,
            DUMP_ROOT_VM_INTERNAL = 0x8d,
            DUMP_ROOT_JNI_MONITOR = 0x8e,
            DUMP_UNREACHABLE = 0x90,
            DUMP_PRIMITIVE_ARRAY_NODATA_DUMP = 0xc3,
        };

        enum jvm_type_t : u_int8_t {
            JVM_TYPE_OBJECT = 2,
            JVM_TYPE_BOOL = 4,
            JVM_TYPE_CHAR = 5,
            JVM_TYPE_FLOAT = 6,
            JVM_TYPE_DOUBLE = 7,
            JVM_TYPE_BYTE = 8,
            JVM_TYPE_SHORT = 9,
            JVM_TYPE_INT = 10,
            JVM_TYPE_LONG = 11,
        };

        class reader {
            using parse_result_t = data_reader_v103_t::parse_result_t;
        public:
            explicit reader(size_t id_size) : _id_size(id_size) {}
            parse_result_t read_next_record(hprof_istream& in);

            const strings_map_t& strings() const { return _strings; }
            const std::vector<gc_root_t>& roots() const { return _gc_roots; }
            const classes_map_t& classes() const { return _classes; }
            const instances_map_t& instances() const { return _instances; }
            const array_map_t& arrays() const { return _arrays; }
        private:
            struct loaded_class_t {
                int32_t class_seq;
                int32_t stack_trace_id;
                id_t name_id;

                loaded_class_t(int32_t seq, int32_t stack_trace, id_t name_id) : class_seq(seq), stack_trace_id(stack_trace), name_id(name_id) {}
            };
        private:
            inline static parse_result_t read_next_reacord_header(hprof_istream& in, hprof_tag_t& tag, int32_t& time_delta, int32_t& size);
            parse_result_t read_utf8_string(hprof_istream& in, ssize_t size);
            parse_result_t read_load_class(hprof_istream& in, ssize_t size);
            parse_result_t read_stack_frame(hprof_istream& in, ssize_t size);
            parse_result_t read_stack_trace(hprof_istream& in, ssize_t size);
            parse_result_t read_heap_dump_segment(hprof_istream& in, ssize_t size);

            id_t read_id(hprof_istream& in, ssize_t& data_left);
            bool read_class_dump(hprof_istream& in, ssize_t& data_left, class_info_impl_t& info);
            bool read_instance_dump(hprof_istream& in, ssize_t& data_left, instance_info_impl_ptr_t& info);
            bool read_objects_array_dump(hprof_istream& in, ssize_t& data_left, array_info_ptr_t& info);
            bool read_array_dump(hprof_istream& in, ssize_t& data_left, array_info_ptr_t& info);
            gc_root_t read_gc_root(hprof_gc_tag_t subtype, hprof_istream& in, ssize_t& data_left);
            size_t get_field_size(jvm_type_t type) const;
            field_info_t::field_type_t to_field_type(jvm_type_t type) const;
            primitive_array_info_t::array_type_t to_array_type(jvm_type_t type);
        private:
            const size_t _id_size;
            strings_map_t _strings;
            std::unordered_map<id_t, loaded_class_t> _loaded_classes;
            std::vector<heap_info_t> _heaps;
            std::vector<gc_root_t> _gc_roots;
            classes_map_t _classes;
            instances_map_t _instances;
            array_map_t _arrays;
        };
    };
}

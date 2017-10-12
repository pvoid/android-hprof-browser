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

#include "hprof.h"
#include "types/class.h"
#include "types/instance.h"
#include "types/objects_array.h"
#include "types/primitives_array.h"
#include "types/string_instance.h"

#include <memory>
#include <unordered_map>

namespace hprof {
    class data_reader_v103_t : public data_reader_t {
    public:
        data_reader_v103_t() {}
        virtual ~data_reader_v103_t() {}
        virtual std::unique_ptr<heap_profile_t> build(hprof_istream_t& in) const override;
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

        enum read_token_result_t {
            HAS_NEXT_TOKEN,
            DONE,
            FAILED
        };

        struct loaded_class_t {
            int32_t class_seq;
            int32_t stack_trace_id;
            id_t name_id;

            loaded_class_t(int32_t seq, int32_t stack_trace, id_t name_id) : class_seq(seq), stack_trace_id(stack_trace), name_id(name_id) {}
        };

        class hprof_section_reader {
        public:
            hprof_section_reader(hprof_istream_t& in, size_t id_size, size_t section_size) : 
                _in(in), _id_size(id_size), _data_left(section_size), _error_occurred(false) {}

            bool has_more_data() const { return _data_left > 0; }

            bool is_error_occurred() const { return _error_occurred; }

            size_t data_left() const { return _data_left; }

            jvm_id_t read_id() {
                _data_left -= _id_size;
                switch (_id_size) {
                    case 4:
                        return _in.read_int32();
                    case 8:
                        return _in.read_int64();
                    default:
                    // TODO: set stream to error state
                    assert(false);
                }
                _error_occurred = true;
                return 0;
            }

            void skip_all() {
                skip(_data_left);
            }

            void skip(size_t count) {
                if (count > _data_left) {
                    _error_occurred = true;
                    return;
                }

                for (; count > 0; --count) {
                    _in.read_byte();
                    if (_in.eof()) {
                        _error_occurred = true;
                        break;
                    }
                }
                
                _data_left -= count;
            }
            
            template<size_t SIZE>
            size_t read_bytes(u_int8_t (&buff)[SIZE]) {
                return read_bytes(buff, SIZE);
            }

            size_t read_bytes(u_int8_t *buff, size_t size) {
                if (size == 0) {
                    return 0;
                }

                size_t result = _in.read_bytes(buff, size);
                if (result == 0) {
                    _error_occurred = true;
                    return 0;
                }
                _data_left -= result;
                return result;
            }

            int32_t read_int32() {
                if (_data_left < 4) {
                    _error_occurred = true;
                    return 0;
                }
                _data_left -= 4;
                return _in.read_int32();
            }

            int32_t read_int16() {
                if (_data_left < 2) {
                    _error_occurred = true;
                    return 0;
                }
                _data_left -= 2;
                return _in.read_int16();
            }

            u_int8_t read_byte() {
                if (_data_left < 1) {
                    _error_occurred = true;
                    return 0;
                }
                _data_left -= 1;
                return _in.read_byte();
            }
        private:
            hprof_istream_t& _in;
            size_t _id_size;
            size_t _data_left;
            bool _error_occurred;
        };

        struct heap_profile_data_t {
            size_t id_size;
            std::unordered_map<jvm_id_t, std::string> strings;
            std::unordered_map<jvm_id_t, loaded_class_t> loaded_class;
            std::vector<object_info_impl_ptr_t> objects;
            std::vector<gc_root_impl_ptr_t> gc_roots;
            std::vector<class_info_impl_ptr_t> classes;
        };
    private:
        read_token_result_t next_record(hprof_istream_t& in, hprof_tag_t& tag, int32_t& time_delta, int32_t& size) const;
        bool process_next_token(hprof_tag_t tag, hprof_section_reader& reader, heap_profile_data_t& data) const;
        bool read_utf8_string(hprof_section_reader& reader, heap_profile_data_t& data) const;
        bool read_load_class(hprof_section_reader& reader, heap_profile_data_t& data) const;
        bool read_stack_frame(hprof_section_reader& reader, heap_profile_data_t&) const;
        bool read_stack_trace(hprof_section_reader& reader, heap_profile_data_t&) const;
        bool read_heap_dump_segment(hprof_section_reader& reader, heap_profile_data_t& data) const;
        bool read_class_dump(hprof_section_reader& reader, size_t id_size, const std::unordered_map<jvm_id_t, std::string>& strings, std::vector<class_info_impl_ptr_t>& classes) const;
        bool read_instance_dump(hprof_section_reader& reader, size_t id_size, std::vector<object_info_impl_ptr_t>& objects) const;
        bool read_objects_array_dump(hprof_section_reader& reader, size_t id_size, std::vector<object_info_impl_ptr_t>& objects) const;
        bool read_array_dump(hprof_section_reader& reader, size_t id_size, std::vector<object_info_impl_ptr_t>& objects) const;
        bool read_gc_root(hprof_gc_tag_t subtype, hprof_section_reader& reader, size_t id_size, std::vector<gc_root_impl_ptr_t>& roots) const;
        bool prepare(heap_profile_data_t& data) const;
    };
}

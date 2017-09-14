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

#include <sys/types.h>
#include <chrono>
#include <memory>

namespace hprof {
    using id_t = u_int64_t;
    using jvm_bool_t = u_int8_t;
    using jvm_byte_t = int8_t;
    using jvm_char_t = u_int16_t;
    using jvm_short_t = int16_t;
    using jvm_float_t = float;
    using jvm_double_t = double;
    using jvm_int_t = int32_t;
    using jvm_long_t = int64_t;

    using time_t = std::chrono::time_point<std::chrono::system_clock>;

    struct link_t {
        id_t from;
        enum {
            TYPE_INSTANCE = 0x001,
            TYPE_SUPER = 0x002,
            TYPE_CLASS_LOADER = 0x004,
            TYPE_OWNERSHIP = 0x008,
        } type;
    };

    struct link_comparator_t {
        bool operator()(const link_t& left, const link_t& right) const {
            return left.from < right.from;
        }
    };

    using incoming_links_t = std::set<link_t, link_comparator_t>;

    struct gc_root_t;

    class object_info_t {
    public:
        enum object_type_t {
            TYPE_CLASS,
            TYPE_INSTANCE,
            TYPE_OBJECTS_ARRAY,
            TYPE_PRIMITIVES_ARRAY
        };
    public:
        virtual id_t id() const = 0;
        virtual object_type_t type() const = 0;
        virtual int32_t has_link_to(id_t id) const = 0;

        void set_heap_type(int32_t heap_type) {
            _heap_type = heap_type;
        }

        void add_root(const gc_root_t& root) {
            _roots.push_back(root);
        }
    protected:
        int32_t _heap_type;
        std::vector<gc_root_t> _roots;
    };

    using object_info_ptr_t = std::shared_ptr<object_info_t>;

    struct field_info_t {
        enum field_type_t {
            TYPE_UNKNOWN,
            TYPE_OBJECT,
            TYPE_BOOL,
            TYPE_CHAR,
            TYPE_FLOAT,
            TYPE_DOUBLE,
            TYPE_BYTE,
            TYPE_SHORT,
            TYPE_INT,
            TYPE_LONG
        };

        id_t name_id;
        field_type_t type;
        size_t offset;

        field_info_t(id_t name, field_type_t type, size_t offset) : name_id(name), type(type), offset(offset) {}
    };

    struct static_field_t : field_info_t {
    public:
        static_field_t(id_t name, field_info_t::field_type_t type, const u_int8_t* value, size_t size) : field_info_t(name, type, 0), value {0} {
            memcpy(&(this->value), value, std::min(size, sizeof(this->value)));
        }
    public:
        union {
            jvm_bool_t bool_value;
            jvm_byte_t byte_value;
            jvm_char_t char_value;
            jvm_short_t short_value;
            jvm_int_t int_value;
            jvm_long_t long_value;
            jvm_float_t float_value;
            jvm_double_t double_value;
            id_t object_value;
        } value;
    };

}

#include "hprof_types_roots.h"

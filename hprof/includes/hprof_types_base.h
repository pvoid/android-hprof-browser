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
#include <set>
#include <vector>
#include <cstring>

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

    enum jvm_type_t {
        JVM_TYPE_UNKNOWN,
        JVM_TYPE_OBJECT,
        JVM_TYPE_BOOL,
        JVM_TYPE_CHAR,
        JVM_TYPE_FLOAT,
        JVM_TYPE_DOUBLE,
        JVM_TYPE_BYTE,
        JVM_TYPE_SHORT,
        JVM_TYPE_INT,
        JVM_TYPE_LONG
    };

    class object_info_t {
    public:
        enum object_type_t {
            TYPE_CLASS,
            TYPE_INSTANCE,
            TYPE_OBJECTS_ARRAY,
            TYPE_PRIMITIVES_ARRAY
        };
    public:
        explicit object_info_t(u_int8_t id_size) : _id_size(id_size), _heap_type(0) {}
        virtual ~object_info_t() {}

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
        u_int8_t _id_size;
        int32_t _heap_type;
        std::vector<gc_root_t> _roots;
    };

    using object_info_ptr_t = std::shared_ptr<object_info_t>;

    class field_info_t {
    public:

    public:
        field_info_t(id_t name, jvm_type_t type, size_t offset) : _name_id(name), _type(type), _offset(offset) {}

        id_t name_id() const { return _name_id; }
        jvm_type_t type() const { return _type; }
        size_t offset() const { return _offset; }
    private:
        id_t _name_id;
        jvm_type_t _type;
        size_t _offset;
    };

    class static_field_t : public field_info_t {
    public:
        static_field_t(id_t name, jvm_type_t type, const u_int8_t* value, size_t size) : field_info_t(name, type, 0), long_value {0} {
            for (const u_int8_t* last = value + size; value != last; ++value) {
                long_value <<= 8;
                long_value |= *value;
            }
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
        };

        operator jvm_bool_t() const { return bool_value; }
        operator jvm_byte_t() const { return byte_value; }
        operator jvm_char_t() const { return char_value; }
        operator jvm_short_t() const { return short_value; }
        operator jvm_int_t() const { return int_value; }
        operator jvm_long_t() const { return long_value; }
        operator jvm_float_t() const { return float_value; }
        operator jvm_double_t() const { return double_value; }
        operator id_t() const { return object_value; }
    };

}

#include "hprof_types_roots.h"

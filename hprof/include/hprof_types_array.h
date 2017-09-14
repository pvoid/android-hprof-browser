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

#include "hprof_types_base.h"

#include <unordered_map>

namespace hprof {
    using std::unordered_map;

    struct array_info_t : object_info_t {
    };

    struct object_array_info_t : array_info_t {
        id_t object_id;
        int32_t stack_trace_id;
        int32_t length;
        id_t *data;

        object_array_info_t(id_t object_id, int32_t stack_trace_id, int32_t length) :
            object_id(object_id), stack_trace_id(stack_trace_id), length(length),
            data(reinterpret_cast<id_t*>(reinterpret_cast<u_int8_t*>(this) + sizeof(object_array_info_t))) {}

        object_array_info_t(const object_array_info_t&) = delete;
        object_array_info_t(object_array_info_t&&) = delete;

        object_array_info_t& operator=(const object_array_info_t&) = delete;
        object_array_info_t& operator=(object_array_info_t&&) = delete;

        id_t id() const override { return object_id; }
        object_type_t type() const override { return TYPE_OBJECTS_ARRAY; }
        int32_t has_link_to(id_t id) const override {
            for (int index = 0; index < length; ++index) {
                if (data[index] == id) {
                    return link_t::TYPE_INSTANCE;
                }
            }
            return 0;
        }
    };

    struct primitive_array_info_t : array_info_t {
        enum array_type_t{
            TYPE_INVALID,
            TYPE_BOOL,
            TYPE_CHAR,
            TYPE_FLOAT,
            TYPE_DOUBLE,
            TYPE_BYTE,
            TYPE_SHORT,
            TYPE_INT,
            TYPE_LONG
        };

        id_t object_id;
        int32_t stack_trace_id;
        int32_t length;
        array_type_t value_type;
        union {
            // cppcheck-suppress unusedStructMember
            jvm_bool_t* bool_data;
            // cppcheck-suppress unusedStructMember
            jvm_char_t* char_data;
            // cppcheck-suppress unusedStructMember
            jvm_float_t* float_data;
            // cppcheck-suppress unusedStructMember
            jvm_double_t* double_data;
            // cppcheck-suppress unusedStructMember
            jvm_byte_t* byte_data;
            // cppcheck-suppress unusedStructMember
            jvm_short_t* short_data;
            // cppcheck-suppress unusedStructMember
            jvm_int_t* int_data;
            // cppcheck-suppress unusedStructMember
            jvm_long_t* long_data;
        } data;

        primitive_array_info_t(id_t id, int32_t stack_trace_id, array_type_t type, int32_t length) :
            object_id(id), stack_trace_id(stack_trace_id), value_type(type), length(length),
            data { reinterpret_cast<jvm_bool_t*>(reinterpret_cast<u_int8_t*>(this) + sizeof(primitive_array_info_t)) }  {}
        primitive_array_info_t(const primitive_array_info_t&) = delete;
        primitive_array_info_t(primitive_array_info_t&&) = delete;

        primitive_array_info_t& operator=(const primitive_array_info_t&) = delete;
        primitive_array_info_t& operator=(primitive_array_info_t&&) = delete;

        id_t id() const override { return object_id; }
        object_type_t type() const override { return TYPE_PRIMITIVES_ARRAY; }
        int32_t has_link_to(id_t id) const override { return 0; }
    };

    using array_info_ptr_t = std::shared_ptr<array_info_t>;
    using array_map_t = unordered_map<id_t, array_info_ptr_t>;
}

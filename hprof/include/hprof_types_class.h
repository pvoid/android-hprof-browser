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
#include "name_tokenizer.h"
#include <type_traits>
#include <unordered_map>

namespace hprof {
    using std::is_same;
    using std::vector;
    using std::unordered_map;

    struct class_info_t : object_info_t {
        id_t class_id;
        id_t super_id;
        id_t class_loader_id;
        id_t name_id;
        name_tokens tokens;
        int32_t seq_number;
        int32_t stack_trace_id;
        int32_t size;
        vector<field_info_t> fields;
        vector<static_field_t> static_fields;
        vector<object_info_ptr_t> instances;

        class_info_t() : class_id(0), super_id(0), class_loader_id(0), name_id(0), seq_number(0), stack_trace_id(0), size(0), instances(0) {}

        id_t id() const override { return class_id; }

        object_type_t type() const override { return TYPE_CLASS; }

        int32_t has_link_to(id_t id) const override {
            int32_t result = 0;
            if (super_id == id) {
                result |= link_t::TYPE_SUPER;
            }

            if (class_loader_id == id) {
                result |= link_t::TYPE_CLASS_LOADER;
            }

            for (const auto& field : static_fields) {
                if (field.type != field_info_t::TYPE_OBJECT) {
                    continue;
                }

                if (field.value.object_value == id) {
                    result |= link_t::TYPE_OWNERSHIP;
                    break;
                }
            }

            return result;
        }

        template<typename T>
        bool read_field_value(u_int8_t* data, size_t index, T& value) const {
            const field_info_t& info = fields[index];
            size_t size = 0;

            if (is_same<decltype(value), jvm_bool_t>::value || is_same<decltype(value), jvm_byte_t>::value) {
                size = 1;
            } else if (is_same<decltype(value), jvm_char_t>::value || is_same<decltype(value), jvm_short_t>::value) {
                size = 2;
            } else if (is_same<decltype(value), jvm_float_t>::value || is_same<decltype(value), jvm_int_t>::value) {
                size = 4;
            } else if (is_same<decltype(value), jvm_double_t>::value || is_same<decltype(value), jvm_long_t>::value) {
                size = 8;
            }

            if (size == 0) {
                return false;
            }

            memcpy(&value, data + info.offset, size);

            return true;
        }
    };

    using class_info_ptr_t = std::shared_ptr<class_info_t>;
    using classes_map_t = unordered_map<id_t, class_info_ptr_t>;
}

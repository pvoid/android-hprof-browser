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
#include "hprof_types_class.h"

#include <unordered_map>
#include <memory>
#include <cassert>

namespace hprof {
    using std::unordered_map;
    using std::shared_ptr;

    struct instance_info_t : object_info_t {
        id_t object_id;
        id_t class_id;
        class_info_ptr_t class_instance;
        int32_t stack_trace_id;
        int32_t size;
        u_int8_t *data;

        instance_info_t(id_t id, id_t class_id, int32_t stack_id, int32_t size) :
            object_id(id), class_id(class_id), stack_trace_id(stack_id), size(size), data(reinterpret_cast<u_int8_t*>(this) + sizeof(instance_info_t)) {
        }

        instance_info_t(const instance_info_t&) = delete;
        instance_info_t(instance_info_t&&) = delete;

        instance_info_t& operator=(const instance_info_t&) = delete;
        instance_info_t& operator=(instance_info_t&&) = delete;

        id_t id() const override { return object_id; }

        object_type_t type() const override { return TYPE_INSTANCE; }

        int32_t has_link_to(id_t id) const override {
            assert(class_instance != nullptr);

            int32_t result = 0;

            if (class_id == id) {
                result |= link_t::TYPE_INSTANCE;
            }

            id_t field_value;
            for (auto index = 0; index < class_instance->fields.size(); ++index) {
                if (class_instance->fields[index].type == field_info_t::TYPE_OBJECT) {
                    if (!class_instance->read_field_value(data, index, field_value)) {
                        continue;
                    }
                    if (field_value == id) {
                        result += link_t::TYPE_OWNERSHIP;
                        break;
                    }
                }
            }

            return result;
        }
    };

    using instance_info_ptr_t = shared_ptr<instance_info_t>;
    using instances_map_t = unordered_map<id_t, instance_info_ptr_t>;
}

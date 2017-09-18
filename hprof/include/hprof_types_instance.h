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
#include "hprof_istream.h"

#include <unordered_map>
#include <memory>
#include <cassert>

namespace hprof {
    using std::unordered_map;
    using std::shared_ptr;

    class instance_info_t : public object_info_t {
    public:
        instance_info_t(const instance_info_t&) = delete;
        instance_info_t(instance_info_t&&) = delete;

        instance_info_t& operator=(const instance_info_t&) = delete;
        instance_info_t& operator=(instance_info_t&&) = delete;

        id_t id() const override { return _object_id; }

        object_type_t type() const override { return TYPE_INSTANCE; }

        id_t class_id() const { return _class_id; }

        int32_t has_link_to(id_t id) const override {
            assert(_class_instance != nullptr);

            int32_t result = 0;

            if (_class_id == id) {
                result |= link_t::TYPE_INSTANCE;
            }

            id_t field_value;
            for (size_t index = 0, count = _class_instance->fields_count(); index < count; ++index) {
                if (_class_instance->field_type(index) == field_info_t::TYPE_OBJECT) {
                    if (!_class_instance->read_field_value(_data, index, field_value)) {
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

        const class_info_ptr_t get_class() const { return _class_instance; }
    protected:
        instance_info_t(id_t id, id_t class_id, int32_t stack_id, int32_t size) :
            _object_id(id), _class_id(class_id), _stack_trace_id(stack_id), _size(size), _data(reinterpret_cast<u_int8_t*>(this) + sizeof(instance_info_t)) {
        }
    protected:
        id_t _object_id;
        id_t _class_id;
        class_info_ptr_t _class_instance;
        int32_t _stack_trace_id;
        int32_t _size;
        u_int8_t* _data;
    };

    using instance_info_ptr_t = shared_ptr<instance_info_t>;

    class instance_info_impl_t : public instance_info_t {
    public:
        instance_info_impl_t(id_t id, id_t class_id, int32_t stack_id, int32_t size) : instance_info_t(id, class_id, stack_id, size) {}

        void read_data(hprof_istream& in) {
            in.read_bytes(_data, _size);
        }

        void set_class(class_info_ptr_t cls) {
            assert(_class_id == cls->id());
            _class_instance = cls;
        }
    };

    using instance_info_impl_ptr_t = shared_ptr<instance_info_impl_t>;
    using instances_map_t = unordered_map<id_t, instance_info_impl_ptr_t>;
}

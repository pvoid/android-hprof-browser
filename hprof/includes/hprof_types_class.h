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

#include <gtest/gtest_prod.h>
#include "hprof_types_base.h"
#include "name_tokenizer.h"
#include <unordered_map>

namespace hprof {
    using std::is_same;
    using std::vector;
    using std::unordered_map;

    class class_info_t : public object_info_t {
        friend class class_info_impl_t;
    public:
        virtual ~class_info_t() {}

        id_t id() const override { return _class_id; }

        object_type_t type() const override { return TYPE_CLASS; }

        id_t super_id() const { return _super_id; }

        id_t name_id() const { return _name_id; }

        int32_t has_link_to(id_t id) const override {
            int32_t result = 0;
            if (_super_id == id) {
                result |= link_t::TYPE_SUPER;
            }

            if (_class_loader_id == id) {
                result |= link_t::TYPE_CLASS_LOADER;
            }

            for (const auto& field : _static_fields) {
                if (field.type() != JVM_TYPE_OBJECT) {
                    continue;
                }

                if (field.object_value == id) {
                    result |= link_t::TYPE_OWNERSHIP;
                    break;
                }
            }

            return result;
        }

        const name_tokens& tokens() const { return _tokens; }

        size_t fields_count() const { return _fields.size(); }

        field_info_t field(size_t index) const {
            if (index < _fields.size()) {
                return _fields[index];
            }
            return field_info_t {0, JVM_TYPE_UNKNOWN, 0};
        }

        size_t static_fields_count() const { return _static_fields.size(); }

        const static_field_t& static_field(size_t index) const {
            return _static_fields[index];
        }

        void add_instance(object_info_ptr_t instance) {
            _instances.push_back(instance);
        }
    private:
        explicit class_info_t(u_int8_t id_size) : object_info_t(id_size), _class_id(0), _super_id(0), _class_loader_id(0), _name_id(0), _seq_number(0), _stack_trace_id(0), _size(0), _instances(0) {}
    private:
        id_t _class_id;
        id_t _super_id;
        id_t _class_loader_id;
        id_t _name_id;
        name_tokens _tokens;
        int32_t _seq_number;
        int32_t _stack_trace_id;
        int32_t _size;
        vector<field_info_t> _fields;
        vector<static_field_t> _static_fields;
        vector<object_info_ptr_t> _instances;
    };

    using class_info_ptr_t = std::shared_ptr<class_info_t>;

    class class_info_impl_t : public class_info_t {
    public:
        explicit class_info_impl_t(u_int8_t id_size) : class_info_t(id_size) {}
        virtual ~class_info_impl_t() {}
    public:
        void set_class_id(id_t id) {
            _class_id = id;
        }

        void set_stack_trace_id(int32_t stack_trace_id) {
            _stack_trace_id = stack_trace_id;
        }

        void set_super_id(id_t super_id) {
            _super_id = super_id;
        }

        void set_class_loader_id(id_t class_loader_id) {
            _class_loader_id = class_loader_id;
        }

        void set_name_id(id_t name_id) {
            _name_id = name_id;
        }

        void set_seq_number(int32_t seq_number) {
            _seq_number = seq_number;
        }

        void set_name(const std::string& name) {
            _tokens.set(name);
        }

        void set_data_size(size_t size) {
            _size = size;
        }

        void add_field_info(id_t name_id, jvm_type_t type, size_t offset) {
            _fields.emplace_back(name_id, type, offset);
        }

        void add_static_field(id_t name, jvm_type_t type, const u_int8_t* value, size_t size) {
            _static_fields.emplace_back(name, type, value, size);
        }
    };

    using class_info_impl_ptr_t = std::shared_ptr<class_info_impl_t>;
    using classes_map_t = unordered_map<id_t, class_info_impl_ptr_t>;
}

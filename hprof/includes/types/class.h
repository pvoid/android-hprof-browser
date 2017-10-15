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

#include "types.h"
#include "types/object.h"
#include "types/fields.h"

#include <memory>
#include <vector>

namespace hprof {
    class class_info_impl_t;

    class class_info_impl_t_deleter {
    public:
        void operator()(class_info_impl_t* ptr) const;
    };

    using class_info_impl_ptr_t = std::unique_ptr<class_info_impl_t, class_info_impl_t_deleter>;

    class class_info_impl_t : public virtual class_info_t, public object_info_impl_t {
    public:
        class_info_impl_t(const class_info_impl_t&) = delete;
        virtual ~class_info_impl_t();

        class_info_impl_t& operator=(const class_info_impl_t&) = delete;
        class_info_impl_t& operator=(class_info_impl_t&&) = default;

        virtual int32_t sequence_number() const override { return _seq_number; }
        virtual int32_t stack_trace_id() const override { return _stack_trace_id; }
        void set_stack_trace_id(int32_t id) { _stack_trace_id = id; }

        virtual jvm_id_t super_id() const override { return _super_id; }
        void set_super_id(jvm_id_t id) { _super_id = id; } 

        virtual const class_info_t* super() const override { return _super_class == nullptr ? nullptr : static_cast<const class_info_t*>(*_super_class); }
        void set_super_class(const heap_item_ptr_t& cls) { _super_class = cls; }

        virtual jvm_id_t class_loader_id() const override { return _class_loader_id; }
        void set_class_loader_id(jvm_id_t id) { _class_loader_id = id; }

        virtual jvm_id_t name_id() const override { return _name_id; }
        void set_name_id(jvm_id_t id) { _name_id = id; }

        virtual size_t name_matches(const name_tokens& name) const override { return _tokens.match(name); }

        void set_name(const std::string& name) { _name = name; _tokens.set(name); }
        virtual const std::string& name() const override { return _name; }

        virtual size_t instance_size() const override { return _size; }
        void set_instance_size(size_t size) { _size = size; }
        
        virtual int32_t has_link_to(jvm_id_t id) const override;

        virtual const fields_spec_t& fields() const override { return _fields; }
        void add_field(const field_spec_impl_t& field) { _fields.add(field); }

        virtual const fields_values_t& static_fields() const override { return _static_fields; }
        void add_static_field(const field_spec_impl_t& field) { _static_fields.add(field); }
        u_int8_t* data() { return _data; }
    public:
        static class_info_impl_ptr_t create(u_int8_t id_size, jvm_id_t id, size_t data_size) {
            auto mem = new (std::nothrow) u_int8_t[sizeof(class_info_impl_t) + data_size];
            return class_info_impl_ptr_t { new (mem) class_info_impl_t(id_size, id) };
        }
    private:
        class_info_impl_t(u_int8_t id_size, jvm_id_t id) : 
            object_info_impl_t(id_size, id), _super_id(0), _class_loader_id(0), _name_id(0), _seq_number(0), _stack_trace_id(0), _size(0), 
            _fields(id_size), _data(reinterpret_cast<u_int8_t*>(this) + sizeof(class_info_impl_t)), _static_fields(id_size, _data) {}
    private:
        jvm_id_t _super_id;
        heap_item_ptr_t _super_class;
        jvm_id_t _class_loader_id;
        jvm_id_t _name_id;
        std::string _name;
        name_tokens _tokens;
        int32_t _seq_number;
        int32_t _stack_trace_id;
        size_t _size;
        fields_spec_impl_t _fields;
        u_int8_t* _data;
        fields_values_impl_t _static_fields;
        // std::vector<object_info_ptr_t> _instances;
    };
}

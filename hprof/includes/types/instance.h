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

namespace hprof {
    class instance_info_impl_t;

    class instance_info_impl_t_deleter {
    public:
        void operator()(instance_info_impl_t* ptr) const;
    };
    
    using instance_info_impl_ptr_t = std::unique_ptr<instance_info_impl_t, instance_info_impl_t_deleter>;

    class instance_info_impl_t : public virtual instance_info_t, public object_info_impl_t {
        friend class string_info_impl_t;
    public:
        instance_info_impl_t(const instance_info_impl_t&) = delete;
        instance_info_impl_t(instance_info_impl_t&&) = default;
        virtual ~instance_info_impl_t() {}

        instance_info_impl_t& operator=(const instance_info_impl_t&) = delete;
        instance_info_impl_t& operator=(instance_info_impl_t&&) = default;

        virtual jvm_id_t class_id() const override { return _class_id; }
        void set_class_id(jvm_id_t id) { _class_id = id; }
        void set_stack_trace_id(int32_t value) { _stack_trace_id = value; }
        int32_t stack_trace_id() const override { return _stack_trace_id; }
        virtual const class_info_t* get_class() const override { return _class == nullptr ? nullptr : static_cast<const class_info_t *>(*_class); }
        void set_class(const heap_item_ptr_t& cls);
        virtual const fields_values_t& fields() const override { return _fields; }
        virtual int32_t has_link_to(jvm_id_t id) const override;
        u_int8_t* data() { return _data; }
        const u_int8_t* data() const { return _data; }
        size_t data_size() const { return _data_size; }
    public:
        static instance_info_impl_ptr_t create(u_int8_t id_size, jvm_id_t id, size_t data_size);
    private:
        instance_info_impl_t(u_int8_t id_size, jvm_id_t id, size_t data_size) :  
            object_info_impl_t(id_size, id), _class_id(0), _stack_trace_id(0), _data_size(data_size),
            _data(reinterpret_cast<u_int8_t *>(this) + sizeof(instance_info_impl_t)), _fields(id_size, _data) {}
        
        instance_info_impl_t(const instance_info_impl_t& src, u_int8_t* data) : 
            object_info_impl_t(src), _class_id(src._class_id), _stack_trace_id(src._stack_trace_id), 
            _class(src._class), _data_size(src._data_size), _data(data), _fields(src._fields, data) {}
    private:
        jvm_id_t _class_id;
        int32_t _stack_trace_id;
        heap_item_ptr_t _class;
        size_t _data_size;
        u_int8_t* _data;
        fields_values_impl_t _fields;
    };
}
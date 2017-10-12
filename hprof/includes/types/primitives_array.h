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
#include "types/value_reader.h"

#include <iostream>

namespace hprof {
    class primitives_array_info_impl_t;

    using primitives_array_info_impl_ptr_t = std::shared_ptr<primitives_array_info_impl_t>;

    class primitives_array_info_impl_t : public virtual primitives_array_info_t, public object_info_impl_t {
    private:
        class array_item_impl_t : public virtual array_item_t, private value_reader_t {
        public:
            array_item_impl_t() : value_reader_t(nullptr, 0), _type(jvm_type_t::JVM_TYPE_UNKNOWN), _offset(0) {}
            array_item_impl_t(const u_int8_t* data, size_t offset, size_t item_size, jvm_type_t type) : 
                value_reader_t(data + offset, item_size), _type(type), _offset(offset) {}
            virtual ~array_item_impl_t() {}
            virtual jvm_type_t type() const override { return _type; }
            virtual size_t offset() const override { return _offset; }
            virtual operator jvm_bool_t() const override { return value_reader_t::operator jvm_bool_t(); }
            virtual operator jvm_byte_t() const override { return value_reader_t::operator jvm_byte_t(); }
            virtual operator jvm_char_t() const override { return value_reader_t::operator jvm_char_t(); }
            virtual operator jvm_short_t() const override { return value_reader_t::operator jvm_short_t(); }
            virtual operator jvm_float_t() const override { return value_reader_t::operator jvm_float_t(); }
            virtual operator jvm_double_t() const override { return value_reader_t::operator jvm_double_t(); }
            virtual operator jvm_int_t() const override { return value_reader_t::operator jvm_int_t(); }
            virtual operator jvm_long_t() const override { return value_reader_t::operator jvm_long_t(); }
        private:
            jvm_type_t _type;
            size_t _offset;
        };

        class items_iterator {
        public:
            items_iterator(const u_int8_t* data, jvm_type_t type, size_t id_size) : 
                _offset(0), _data(data), _type(type), _item_size(jvm_type_t::size(type, id_size)), _current(_data, _offset, _item_size, _type) {
            }

            bool operator!=(const items_iterator& src) const { return _data + _offset != src._data + src._offset; }
            bool operator==(const items_iterator& src) const { return _data + _offset == src._data + src._offset; }
    
            items_iterator& operator++() {
                _offset += _item_size;
                _current = array_item_impl_t {_data, _offset, _item_size, _type};
                return *this; 
            }
    
            const array_item_impl_t& operator*() const { return _current; }
                const array_item_impl_t* operator->() const { return &_current; }
        private:
            size_t _offset;
            mutable const u_int8_t* _data;
            jvm_type_t _type;
            size_t _item_size;
            mutable array_item_impl_t _current;
        };
    public:
        primitives_array_info_impl_t(const primitives_array_info_impl_t&) = delete;
        primitives_array_info_impl_t(primitives_array_info_impl_t&&) = default;
        virtual ~primitives_array_info_impl_t() {}

        primitives_array_info_impl_t& operator=(const primitives_array_info_impl_t&) = delete;
        primitives_array_info_impl_t& operator=(primitives_array_info_impl_t&&) = default;

        virtual object_type_t type() const override { return TYPE_PRIMITIVES_ARRAY; }
        virtual int32_t has_link_to(jvm_id_t id) const override {
            return 0;
        }

        virtual size_t length() const override { return _length; }
        virtual jvm_type_t item_type() const override { return _type; }

        virtual iterator begin() const override { return items_iterator {_data, _type, id_size()}; }
        virtual iterator end() const override { return items_iterator {_data + jvm_type_t::size(_type, id_size()) * _length, _type, id_size()}; }
        virtual iterator operator[](size_t index) const override {
            if (index < _length) {
                auto offset = jvm_type_t::size(_type, id_size()) * index;
                return items_iterator { _data + offset, _type, id_size() };
            }
            return end();
        }

        u_int8_t* data() { return _data; }
    public:
        static primitives_array_info_impl_ptr_t create(size_t id_size, jvm_id_t id, jvm_type_t type, size_t length, size_t data_size) {
            auto mem = new (std::nothrow) u_int8_t[sizeof(primitives_array_info_impl_t) + data_size];
            primitives_array_info_impl_ptr_t result { new (mem) primitives_array_info_impl_t(id_size, id, type, length), 
                [] (auto item) { item->~primitives_array_info_impl_t(); delete[] (u_int8_t*)item; }};
            return result;
        }
    private:
        primitives_array_info_impl_t(size_t id_size, jvm_id_t id, jvm_type_t type, size_t length) : 
        object_info_impl_t(id_size, id), _type(type), _length(length), _data(reinterpret_cast<u_int8_t*>(this) + sizeof(primitives_array_info_impl_t)) {}
    
    private:
        jvm_type_t _type;
        size_t _length;
        u_int8_t* _data;
    };
}
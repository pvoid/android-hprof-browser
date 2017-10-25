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
#include "types/value_reader.h"

#include <vector>

namespace hprof {
    class field_spec_impl_t: public field_spec_t {
    public:
        field_spec_impl_t(jvm_id_t name_id, jvm_type_t type, size_t offset) : _name_id(name_id), _type(type), _offset(offset) {}
        field_spec_impl_t(jvm_id_t name_id, const std::string& name, jvm_type_t type, size_t offset) : _name_id(name_id), _name(name), _type(type), _offset(offset) {}
        virtual ~field_spec_impl_t() {}

        virtual jvm_id_t name_id() const override { return _name_id; }
        virtual const std::string& name() const override { return _name; }
        virtual jvm_type_t type() const override { return _type; }
        virtual size_t offset() const override { return _offset; }

        void set_name(const std::string& name) {
            _name = name;
        }

    private:
        jvm_id_t _name_id;
        std::string _name;
        jvm_type_t _type;
        size_t _offset;
    };

    class fields_spec_impl_t : public fields_spec_t {
    public:
        fields_spec_impl_t(size_t id_size) : _id_size(id_size), _data_size(0) {}
        virtual ~fields_spec_impl_t() {}

        virtual size_t count() const override { return _fields.size(); }
        virtual fields_spec_t::iterator operator[](size_t index) const override {
            if (index < _fields.size()) {
                return std::begin(_fields) + index;
            }
            return std::end(_fields);
        }

        virtual fields_spec_t::iterator find(const std::string& name) const override {
            for (auto it = std::begin(_fields); it != std::end(_fields); ++it) {
                if (it->name() == name) {
                    return it;
                }
            }
            return end();
        }

        virtual fields_spec_t::iterator begin() const override { return std::begin(_fields); }
        virtual fields_spec_t::iterator end() const override { return std::end(_fields); }
        virtual size_t data_size() const override { return _data_size; }

        void add(const field_spec_impl_t& field) {
            _fields.push_back(field);
            _data_size += jvm_type_t::size(field.type(), _id_size);
        }
    private:
        size_t _id_size;
        size_t _data_size;
        std::vector<field_spec_impl_t> _fields;
    };

    class field_value_impl_t : public field_value_t, private value_reader_t {
    public:
        field_value_impl_t(const field_spec_t& field, size_t id_size, const u_int8_t* data) 
            :  field_value_impl_t(field.name(), field.type(), field.offset(), id_size, data) {}

        field_value_impl_t(const std::string& name, jvm_type_t type, size_t offset, size_t id_size, const u_int8_t* data) 
            : value_reader_t(data + offset, jvm_type_t::size(type, id_size)), _name(name), _type(type), _offset(offset) {}

        field_value_impl_t() : value_reader_t(nullptr, 0), _type(jvm_type_t::JVM_TYPE_UNKNOWN), _offset(0) {}
        virtual ~field_value_impl_t() {}

        virtual const std::string& name() const override { return _name; }
        virtual jvm_type_t type() const override { return _type; }
        virtual size_t offset() const override { return _offset; }
        
        virtual operator jvm_id_t() const override { return value_reader_t::operator jvm_id_t(); }
        virtual operator jvm_bool_t() const override { return value_reader_t::operator jvm_bool_t(); }
        virtual operator jvm_byte_t() const override { return value_reader_t::operator jvm_byte_t(); }
        virtual operator jvm_char_t() const override { return value_reader_t::operator jvm_char_t(); }
        virtual operator jvm_short_t() const override { return value_reader_t::operator jvm_short_t(); }
        virtual operator jvm_float_t() const override { return value_reader_t::operator jvm_float_t(); }
        virtual operator jvm_double_t() const override { return value_reader_t::operator jvm_double_t(); }
        virtual operator jvm_int_t() const override { return value_reader_t::operator jvm_int_t(); }
        virtual operator jvm_long_t() const override { return value_reader_t::operator jvm_long_t(); }
        
    private:
        std::string _name;
        jvm_type_t _type;
        size_t _offset;
    };

    class fields_values_impl_t : public fields_values_t {
        class iterator;
    public:
        fields_values_impl_t(size_t id_size, const u_int8_t* data) : _id_size(id_size), _data(data) {}
        fields_values_impl_t(const fields_values_impl_t& src, const u_int8_t* data) : _id_size(src._id_size), _data(data) {
            std::copy(std::begin(src._fields), std::end(src._fields), std::back_inserter(_fields));
        }
        virtual ~fields_values_impl_t() {}
        virtual size_t count() const override { return _fields.size(); }

        virtual fields_values_t::iterator operator[](size_t index) const override {
            if (index < _fields.size()) {
                return iterator { _id_size, std::begin(_fields) + index, _data };
            }
            return end();
        }

        virtual fields_values_t::iterator find(std::string name) const override {
            for (auto it = std::begin(_fields); it != std::end(_fields); ++it) {
                if (it->name() == name) {
                    return iterator { _id_size, it, _data };
                }
            }
            return end();
        }

        virtual fields_values_t::iterator begin() const override { 
            return iterator { _id_size, std::begin(_fields), _data }; 
        }

        virtual fields_values_t::iterator end() const override { 
            return iterator { _id_size, std::end(_fields), _data }; 
        }

        void add(const field_spec_impl_t& field) {
            _fields.push_back(field);
        }

        void add(jvm_id_t name_id, const std::string& name, jvm_type_t type, size_t offset) {
            _fields.emplace_back(name_id, name, type, offset);
        }
    private:
        class iterator {
        public:
            iterator(size_t id_size, std::vector<field_spec_impl_t>::const_iterator it, const u_int8_t* data) : 
                _id_size(id_size), _it(it), _data(data), _fetch_value(true) {}
            virtual ~iterator() {}

            bool operator==(const iterator& src) const { return _it == src._it; }
            bool operator!=(const iterator& src) const { return _it != src._it; }

            iterator& operator++() { 
                ++_it; 
                _fetch_value = true; 
                return *this; 
            }

            const field_value_t& operator*() const {
                fetch_if_necessary();
                return _current;
            }

            const field_value_t* operator->() const {
                fetch_if_necessary();
                return &_current;
            }
        private:
            void fetch_if_necessary() const {
                if (!_fetch_value) return;

                _current = field_value_impl_t { *_it, _id_size, _data };
                _fetch_value = false;
            }
        private:
            size_t _id_size;
            std::vector<field_spec_impl_t>::const_iterator _it;
            const u_int8_t* _data;
            mutable bool _fetch_value;
            mutable field_value_impl_t _current;
        };
    private:
        std::vector<field_spec_impl_t> _fields;
        size_t _id_size;
        const u_int8_t* _data;
    };
}

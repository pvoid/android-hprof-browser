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
#include <type_traits>

#include <iostream>

namespace hprof {
    using std::unordered_map;
    using std::shared_ptr;

    class instance_info_t : public object_info_t {
        friend class instance_info_impl_t;
    public:
        class fields_t;
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

            for (size_t index = 0, count = _fields.count(); index < count; ++index) {
                auto field = _fields[index];
                if (field.type() == JVM_TYPE_OBJECT) {
                    if (static_cast<id_t>(field) == id) {
                        result += link_t::TYPE_OWNERSHIP;
                        break;
                    }
                }
            }

            return result;
        }

        const class_info_ptr_t get_class() const { return _class_instance; }

        const fields_t& fields() const { return _fields; }

        const void dump() const {
            for (int index = 0; index < _size; ++index) {
                std::cout << (int) _data[index] << " ";
            }
            std::cout << std::endl;
        }
    public:
        class field_t {
            friend class fields_t;
        public:
            id_t name_id() const { return _name_id; }
            jvm_type_t type() const { return _type; }

            operator jvm_bool_t() const {
                u_int8_t result = false;
                read_value(result);
                return *(jvm_bool_t *) &result;
            }

            operator jvm_byte_t() const {
                u_int8_t result = 0;
                read_value(result);
                return *(jvm_byte_t *) &result;
            }

            operator jvm_char_t() const {
                u_int16_t result = 0;
                read_value(result);
                return *(jvm_char_t *) &result;
            }

            operator jvm_short_t() const {
                u_int16_t result = 0;
                read_value(result);
                return *(jvm_short_t *) &result;
            }

            operator jvm_int_t() const {
                u_int32_t result = 0;
                read_value(result);
                return *(jvm_int_t *) &result;
            }

            operator jvm_long_t() const {
                u_int64_t result = 0;
                read_value(result);
                return *(jvm_long_t *) &result;
            }

            operator jvm_float_t() const {
                u_int32_t result = 0;
                read_value(result);
                return *(jvm_float_t *) &result;
            }

            operator jvm_double_t() const {
                u_int64_t result = 0.f;
                read_value(result);
                return *(jvm_double_t *) &result;
            }

            operator id_t() const {
                switch (_id_size) {
                    case sizeof(u_int32_t): {
                        return operator jvm_int_t();
                    }
                    case sizeof(u_int64_t): {
                        return operator jvm_long_t();
                    }
                    default:
                        assert(0);
                }
            }

        private:
            field_t(u_int8_t id_size, id_t name, jvm_type_t type, size_t offset, u_int8_t* data) :
                _id_size(id_size), _name_id(name), _type(type), _offset(offset), _data(data) {}

            template<typename T>
            bool read_value(T& value) const {
                value = 0;
                for (auto val = _data + _offset; val < _data + _offset + sizeof(value); ++val) {
                    if (sizeof(value) > 1) {
                        value <<= 8;
                    }
                    value |= *val;
                }

                return true;
            }
        private:
            u_int8_t _id_size;
            id_t _name_id;
            jvm_type_t _type;
            size_t _offset;
            u_int8_t* _data;
        };

        class fields_t {
            friend class instance_info_t;
            friend class instance_info_impl_t;
        public:
            size_t count() const { return _class->fields_count(); }

            field_t operator[](size_t index) const {
                field_info_t info = _class->field(index);
                return field_t { _id_size, info.name_id(), info.type(), info.offset(), _data };
            }

        private:
            explicit fields_t(u_int8_t id_size, u_int8_t* data) : _id_size(id_size), _data(data) {}
        private:
            u_int8_t _id_size;
            class_info_ptr_t _class;
            u_int8_t* _data;
        };
    private:
        instance_info_t(u_int8_t id_size, id_t id, id_t class_id, int32_t stack_id, int32_t size) :
            object_info_t(id_size), _object_id(id), _class_id(class_id), _stack_trace_id(stack_id), _size(size),
            _data(reinterpret_cast<u_int8_t*>(this) + sizeof(instance_info_t)), _fields(id_size, _data) {
        }
    private:
        id_t _object_id;
        id_t _class_id;
        class_info_ptr_t _class_instance;
        int32_t _stack_trace_id;
        int32_t _size;
        u_int8_t* _data;
        fields_t _fields;
    };

    using instance_info_ptr_t = shared_ptr<instance_info_t>;

    class instance_info_impl_t : public instance_info_t {
    public:
        instance_info_impl_t(u_int8_t id_size, id_t id, id_t class_id, int32_t stack_id, int32_t size) : instance_info_t(id_size, id, class_id, stack_id, size) {}

        void read_data(hprof_istream& in) {
            in.read_bytes(_data, _size);
        }

        void set_class(class_info_ptr_t cls) {
            assert(_class_id == cls->id());
            _class_instance = cls;
            _fields._class = cls;
        }
    };

    using instance_info_impl_ptr_t = shared_ptr<instance_info_impl_t>;
    using instances_map_t = unordered_map<id_t, instance_info_impl_ptr_t>;
}

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

    class array_info_t : public object_info_t {
    public:
        virtual ~array_info_t() {}
        array_info_t(const array_info_t&) = delete;
        array_info_t(array_info_t&&) = delete;

        array_info_t& operator=(const array_info_t&) = delete;
        array_info_t& operator=(array_info_t&&) = delete;

        id_t id() const override { return _object_id; }
        size_t length() const { return _length; }
    protected:
        explicit array_info_t(u_int8_t id_size, id_t object_id, int32_t stack_trace_id, int32_t length, u_int8_t* data) :
            object_info_t(id_size), _object_id(object_id), _stack_trace_id(stack_trace_id), _length(length), _data(data) {}

        template<typename T>
        T read_item(size_t index) const {
            T value = 0;
            u_int8_t* current = _data + index * sizeof(value);
            u_int8_t* last = _data + ((index + 1) * sizeof(value));
            for (; current != last; ++current) {
                if (sizeof(value) > 1) {
                    value <<= 8;
                }
                value |= *current;
            }
            return value;
        }
    protected:
        id_t _object_id;
        int32_t _stack_trace_id;
        int32_t _length;
        u_int8_t* _data;
    };

    class object_array_info_t : public array_info_t {
        friend class object_array_info_impl_t;
    public:
        virtual ~object_array_info_t() {}

        object_type_t type() const override { return TYPE_OBJECTS_ARRAY; }
        int32_t has_link_to(id_t id) const override {
            for (int index = 0; index < _length; ++index) {
                if (_data[index] == id) {
                    return link_t::TYPE_INSTANCE;
                }
            }
            return 0;
        }

        id_t item(size_t index) const {
            switch (_id_size) {
                case 4: {
                    return read_item<u_int32_t>(index);
                }
                case 8: {
                    return read_item<u_int64_t>(index);
                }
            }
            return 0;
        }
    private:
        object_array_info_t(u_int8_t id_size, id_t object_id, int32_t stack_trace_id, int32_t length) :
            array_info_t(id_size, object_id, stack_trace_id, length,
                reinterpret_cast<u_int8_t*>(this) + sizeof(object_array_info_t)) {}
    };

    class object_array_info_impl_t : public object_array_info_t {
    public:
        object_array_info_impl_t(u_int8_t id_size, id_t object_id, int32_t stack_trace_id, int32_t length) :
            object_array_info_t(id_size, object_id, stack_trace_id, length) {}
        virtual ~object_array_info_impl_t() {}

        void read(hprof_istream& in, size_t size) {
            in.read_bytes(_data, size);
        }
    };

    class primitive_array_info_t : public array_info_t {
        friend class primitive_array_info_impl_t;
    public:
        virtual ~primitive_array_info_t() {}
        object_type_t type() const override { return TYPE_PRIMITIVES_ARRAY; }

        int32_t has_link_to(id_t id) const override { return 0; }
        jvm_type_t value_type() const { return _value_type; }

        template<typename T>
        T item(size_t index) const = delete;

    private:
        primitive_array_info_t(u_int8_t id_size, id_t id, int32_t stack_trace_id, jvm_type_t type, int32_t length) :
            array_info_t(id_size, id, stack_trace_id, length, reinterpret_cast<u_int8_t*>(this) + sizeof(primitive_array_info_t)),
            _value_type(type) {}


    private:
        jvm_type_t _value_type;
    };

    class primitive_array_info_impl_t : public primitive_array_info_t {
    public:
        primitive_array_info_impl_t(u_int8_t id_size, id_t id, int32_t stack_trace_id, jvm_type_t type, int32_t length) :
            primitive_array_info_t(id_size, id, stack_trace_id, type, length) {}
        virtual ~primitive_array_info_impl_t() {}

        void read(hprof_istream& in, size_t size) {
            in.read_bytes(_data, size);
        }
    };

    template<>
    inline jvm_bool_t primitive_array_info_t::item(size_t index) const {
        return static_cast<jvm_bool_t>(_data[index]);
    }

    template<>
    inline jvm_byte_t primitive_array_info_t::item(size_t index) const {
        return static_cast<jvm_byte_t>(_data[index]);
    }

    template<>
    inline jvm_short_t primitive_array_info_t::item(size_t index) const {
        return read_item<jvm_short_t>(index);
    }

    template<>
    inline jvm_char_t primitive_array_info_t::item(size_t index) const {
        return read_item<jvm_char_t>(index);
    }

    template<>
    inline jvm_int_t primitive_array_info_t::item(size_t index) const {
        return read_item<jvm_int_t>(index);
    }

    template<>
    inline jvm_float_t primitive_array_info_t::item(size_t index) const {
        auto result = read_item<jvm_int_t>(index);
        return  *(jvm_float_t *) &result;
    }

    template<>
    inline jvm_long_t primitive_array_info_t::item(size_t index) const {
        return read_item<jvm_long_t>(index);
    }

    template<>
    inline jvm_double_t primitive_array_info_t::item(size_t index) const {
        auto result = read_item<jvm_long_t>(index);
        return *(jvm_double_t *) &result;
    }

    using array_info_ptr_t = std::shared_ptr<array_info_t>;
    using array_map_t = unordered_map<id_t, array_info_ptr_t>;
}

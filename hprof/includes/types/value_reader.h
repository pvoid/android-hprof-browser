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

namespace hprof {
    class value_reader_t {
    public:
        value_reader_t(const u_int8_t* data, size_t size) : _size(size), _data(data) {} 
        virtual ~value_reader_t() {}

        operator jvm_id_t() const {
            u_int64_t result = 0;
            read_value(result);
            return *(jvm_id_t *) &result;
        }

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

    private:
        template<typename T>
        bool read_value(T& result) const {
            if (_data == nullptr) {
                return false;
            }

            result = 0;
            for (auto val = _data; val < _data + _size; ++val) {
                if (sizeof(result) > 1) {
                    result <<= 8;
                }
                result |= *val;
            }

            return true;
        }
    private:
        size_t _size;
        const u_int8_t* _data;
    };
}
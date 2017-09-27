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

#include "hprof_types_instance.h"

#include <codecvt>
#include <locale>

namespace hprof {
    class dump_data_t;

    class types_helper_t {
        using text_converter = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>;
    public:
        enum string_value_t {
            Invalid,
            NotString,
            Null,
            Value
        };
    public:
        explicit types_helper_t(id_t string_class, const dump_data_t& dump) : _string_class_id(string_class), _dump(dump) {}

        types_helper_t(const types_helper_t&) = delete;
        types_helper_t(types_helper_t&&) = default;

        types_helper_t& operator=(const types_helper_t&) = delete;
        types_helper_t& operator=(types_helper_t&&) = default;

        bool is_string(const object_info_t& object) const;
        string_value_t get_string_value(const instance_info_t& object, std::string& result) const;
    private:
        id_t _string_class_id;
        mutable text_converter _converter;
        const dump_data_t& _dump;
    };
}

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

#include "types/instance.h"
#include "types/primitives_array.h"
#include "hprof.h"

#include <codecvt>
#include <locale>

namespace hprof {
    class string_info_impl_t;

    class string_info_impl_t_deleter {
    public:
        void operator()(string_info_impl_t* ptr) const;
    };

    using string_info_impl_ptr_t = std::unique_ptr<string_info_impl_t, string_info_impl_t_deleter>;

    class string_info_impl_t : public virtual string_info_t, public instance_info_impl_t {
    public:
        using text_converter = std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::little_endian>, char16_t>;
    public:
        string_info_impl_t(const string_info_impl_t&) = delete;
        string_info_impl_t(string_info_impl_t&&) = default;
        
        virtual ~string_info_impl_t();

        string_info_impl_t& operator=(const string_info_impl_t&) = delete;
        string_info_impl_t& operator=(string_info_impl_t&&) = default;

        virtual const std::string& value() const override { return _value; }
    public:
        static string_info_impl_ptr_t create(const instance_info_impl_t& instance, const objects_index_t& objects) {
            auto mem = new (std::nothrow) u_int8_t[instance.data_size() + sizeof(string_info_impl_t)];
            // copy old data
            std::memcpy(mem + sizeof(string_info_impl_t), instance.data(), instance.data_size());
            return string_info_impl_ptr_t { new (mem) string_info_impl_t(instance, objects) };
        }
    private:
        string_info_impl_t(const instance_info_impl_t& obj, const objects_index_t& objects);
    private:
        std::string _value;
        static text_converter _converter;
    };
}

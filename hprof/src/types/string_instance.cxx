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
#include "types/string_instance.h"

using namespace hprof;

void string_info_impl_t_deleter::operator()(string_info_impl_t* ptr) const {
    ptr->~string_info_impl_t();
    delete[] reinterpret_cast<u_int8_t*>(ptr);
}

string_info_impl_t::text_converter string_info_impl_t::_converter {"\xFF ", u"\xFFFF"};

string_info_impl_t::string_info_impl_t(const instance_info_impl_t& obj, const objects_index_t& objects) : 
                instance_info_impl_t(obj, reinterpret_cast<u_int8_t*>(this) + sizeof(string_info_impl_t)) {
    auto field = instance_info_impl_t::fields().find("value");
    if (field == instance_info_impl_t::fields().end() || field->type() != jvm_type_t::JVM_TYPE_OBJECT) {
        return;
    }

    auto item = objects.find_object(static_cast<jvm_id_t>(*field));
    if (item == nullptr || item->type() != heap_item_t::PrimitivesArray) {
        return;
    }

    auto array = static_cast<const primitives_array_info_t *>(*item);
    if (array->item_type() != jvm_type_t::JVM_TYPE_CHAR) {
        return;
    }

    std::u16string text;
    for (auto chr = std::begin(*array); chr != std::end(*array); ++chr) {
        _value += _converter.to_bytes(static_cast<jvm_char_t>(*chr));
    }
}

string_info_impl_t::~string_info_impl_t() {}

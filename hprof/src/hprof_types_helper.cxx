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
#include "hprof_types_helper.h"
#include "hprof_dump_data.h"


using namespace hprof;

bool types_helper_t::is_string(const object_info_t& object) const {
    if (object.type() != object_info_t::TYPE_INSTANCE) {
        return false;
    }

    auto cls = static_cast<const instance_info_t *>(&object)->get_class();
    id_t id = cls->id();
    while (id != 0) {
        if (id == _string_class_id) {
            return true;
        }

        if (cls->super_id() == 0) {
            break;
        }

        cls = _dump.get_class_by_id(cls->super_id());
        id = cls->super_id();
    }

    return false;
}

types_helper_t::string_value_t types_helper_t::get_string_value(const instance_info_t& object, std::string& result) const {
    if (object.class_id() != _string_class_id) {
        return NotString;
    }

    auto& fields = object.fields();
    for (int index = 0, count = fields.count(); index < count; ++index) {
        auto field = fields[index];
        if ("value" == _dump.get_string(field.name_id())) {
            auto id = static_cast<id_t>(field);
            if (id == 0) {
                return Null;
            }
            auto value = _dump.get_object_by_id(id);
            if (value->type() == object_info_t::TYPE_PRIMITIVES_ARRAY) {
                primitive_array_info_t *arr = dynamic_cast<primitive_array_info_t *>(value.get());
                size_t length = arr->length();

                for (int index = 0; index < length; ++index) {
                    char16_t chr = arr->item<jvm_char_t>(index);
                    result += _converter.to_bytes(chr);
                }

                return Value;
            }
        }
    }

    return Invalid;
}

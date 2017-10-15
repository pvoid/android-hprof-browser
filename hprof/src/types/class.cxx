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
#include "types/class.h"

using namespace hprof;

void class_info_impl_t_deleter::operator()(class_info_impl_t* ptr) const {
    ptr->~class_info_impl_t();
    delete[] reinterpret_cast<u_int8_t*>(ptr);
}

class_info_impl_t::~class_info_impl_t() {}

int32_t class_info_impl_t::has_link_to(jvm_id_t id) const {
    int32_t result = 0;
    if (_super_id == id) {
        result |= link_t::TYPE_SUPER;
    }

    if (_class_loader_id == id) {
        result |= link_t::TYPE_CLASS_LOADER;
    }

    for (const auto& field : _static_fields) {
        if (field.type() != jvm_type_t::JVM_TYPE_OBJECT) {
            continue;
        }

        if (static_cast<jvm_id_t>(field) == id) {
            result |= link_t::TYPE_OWNERSHIP;
            break;
        }
    }

    return result;
}

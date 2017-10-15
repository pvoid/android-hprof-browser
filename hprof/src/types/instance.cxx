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
#include "types/instance.h"

using namespace hprof;

void instance_info_impl_t_deleter::operator()(instance_info_impl_t* ptr) const {
    ptr->~instance_info_impl_t();
    delete[] reinterpret_cast<u_int8_t*>(ptr);
}

void instance_info_impl_t::set_class(const heap_item_ptr_t& cls) { 
    _class = cls; 
    const class_info_t* c = static_cast<const class_info_t *>(*cls);
    size_t offset = 0;
    while (c != nullptr) {
        for (auto& field : c->fields()) {
            _fields.add(field.name_id(), field.name(), field.type(), offset + field.offset());
        }
        offset += c->fields().data_size();
        c = c->super();
    }
}

int32_t instance_info_impl_t::has_link_to(jvm_id_t id) const {
    int32_t result = 0;
    
    if (_class_id == id) {
        result |= link_t::TYPE_INSTANCE;
    }

    for (auto& item : _fields) {
        if (item.type() != jvm_type_t::JVM_TYPE_OBJECT) {
            continue;
        }
        jvm_id_t value = static_cast<jvm_id_t>(item);
        if (value == id) {
            result |= link_t::TYPE_INSTANCE;
        }
    }

    return result;
}

instance_info_impl_ptr_t instance_info_impl_t::create(u_int8_t id_size, jvm_id_t id, size_t data_size) {
    auto mem = new (std::nothrow) u_int8_t[sizeof(instance_info_impl_t) + data_size];
    instance_info_impl_ptr_t result { new (mem) instance_info_impl_t(id_size, id, data_size)};
    return result;
}

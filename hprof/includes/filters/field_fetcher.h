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
#include "objects_index.h"

#include <vector>
#include <string>

namespace hprof {
    class field_fetcher_t {
    public:
        field_fetcher_t() {}

        explicit field_fetcher_t(const char* field_name) {
            _fields.emplace_back(field_name);
        }

        void add(const char* field_name) {
            _fields.emplace(std::begin(_fields), field_name);
        }

        template<typename action_t>
        bool apply(const object_info_t* object, const objects_index_t& helper, const action_t& action) const {
            if (_fields.empty()) {
                return false;
            }
            
            auto it = std::begin(_fields);
            for (;;) {
                if (object == nullptr || (object->type() != object_info_t::TYPE_INSTANCE && object->type() != object_info_t::TYPE_STRING)) {
                    return false;
                }

                auto& fields = dynamic_cast<const instance_info_t*>(object)->fields();
                auto field = fields.find(*it);
                
                if (field == std::end(fields)) {
                    return false;
                }
                
                if (++it == std::end(_fields)) {
                    return action(*field);
                }

                if (field->type() != jvm_type_t::JVM_TYPE_OBJECT) {
                    return false;
                }

                object = helper.find_object(static_cast<jvm_id_t>(*field)).get();
            }

            return false;
        }
    private:
        std::vector<std::string> _fields;
    };
}

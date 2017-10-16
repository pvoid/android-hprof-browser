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
        bool apply(const heap_item_ptr_t& object, const objects_index_t& helper, const action_t& action) const {
            if (_fields.empty()) {
                return false;
            }
            
            auto it = std::begin(_fields);
            heap_item_ptr_t item = object;
            for (;;) {
                const instance_info_t* instance = nullptr;

                if (item == nullptr) {
                    return false;
                }

                switch(item->type()) {
                    case heap_item_t::Object:
                        instance = static_cast<const instance_info_t*>(*item);
                        break;
                    case heap_item_t::String:
                        instance = static_cast<const string_info_t*>(*item);
                        break;
                    default:
                        return false;
                }

                assert(instance != nullptr);

                auto& fields = instance->fields();
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

                item = helper.find_object(static_cast<jvm_id_t>(*field));
            }

            return false;
        }
    private:
        std::vector<std::string> _fields;
    };
}

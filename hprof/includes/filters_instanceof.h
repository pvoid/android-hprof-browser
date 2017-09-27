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

#include "filters_base.h"

namespace hprof {
    class filter_instance_of_t : public filter_t {
    public:
        explicit filter_instance_of_t(const std::string& name) : _class_name(name) {}
        virtual ~filter_instance_of_t() {}
        virtual filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
            if (object->type() != object_info_t::TYPE_INSTANCE) {
                return NoMatch;
            }

            const instance_info_t* const instance = static_cast<const instance_info_t* const>(object);
            class_info_ptr_t cls = instance->get_class();
            for(;;) {
                const std::string name = helper.get_string(cls->name_id());
                // TODO: case insencative comparation
                if (name == _class_name) {
                    return Match;
                }

                if (cls->super_id() == 0) {
                    break;
                }

                cls = helper.get_class_by_id(cls->super_id());
            }
            return NoMatch;
        }
    private:
        std::string _class_name;
    };
}

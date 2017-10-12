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

#include "filters/base.h"

namespace hprof {
    class filter_classname_t : public filter_t {
    public:
        explicit filter_classname_t(const char* name) : _name(name) {}
        explicit filter_classname_t(const std::string& name) : _name(name) {}
        virtual ~filter_classname_t() {}
        virtual filter_result_t operator()(const object_info_t* object, const objects_index_t& objects) const override {
            bool result = false;
            switch (object->type()) {
                case object_info_t::TYPE_INSTANCE:
                    result = dynamic_cast<const instance_info_t*>(object)->get_class()->name_matches(_name) == 0;
                    break;
                case object_info_t::TYPE_CLASS:
                    result = dynamic_cast<const class_info_t*>(object)->name_matches(_name) == 0;
                    break;
                case object_info_t::TYPE_STRING:
                    return _name.match( name_tokens { "java.lang.String" }) == 0 ? Match : NoMatch;
                case object_info_t::TYPE_OBJECTS_ARRAY:
                case object_info_t::TYPE_PRIMITIVES_ARRAY:
                    break;
            }
            return result ? Match : NoMatch;
        }
    private:
        name_tokens _name;
    };
}
    
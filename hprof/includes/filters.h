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
#include "filters_logical.h"
#include "filters_instanceof.h"
#include "filters_comparation.h"
#include "filters_apply.h"

#include "hprof_types_base.h"
#include "hprof_types_class.h"
#include "hprof_types_instance.h"
#include "name_tokenizer.h"

#include <string>
#include <cstring>
#include <memory>

namespace hprof {

    class filter_class_name_t : public filter_t {
    public:
        explicit filter_class_name_t(const char* name) : _name(name) {}
        explicit filter_class_name_t(const std::string& name) : _name(name) {}
        virtual ~filter_class_name_t() {}
        virtual filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
            bool result = false;
            switch (object->type()) {
                case object_info_t::TYPE_INSTANCE:
                    result = static_cast<const instance_info_t* const>(object)->get_class()->tokens().match(_name) == 0;
                    break;
                case object_info_t::TYPE_CLASS:
                    result = static_cast<const class_info_t* const>(object)->tokens().match(_name) == 0;
                    break;
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

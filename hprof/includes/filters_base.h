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

#include "hprof_types_helper.h"
#include "filters_value.h"

namespace hprof {
    struct filter_helper_t {
        virtual object_info_ptr_t get_object_by_id(id_t id) const = 0;
        virtual class_info_ptr_t get_class_by_id(id_t id) const = 0;
        virtual const std::string& get_string(id_t id) const = 0;
        virtual const types_helper_t& types_helper() const = 0;
    };

    class filter_t {
    public:
        enum filter_result_t : int {
            Match,
            NoMatch,
            Fail
        };
    public:
        virtual ~filter_t() {}
        virtual filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const = 0;
    };

    class filter_fetch_all_t : public filter_t {
    public:
        virtual ~filter_fetch_all_t() {}
        virtual filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
            return Match;
        }
    };

    class filter_by_field_t : public filter_t {
    public:
        filter_by_field_t(const char* field_name) : _field_name(field_name) {}
        virtual ~filter_by_field_t() {}

        virtual filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
            if (object->type() != object_info_t::TYPE_INSTANCE) {
                return NoMatch;
            }

            const instance_info_t* instance = static_cast<const instance_info_t*>(object);
            auto& fields = instance->fields();
            for (int index = 0, length = fields.count(); index < length; ++index) {
                if (_field_name == helper.get_string(fields[index].name_id())) {
                    return match(fields[index], helper) ? Match : NoMatch;
                }
            }

            return NoMatch;
        }
    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const = 0;

    protected:
        std::string _field_name;
    };
}

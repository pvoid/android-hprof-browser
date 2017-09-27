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

#include <memory>

namespace hprof {
    class filter_apply_filter_t : public filter_by_field_t {
        public:
            filter_apply_filter_t(const char* field_name, std::unique_ptr<filter_t>&& filter) :
                filter_by_field_t(field_name), _filter(std::move(filter)) {}
            virtual ~filter_apply_filter_t() {}
        protected:
            virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
                if (field.type() != JVM_TYPE_OBJECT) {
                    return false;
                }

                id_t id = static_cast<id_t>(field);
                if (id == 0) {
                    return false;
                }

                object_info_ptr_t value = helper.get_object_by_id(id);
                if (value == nullptr) {
                    return false;
                }

                return (*_filter)(value.get(), helper) == Match;
            }
        private:
            std::unique_ptr<filter_t> _filter;
    };
}

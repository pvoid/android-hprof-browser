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

#include <memory>

namespace hprof {
    class filter_apply_to_field_t : public filter_by_field_t {
        public:
            filter_apply_to_field_t(field_fetcher_t *fetcher, std::unique_ptr<filter_t>&& filter) :
                filter_by_field_t(fetcher), _filter(std::move(filter)) {}
            virtual ~filter_apply_to_field_t() {}
        protected:
            virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
                if (field.type() != jvm_type_t::JVM_TYPE_OBJECT) {
                    return false;
                }

                object_info_ptr_t value = objects.find_object(static_cast<jvm_id_t>(field));
                if (value == nullptr) {
                    return false;
                }

                return (*_filter)(value.get(), objects) == Match;
            }
        private:
            std::unique_ptr<filter_t> _filter;
    };
}

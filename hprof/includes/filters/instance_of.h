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

#include <iostream>

namespace hprof {
    class filter_instance_of_t : public filter_t {
    public:
        explicit filter_instance_of_t(const std::string& name) : _class_name(name) {}
        virtual ~filter_instance_of_t() {}
        virtual filter_result_t operator()(const heap_item_ptr_t& item, const objects_index_t&) const override {
            if (item == nullptr) {
                return NoMatch;
            }

            const class_info_t* cls = nullptr;
            
            switch (item->type()) {
                case heap_item_t::Object:
                    cls = static_cast<const instance_info_t*>(*item)->get_class();
                    break;
                case heap_item_t::String:
                    cls = static_cast<const string_info_t*>(*item)->get_class();
                    break;
                case heap_item_t::Class:
                case heap_item_t::PrimitivesArray:
                case heap_item_t::ObjectsArray:
                    break;
            }

            while(cls != nullptr) {
                // TODO: fuzzy case insensitive comparaison
                if (cls->name() == _class_name) {
                    return Match;
                }

                cls = cls->super();
            }
            return NoMatch;
        }
    private:
        std::string _class_name;
    };
}

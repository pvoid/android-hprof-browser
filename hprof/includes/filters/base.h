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

#include "types/fields.h"
#include "filters/field_fetcher.h"

namespace hprof {
    class filter_t {
    public:
        enum filter_result_t : int {
            Match,
            NoMatch,
            Fail
        };
    public:
        virtual ~filter_t() {}
        virtual filter_result_t operator()(const heap_item_ptr_t& item, const objects_index_t& objects) const = 0;
    };

    class filter_fetch_all_t : public filter_t {
    public:
        virtual ~filter_fetch_all_t() {}
        virtual filter_result_t operator()(const heap_item_ptr_t&, const objects_index_t&) const override {
            return Match;
        }
    };

    class filter_by_field_t : public filter_t {
    public:
        explicit filter_by_field_t(field_fetcher_t *fetcher) : _field_fetcher(fetcher) {}
        virtual ~filter_by_field_t() {}

        virtual filter_result_t operator()(const heap_item_ptr_t& item, const objects_index_t& objects) const override {
            if (item->type() != heap_item_t::Object && item->type() != heap_item_t::String) {
                return NoMatch;
            }

            if (_field_fetcher->apply(item, objects, [this, &objects] (auto& field) -> bool { return this->match(field, objects); })) {
                return Match;
            }

            return NoMatch;
        }
    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const = 0;

    protected:
        std::unique_ptr<field_fetcher_t> _field_fetcher;
    };
}

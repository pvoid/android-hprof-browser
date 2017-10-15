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
#include "heap_profile.h"
#include <cassert>

using namespace hprof;

heap_profile_impl_t::heap_profile_impl_t(gc_roots_t&& roots) : _has_error(false) {
    _roots = std::move(roots);
}

heap_profile_impl_t::~heap_profile_impl_t() {}

heap_item_ptr_t heap_profile_impl_t::find_object(jvm_id_t id) const {
    auto it = _objects.find(id);
    if (it == std::end(_objects)) {
        return heap_item_ptr_t {};
    }

    return it->second;
}

heap_item_ptr_t heap_profile_impl_t::find_class(jvm_id_t id) const {
    auto it = _classes.find(id);
    if (it == std::end(_classes)) {
        return heap_item_ptr_t {};
    }

    return it->second;
}

bool heap_profile_impl_t::query(const query_t& query, std::vector<heap_item_ptr_t>& result) const {
    switch (query.source) {
        case query_t::SOURCE_CLASSES:
            return query_classes(*query.filter, result);
        case query_t::SOURCE_OBJECTS:
            return query_instances(*query.filter, result);
    }

    return false;
}

void heap_profile_impl_t::add(jvm_id_t id, const heap_item_ptr_t& item) {
    if (item->type() == heap_item_t::Class) _classes.emplace(id, item);
    else _objects.emplace(id, item);
}

bool heap_profile_impl_t::query_classes(const filter_t& filter, std::vector<heap_item_ptr_t>& result) const {
    for (auto item : _classes) {
        switch (filter(item.second, *this)) {
            case filter_t::Match:
                result.push_back(item.second);
                continue;
            case filter_t::NoMatch:
                continue;
            case filter_t::Fail:
                return false;
        }
        assert(false);
    }
    return true;
}

bool heap_profile_impl_t::query_instances(const filter_t& filter, std::vector<heap_item_ptr_t>& result) const {
    for (auto& item : _objects) {
        switch (filter(item.second, *this)) {
            case filter_t::Match:
                result.push_back(item.second);
                continue;
            case filter_t::NoMatch:
                continue;
            case filter_t::Fail:
                return false;
        }
        assert(false);
    }
    return true;
}

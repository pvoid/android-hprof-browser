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
#include "hprof_dump_data.h"

#include <algorithm>
#include <iostream>
#include <cassert>

using namespace hprof;

const std::string g_unknown_string = "<unknown string>";

bool dump_data_t::add(const std::vector<gc_root_t>& roots) {
    if (_is_index_built) {
        return false;
    }

    std::copy(std::begin(roots), std::end(roots), std::back_inserter(_gc_roots));
    return true;
}

bool dump_data_t::add(const instances_map_t& instances) {
    if (_is_index_built) {
        return false;
    }

    _all.insert(std::begin(instances), std::end(instances));
    return true;
}

bool dump_data_t::add(const classes_map_t& classes) {
    if (_is_index_built) {
        return false;
    }

    _classes.insert(std::begin(classes), std::end(classes));
    return true;
}

bool dump_data_t::add(const array_map_t& arrays) {
    if (_is_index_built) {
        return false;
    }

    _all.insert(std::begin(arrays), std::end(arrays));
    return true;
}

bool dump_data_t::add(const strings_map_t& strings) {
    if (_is_index_built) {
        return false;
    }

    _strings.insert(std::begin(strings), std::end(strings));
    return true;
}

bool dump_data_t::analyze() {
    if (_is_index_built) {
        return false;
    }

    prepare_gc_roots();
    prepare_classes();
    prepare_instances();

    _is_index_built = true;
    return true;
}

bool dump_data_t::prepare_gc_roots() {
    for(auto& root : _gc_roots) {
        auto item = _all.find(root.object_id);
        if (item != std::end(_all)) {
            assert(item->second != nullptr);
            item->second->add_root(root);
            continue;
        }

        auto cls = _classes.find(root.object_id);
        if (cls != std::end(_classes)) {
            assert(cls->second != nullptr);
            cls->second->add_root(root);
            continue;
        }

        assert(0);
    }
    _gc_roots.clear();

    return true;
}

bool dump_data_t::prepare_classes() {
    for (auto current = std::begin(_classes); current != std::end(_classes); ++current) {
        auto name = _strings.find(current->second->name_id);
        if (name == std::end(_strings)) {
            continue;
        }
        current->second->tokens.set(name->second);
    }
    return true;
}

bool dump_data_t::prepare_instances() {
    for (auto current = std::begin(_all); current != std::end(_all); ++current) {
        if (current->second->type() != object_info_t::TYPE_INSTANCE) {
            continue;
        }

        instance_info_t* info = reinterpret_cast<instance_info_t*>(current->second.get());
        assert(info->class_instance == nullptr);
        info->class_instance = _classes[info->class_id];
        info->class_instance->instances.push_back(current->second);
    }

    return true;
}

void dump_data_t::print_stats() const {
    std::cout << "Instances: " << _all.size() << std::endl;
}

bool dump_data_t::get_classes(std::vector<class_info_ptr_t>& classes) const {
    for (auto it = std::begin(_classes); it != std::end(_classes); ++it) {
        classes.push_back(it->second);
    }
    return true;
}

const std::string& dump_data_t::get_string(id_t id) const {
    auto item = _strings.find(id);
    if (item == std::end(_strings)) {
        return g_unknown_string;
    }

    return item->second;
}

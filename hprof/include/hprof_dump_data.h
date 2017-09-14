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

#include "hprof_types.h"

namespace hprof {
    struct hprof_name_entry {
        char letter;
        // std::unordered_map<char, hprof_name_entry> nodes;
        std::vector<class_info_ptr_t> classes;

        hprof_name_entry() : letter(0) {
        }
    };

    class dump_data_t {
    public:
        dump_data_t(size_t id_size, const time_t& time) : _id_size(id_size), _time(time), _is_index_built(false) {}
        ~dump_data_t() {}

        size_t id_size() const { return _id_size; }
        const time_t& time() const { return _time; }

        bool add(const std::vector<gc_root_t>& roots);
        bool add(const instances_map_t& instances);
        bool add(const classes_map_t& classes);
        bool add(const strings_map_t& strings);
        bool add(const array_map_t& arrays);
        void print_stats() const;
        bool analyze();
        bool get_classes(std::vector<class_info_ptr_t>& classes) const;
        const std::string& get_string(id_t id) const;

    private:
        bool append_gc_roots();
        bool append_classes_to_instances();
        bool build_classes_search_tree();
    private:
        size_t _id_size;
        time_t _time;
        bool _is_index_built;

        std::unordered_map<id_t, object_info_ptr_t> _all;
        std::vector<gc_root_t> _gc_roots;
        classes_map_t _classes;
        strings_map_t _strings;
        hprof_name_entry _classes_root;
    };
}

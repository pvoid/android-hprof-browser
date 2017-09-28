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
#include "filters.h"
#include "query.h"
#include "hprof_types_helper.h"

namespace hprof {
    class dump_data_t : filter_helper_t {
    public:
        dump_data_t(size_t id_size, const time_t& time) :
            _id_size(id_size), _time(time), _is_index_built(false), _types_helper(std::make_unique<types_helper_t>(0, *this)) {}
        ~dump_data_t() {}

        size_t id_size() const { return _id_size; }
        const time_t& time() const { return _time; }

        bool analyze();
        bool add(const std::vector<gc_root_t>& roots);
        bool add(const std::vector<object_info_ptr_t>& instances);
        bool add(const classes_map_t& classes);
        bool add(const strings_map_t& strings);
        bool add(const array_map_t& arrays);
        void print_stats() const;
        bool query(const query_t& query, std::vector<object_info_ptr_t>& result) const;
        virtual const std::string& get_string(id_t id) const override;
        virtual class_info_ptr_t get_class_by_id(id_t id) const override;
        virtual const types_helper_t& types_helper() const override { return *_types_helper; }
        virtual object_info_ptr_t get_object_by_id(id_t id) const override;
    private:
        bool prepare_gc_roots();
        bool prepare_instances();
        bool prepare_classes();

        bool query_classes(const filter_t& filter, std::vector<object_info_ptr_t>& result) const;
        bool query_instances(const filter_t& filter, std::vector<object_info_ptr_t>& result) const;
    private:
        size_t _id_size;
        time_t _time;
        bool _is_index_built;

        std::unordered_map<id_t, object_info_ptr_t> _all;
        std::vector<gc_root_t> _gc_roots;
        classes_map_t _classes;
        strings_map_t _strings;
        std::unique_ptr<types_helper_t> _types_helper;
    };
}

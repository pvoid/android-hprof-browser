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

#include "hprof.h"
#include "types/gc_root.h"

#include <unordered_map>
#include <vector>
#include <algorithm>

namespace hprof {
    class heap_profile_impl_t : public heap_profile_t, public objects_index_t, public classes_index_t {
        using heap_items_map_t = std::unordered_map<jvm_id_t, heap_item_ptr_t>;
        using gc_roots_t = std::vector<gc_root_impl_ptr_t>;
    public:
        heap_profile_impl_t(gc_roots_t&& roots);
        heap_profile_impl_t(const std::string& message) : _has_error(true), _error_message(message) {}
        ~heap_profile_impl_t();

        virtual bool has_errors() const override { return _has_error; }
        virtual const std::string& error_message() const override { return _error_message; }
        
        virtual heap_item_ptr_t find_object(jvm_id_t id) const override;
        virtual heap_item_ptr_t find_class(jvm_id_t id) const override;

        virtual bool query(const query_t& query, std::vector<heap_item_ptr_t>& result) const override;

        virtual const objects_index_t& objects_index() const override { return *this; }
        virtual const classes_index_t& classes_index() const override { return *this; }

        void add(jvm_id_t id, const heap_item_ptr_t& item);
    private:
        bool query_classes(const filter_t& filter, std::vector<heap_item_ptr_t>& result) const;
        bool query_instances(const filter_t& filter, std::vector<heap_item_ptr_t>& result) const;
        
    private:
        bool _has_error;
        std::string _error_message;
        std::unordered_map<jvm_id_t, heap_item_ptr_t> _objects;
        std::unordered_map<jvm_id_t, heap_item_ptr_t> _classes;
        gc_roots_t _roots;
    };
}

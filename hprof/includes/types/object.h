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

#include "types.h"
#include "types/gc_root.h"

namespace hprof {
    class object_info_impl_t : public virtual object_info_t {
    public:
        object_info_impl_t(u_int8_t id_size, jvm_id_t id) : _object_id(id), _id_size(id_size), _heap_type(heap_info_t::HEAP_UNKNOWN) {}
        object_info_impl_t(const object_info_impl_t& src) {
            operator=(src);
        }
        virtual ~object_info_impl_t() {}

        object_info_impl_t& operator=(const object_info_impl_t& src) {
            _object_id = src._object_id;
            _id_size = src._id_size;
            _heap_type = src._heap_type;
            std::transform(std::begin(src._roots), std::end(src._roots), std::end(_roots), 
                [] (auto& item) -> auto { return std::make_unique<gc_root_impl_t>(*(gc_root_impl_t *)item.get()); });
            return *this;
        }

        virtual size_t id_size() const override { return _id_size; }
        virtual jvm_id_t id() const override { return _object_id; }

        virtual int32_t heap_type() const override { return _heap_type; }
        void set_heap_type(int32_t heap_type) { _heap_type = heap_type; }
        
        virtual const std::vector<std::unique_ptr<gc_root_t>>& gc_roots() const override { return _roots; }
        void add_root(const gc_root_impl_t& root) { _roots.push_back(std::make_unique<gc_root_impl_t>(root)); }
    private:
        jvm_id_t _object_id;
        u_int8_t _id_size;
        int32_t _heap_type;
        std::vector<std::unique_ptr<gc_root_t>> _roots;
    };

    using object_info_impl_ptr_t = std::shared_ptr<object_info_impl_t>;
}

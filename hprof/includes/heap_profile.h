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
#include "hprof.h"
#include "types/object.h"
#include "types/class.h"
#include "types/gc_root.h"

#include <unordered_map>
#include <vector>
#include <algorithm>

namespace hprof {
    class heap_profile_impl_t : public heap_profile_t {
        using objects_t = std::vector<object_info_impl_ptr_t>;
        using classes_t = std::vector<class_info_impl_ptr_t>;
        using gc_roots_t = std::vector<gc_root_impl_ptr_t>;
    public:
        heap_profile_impl_t(const objects_t& objects, const classes_t& classes, gc_roots_t&& roots) : _has_error(false) {
            std::transform(std::begin(objects), std::end(objects), std::inserter(_objects, std::end(_objects)),
                [] (auto item) -> auto { return std::make_pair(item->id(), item); });

            std::transform(std::begin(classes), std::end(classes), std::inserter(_classes, std::end(_classes)),
                [] (auto item) -> auto { return std::make_pair(item->id(), item); });

            _roots = std::move(roots);
        }
        heap_profile_impl_t(const std::string& message) : _has_error(true), _error_message(message) {}
        ~heap_profile_impl_t() {}

        virtual bool has_errors() const override { return _has_error; }
        virtual const std::string& error_message() const override { return _error_message; }

        virtual object_info_ptr_t find_object(jvm_id_t id) const override {
            auto it = _objects.find(id);
            if (it == std::end(_objects)) {
                return object_info_ptr_t {};
            }

            return it->second;
        }

        bool query(const query_t& query, std::vector<object_info_ptr_t>& result) const override {
            switch (query.source) {
                case query_t::SOURCE_CLASSES:
                    return query_classes(*query.filter, result);
                case query_t::SOURCE_OBJECTS:
                    return query_instances(*query.filter, result);
            }
        
            return false;
        }
    private:
        bool query_classes(const filter_t& filter, std::vector<object_info_ptr_t>& result) const {
            for (auto item : _classes) {
                switch (filter(item.second.get(), *this)) {
                    case filter_t::Match:
                        result.push_back(item.second);
                        break;
                    case filter_t::NoMatch:
                        continue;
                    case filter_t::Fail:
                    default:
                        return false;
                }
            }
        
            return true;
        }
        
        bool query_instances(const filter_t& filter, std::vector<object_info_ptr_t>& result) const {
            for (auto& item : _objects) {
                switch (filter(item.second.get(), *this)) {
                    case filter_t::Match:
                        result.push_back(item.second);
                        break;
                    case filter_t::NoMatch:
                        continue;
                    case filter_t::Fail:
                    default:
                        return false;
                }
            }
        
            return true;
        }
        
    private:
        bool _has_error;
        std::string _error_message;
        std::unordered_map<jvm_id_t, object_info_ptr_t> _objects;
        std::unordered_map<jvm_id_t, class_info_ptr_t> _classes;
        gc_roots_t _roots;
    };
}
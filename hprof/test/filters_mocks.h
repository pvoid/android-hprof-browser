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

#include "filters.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace hprof;

class filter_mock_t : public filter_t {
public:
    explicit filter_mock_t(filter_t::filter_result_t value) : _value(value) {}
    virtual ~filter_mock_t() {}

    virtual filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
        return _value;
    }
private:
    filter_t::filter_result_t _value;
};

class mock_filter_helper_t : public filter_helper_t {
public:
    virtual class_info_ptr_t get_class_by_id(hprof::id_t id) const override {
        auto item = std::find_if(std::begin(_classes), std::end(_classes),
            [id] (auto& cls) -> bool { return cls->id() == id; });

        if (item == std::end(_classes)) {
            return nullptr;
        }
        return *item;
    }

    virtual const std::string& get_string(hprof::id_t id) const override {
        auto item = _strings.find(id);
        if (item == std::end(_strings)) {
            return _empty;
        }
        return item->second;
    }

    virtual object_info_ptr_t get_object_by_id(hprof::id_t id) const override {
        return object_info_ptr_t {};
    }

    virtual const types_helper_t& types_helper() const override {

    }

    void add(class_info_ptr_t cls) {
        _classes.push_back(cls);
    }

    void add(hprof::id_t id, const std::string& text) {
        _strings.emplace(id, text);
    }
private:
    static const std::string _empty;
    std::vector<class_info_ptr_t> _classes;
    std::unordered_map<hprof::id_t, std::string> _strings;
};

template <typename filter_type>
inline filter_type create_filter(filter_t::filter_result_t base_value) {
    return filter_type { std::move(std::make_unique<filter_mock_t>(base_value)) };
}

template <typename filter_type>
inline filter_type create_filter(filter_t::filter_result_t base_value_left, filter_t::filter_result_t base_value_right) {
    return filter_type { std::move(std::make_unique<filter_mock_t>(base_value_left)),
        std::move(std::make_unique<filter_mock_t>(base_value_right))};
}

template <typename filter_type, typename... mock_results>
inline filter_t::filter_result_t apply_filter(mock_results... base_values) {
    class_info_impl_t instance { 4 };
    return create_filter<filter_type>(base_values...)(&instance, mock_filter_helper_t {});
}

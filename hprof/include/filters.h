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

#include "hprof_types_base.h"
#include "hprof_types_class.h"
#include "hprof_types_instance.h"
#include "name_tokenizer.h"

#include <string>
#include <memory>

namespace hprof {
    struct filter_helper_t {
        virtual class_info_ptr_t get_class_by_id(id_t id) const = 0;
        virtual const std::string& get_string(id_t id) const = 0;
    };

    class filter_t {
    public:
        enum filter_result_t : int {
            Match,
            NoMatch,
            Fail
        };
    public:
        virtual ~filter_t() {}
        virtual filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const = 0;
    };

    class filter_fetch_all_t : public filter_t {
    public:
        virtual ~filter_fetch_all_t() {}
        virtual filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
            return Match;
        }
    };

    class filter_not_t : public filter_t {
    public:
        filter_not_t(std::unique_ptr<filter_t>&& src) : _filter(std::move(src)) {}
        filter_not_t(const filter_not_t&) = delete;
        filter_not_t(filter_not_t&&) = default;
        virtual ~filter_not_t() {}

        filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
            switch ((*_filter)(object, helper)) {
                case Match:
                    return NoMatch;
                case NoMatch:
                    return Match;
                case Fail:
                    return Fail;
            }
        }
    private:
        std::unique_ptr<filter_t> _filter;
    };

    class filter_and_t : public filter_t {
    public:
        filter_and_t(std::unique_ptr<filter_t>&& left, std::unique_ptr<filter_t>&& right) : _left(std::move(left)), _right(std::move(right)) {
        }
        filter_and_t(const filter_and_t&) = delete;
        filter_and_t(filter_and_t&&) = default;
        virtual ~filter_and_t() {}
        filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
            auto left_result = (*_left)(object, helper);
            if (left_result == Fail) {
                return Fail;
            }
            auto right_result = (*_right)(object, helper);
            if (right_result == Fail) {
                return Fail;
            }

            return left_result == Match && right_result == Match ? Match : NoMatch;
        }
    private:
        std::unique_ptr<filter_t> _left;
        std::unique_ptr<filter_t> _right;
    };

    class filter_or_t : public filter_t {
    public:
        filter_or_t(std::unique_ptr<filter_t>&& left, std::unique_ptr<filter_t>&& right) : _left(std::move(left)), _right(std::move(right)) {
        }
        filter_or_t(const filter_or_t&) = delete;
        filter_or_t(filter_or_t&&) = default;
        virtual ~filter_or_t() {}
        filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
            auto left_result = (*_left)(object, helper);
            if (left_result == Fail) {
                return Fail;
            }
            auto right_result = (*_right)(object, helper);
            if (right_result == Fail) {
                return Fail;
            }

            return left_result == Match || right_result == Match ? Match : NoMatch;
        }
    private:
        std::unique_ptr<filter_t> _left;
        std::unique_ptr<filter_t> _right;
    };

    class filter_class_name_t : public filter_t {
    public:
        explicit filter_class_name_t(const char* name) : _name(name) {}
        explicit filter_class_name_t(const std::string& name) : _name(name) {}
        virtual ~filter_class_name_t() {}
        virtual filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
            bool result = false;
            switch (object->type()) {
                case object_info_t::TYPE_INSTANCE:
                    result = static_cast<const instance_info_t* const>(object)->get_class()->tokens().match(_name) == 0;
                    break;
                case object_info_t::TYPE_CLASS:
                    result = static_cast<const class_info_t* const>(object)->tokens().match(_name) == 0;
                    break;
                case object_info_t::TYPE_OBJECTS_ARRAY:
                case object_info_t::TYPE_PRIMITIVES_ARRAY:
                    break;
            }
            return result ? Match : NoMatch;
        }
    private:
        name_tokens _name;
    };

    class filter_instance_of_t : public filter_t {
    public:
        explicit filter_instance_of_t(const std::string& name) : _class_name(name) {}
        ~filter_instance_of_t() {}
        virtual filter_result_t operator()(const object_info_t* const object, const filter_helper_t& helper) const override {
            if (object->type() != object_info_t::TYPE_INSTANCE) {
                return NoMatch;
            }

            const instance_info_t* const instance = static_cast<const instance_info_t* const>(object);
            class_info_ptr_t cls = instance->get_class();
            for(;;) {
                const std::string name = helper.get_string(cls->name_id());
                if (name == _class_name) {
                    return Match;
                }

                if (cls->super_id() == 0) {
                    break;
                }

                cls = helper.get_class_by_id(cls->super_id());
            }
            return NoMatch;
        }
    private:
        std::string _class_name;
    };
}

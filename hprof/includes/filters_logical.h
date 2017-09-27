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

#include "filters_base.h"

namespace hprof {
    class filter_not_t : public filter_t {
    public:
        filter_not_t(std::unique_ptr<filter_t>&& src) : _filter(std::move(src)) {}
        filter_not_t(const filter_not_t&) = delete;
        filter_not_t(filter_not_t&&) = default;
        virtual ~filter_not_t() {}

        filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
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
        filter_and_t(std::unique_ptr<filter_t>&& left, std::unique_ptr<filter_t>&& right) : _left(std::move(left)), _right(std::move(right)) {}
        filter_and_t(filter_t* left, filter_t* right) : _left(left), _right(right) {
            assert(left != right);
        }
        filter_and_t(const filter_and_t&) = delete;
        filter_and_t(filter_and_t&&) = default;
        virtual ~filter_and_t() {}
        filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
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
        filter_or_t(std::unique_ptr<filter_t>&& left, std::unique_ptr<filter_t>&& right) : _left(std::move(left)), _right(std::move(right)) {}
        filter_or_t(filter_t* left, filter_t* right) : _left(left), _right(right) {}
        filter_or_t(const filter_or_t&) = delete;
        filter_or_t(filter_or_t&&) = default;
        virtual ~filter_or_t() {}
        filter_result_t operator()(const object_info_t* object, const filter_helper_t& helper) const override {
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
}

///
/// Copyright 2017 Dmitry `PVOID` Petukhov
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///      http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
#pragma once

#include <string>
#include <cstring>
#include <vector>
#include <cassert>
#include <limits>
#include <algorithm>

namespace hprof {
    class class_name_tokenizer {
    public:
        typedef std::ptrdiff_t difference_type;
        typedef std::string value_type;
        typedef std::string& reference;
        typedef std::string* pointer;
        typedef std::input_iterator_tag iterator_category;
    public:
        explicit class_name_tokenizer(const char* item, size_t max_length) : _current(item), _last(item + max_length) {
            _current_end = next_token_start(item);
        }
        bool operator==(const class_name_tokenizer& other) const { return _current == other._current; }
        bool operator!=(const class_name_tokenizer& other) const { return _current != other._current; }
        std::string operator*() {
            return std::string { _current, _current_end };
        }

        class_name_tokenizer& operator++() {
            switch_to_next_token();
            return *this;
        }

        class_name_tokenizer operator++(int) {
            auto result = *this;
            switch_to_next_token();
            return result;
        }
    private:
        const char* next_token_start(const char* text) {
            while (++text < _last && *text != '.');
            return text;
        }

        void switch_to_next_token() {
            assert(_current != _last);

            if (_current_end == _last) {
                _current = _current_end;
                return;
            }

            _current = _current_end + 1;
            _current_end = next_token_start(_current_end);
        }
    private:
        const char* _current;
        const char* _current_end;
        const char* _last;
    };

    class name_tokens {
    public:
        name_tokens() {}
        // cppcheck-suppress noExplicitConstructor
        name_tokens(const std::string& class_name);
        // cppcheck-suppress noExplicitConstructor
        name_tokens(const char* class_name);
        void set(const std::string& class_name);
        void set(const char* class_name);
        size_t match(const name_tokens& token) const;
        bool operator==(const name_tokens& src) const { return _tokens == src._tokens; }
    private:
        static size_t match(const std::string& left, const std::string& right);
    private:
        std::vector<std::string> _tokens;
    };


    inline name_tokens::name_tokens(const std::string& class_name) {
        set(class_name);
    }

    inline name_tokens::name_tokens(const char* class_name) {
        set(class_name);
    }

    inline void name_tokens::set(const std::string& class_name) {
        const size_t length = class_name.length();
        class_name_tokenizer begin { class_name.c_str(), length };
        class_name_tokenizer end { class_name.c_str() + length, 0 };

        _tokens.clear();
        std::copy(begin, end, std::back_inserter(_tokens));
    }

    inline void name_tokens::set(const char* class_name) {
        const size_t length = std::strlen(class_name);
        class_name_tokenizer begin { class_name, length };
        class_name_tokenizer end { class_name + length, 0 };

        _tokens.clear();
        std::copy(begin, end, std::back_inserter(_tokens));
    }

    inline size_t name_tokens::match(const name_tokens& token) const {
        size_t result = std::numeric_limits<size_t>::max();
        if (token._tokens.size() == 1) {
            for (auto& item : _tokens) {
                if (match(item, token._tokens[0]) == 0) {
                    result = 0;
                    break;
                }
            }
        } else {
            if (_tokens.size() > 1 && _tokens.size() >= token._tokens.size()) {
                if (std::equal(std::begin(token._tokens), std::end(token._tokens), std::begin(_tokens),
                    [] (auto& left, auto& right) -> bool { return match(right, left) == 0; })) {
                    result = 0;
                }
            }
        }
        return result;
    }

    // TODO: Add more smart comparasion inside
    inline size_t name_tokens::match(const std::string& left, const std::string& right) {
        size_t result = std::numeric_limits<size_t>::max();

        if (right.length() <= left.length()) {
            if (std::equal(std::begin(right), std::end(right), std::begin(left),
                           [] (auto& left_char, auto& right_char) -> bool { return std::toupper(left_char) == std::toupper(right_char); })) {
                result = 0;
            }
        }
        return result;
    }
}

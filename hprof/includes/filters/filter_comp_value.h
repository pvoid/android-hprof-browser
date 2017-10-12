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

#include <iostream>
#include <cstring>

namespace hprof {
    struct filter_comp_value_t {
        enum {
            TYPE_INT,
            TYPE_DOUBLE,
            TYPE_BOOL,
            TYPE_TEXT
        } type;

        union {
            bool bool_value;
            int64_t int_value;
            int64_t double_value;
            char* text_value;
        };

        explicit filter_comp_value_t(int value) : type(TYPE_INT), int_value(value) {}

        explicit filter_comp_value_t(bool value) : type(TYPE_BOOL), bool_value(value) {}

        explicit filter_comp_value_t(double value) : type(TYPE_DOUBLE), double_value(value) {}

        explicit filter_comp_value_t(const char* value) : type(TYPE_TEXT), text_value(nullptr) {
            operator=(value);
        }

        filter_comp_value_t(const filter_comp_value_t& src) : type(src.type), text_value(nullptr) {
            operator=(src);
        }

        ~filter_comp_value_t() {
            if (type == TYPE_TEXT && text_value != nullptr) {
                delete[] text_value;
            }
        }


        filter_comp_value_t& operator=(int64_t value) {
            type = TYPE_INT;
            int_value = value;
            return *this;
        }

        filter_comp_value_t& operator=(bool value) {
            type = TYPE_BOOL;
            bool_value = value;
            return *this;
        }

        filter_comp_value_t& operator=(double value) {
            type = TYPE_DOUBLE;
            double_value = value;
            return *this;
        }

        filter_comp_value_t& operator=(const char* value) {
            type = TYPE_TEXT;
            if (value == nullptr) {
                text_value = nullptr;
            } else if (value != text_value) {
                if (text_value != nullptr) {
                    delete[] text_value;
                }

                size_t len = std::strlen(value);
                text_value = new (std::nothrow) char[len + 1];
                std::strcpy(text_value, value);
            }
            return *this;
        }

        filter_comp_value_t& operator=(const filter_comp_value_t& src) {
            switch(type) {
                case TYPE_INT:
                    operator=(src.int_value);
                    break;
                case TYPE_DOUBLE:
                    operator=(src.double_value);
                    break;
                case TYPE_BOOL:
                    operator=(src.bool_value);
                    break;
                case hprof::filter_comp_value_t::TYPE_TEXT: {
                    operator=(src.text_value);
                    break;
                }
            }
            return *this;
        }
    };

    inline std::ostream& operator<<(std::ostream& out, filter_comp_value_t& value) {
        switch(value.type) {
            case filter_comp_value_t::TYPE_INT:
                out << "int(" << value.int_value << ")";
                break;
            case filter_comp_value_t::TYPE_DOUBLE:
                out << "double(" << value.double_value << ")";
                break;
            case filter_comp_value_t::TYPE_BOOL:
                out << "bool(" << (value.bool_value ? "true" : "false") << ")";
                break;
            case filter_comp_value_t::TYPE_TEXT:
                out << "text(\"" << value.text_value << "\")";
                break;
        }
        return out;
    }

    inline bool operator==(const filter_comp_value_t& left, int64_t value) {
        switch (left.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left.int_value == value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left.double_value == value;
        }
    }

    inline bool operator==(const filter_comp_value_t& left, float value) {
        switch (left.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left.int_value == value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left.double_value == value;
        }
    }

    inline bool operator==(const filter_comp_value_t& left, double value) {
        switch (left.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left.int_value == value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left.double_value == value;
        }
    }

    inline bool operator!=(const filter_comp_value_t& left, int64_t value) {
        switch (left.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left.int_value != value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left.double_value != value;
        }
    }

    inline bool operator!=(const filter_comp_value_t& left, float value) {
        switch (left.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left.int_value != value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left.double_value != value;
        }
    }

    inline bool operator!=(const filter_comp_value_t& left, double value) {
        switch (left.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left.int_value != value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left.double_value != value;
        }
    }

    inline bool operator<(int64_t left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left < right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left < right.double_value;
        }
    }

    inline bool operator<(float left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left < right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left < right.double_value;
        }
    }

    inline bool operator<(double left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left < right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left < right.double_value;
        }
    }

    inline bool operator<=(int64_t left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left <= right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left <= right.double_value;
        }
    }

    inline bool operator<=(float left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left <= right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left <= right.double_value;
        }
    }

    inline bool operator<=(double left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left <= right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left <= right.double_value;
        }
    }

    inline bool operator>( int64_t left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left > right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left > right.double_value;
        }
    }

    inline bool operator>(float left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left > right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left > right.double_value;
        }
    }

    inline bool operator>(double left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left > right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left > right.double_value;
        }
    }

    inline bool operator>=(int64_t left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left >= right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left >= right.double_value;
        }
    }

    inline bool operator>=(float left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left >= right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left >= right.double_value;
        }
    }

    inline bool operator>=( double left, const filter_comp_value_t& right) {
        switch (right.type) {
            case filter_comp_value_t::TYPE_BOOL:
            case filter_comp_value_t::TYPE_TEXT:
                return false;
            case filter_comp_value_t::TYPE_INT:
                return left >= right.int_value;
            case filter_comp_value_t::TYPE_DOUBLE:
                return left >= right.double_value;
        }
    }
}

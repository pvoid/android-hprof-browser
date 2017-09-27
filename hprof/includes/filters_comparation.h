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

    class filter_compare_field_t : public filter_by_field_t {
    public:
        filter_compare_field_t(const char* field_name, const filter_comp_value_t& value) : filter_by_field_t(field_name) {}
        virtual ~filter_compare_field_t() {}
    protected:
        filter_comp_value_t _value;
    };

    class filter_compare_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_equals_field_t(const char* field_name, const filter_comp_value_t& value) : filter_compare_field_t(field_name, value) {}
        virtual ~filter_compare_equals_field_t() {}

    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
            switch (field.type()) {
                case JVM_TYPE_UNKNOWN:
                    return false;
                case JVM_TYPE_BOOL: {
                    if (_value.type != filter_comp_value_t::TYPE_BOOL) {
                        return false;
                    }
                    return _value.bool_value == static_cast<jvm_bool_t>(field);
                }
                case JVM_TYPE_BYTE:
                    return _value == static_cast<int64_t>(static_cast<jvm_byte_t>(field));
                case JVM_TYPE_SHORT:
                    return _value == static_cast<int64_t>(static_cast<jvm_short_t>(field));
                case JVM_TYPE_CHAR:
                    return _value == static_cast<int64_t>(static_cast<jvm_char_t>(field));
                case JVM_TYPE_INT:
                    return _value == static_cast<int64_t>(static_cast<jvm_int_t>(field));
                case JVM_TYPE_LONG:
                    return _value == static_cast<jvm_long_t>(field);
                case JVM_TYPE_FLOAT:
                    return _value == static_cast<jvm_float_t>(field);
                case JVM_TYPE_DOUBLE:
                    return _value == static_cast<jvm_double_t>(field);
                case JVM_TYPE_OBJECT: {
                    if (_value.type != filter_comp_value_t::TYPE_TEXT) {
                        return false;
                    }

                    id_t id = static_cast<id_t>(field);
                    if (id == 0) {
                        return false;
                    }

                    object_info_ptr_t value = helper.get_object_by_id(id);
                    if (!helper.types_helper().is_string(*value)) {
                        return false;
                    }

                    std::string text;
                    auto value_type = helper.types_helper().get_string_value(*static_cast<instance_info_t *>(value.get()), text);

                    switch (value_type) {
                        case types_helper_t::Null:
                            return _value.text_value == nullptr;
                        case types_helper_t::Value:
                            return text == _value.text_value;
                        case types_helper_t::NotString:
                        case types_helper_t::Invalid:
                        default:
                            return false;
                    }
                }
            }
        }
    };

    class filter_compare_not_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_not_equals_field_t(const char* field_name, const filter_comp_value_t& value) : filter_compare_field_t(field_name, value) {}
        virtual ~filter_compare_not_equals_field_t() {}

    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
            switch (field.type()) {
                case JVM_TYPE_UNKNOWN:
                    return false;
                case JVM_TYPE_BOOL: {
                    if (_value.type != filter_comp_value_t::TYPE_BOOL) {
                        return false;
                    }
                    return _value.bool_value != static_cast<jvm_bool_t>(field);
                }
                case JVM_TYPE_BYTE:
                    return _value != static_cast<int64_t>(static_cast<jvm_byte_t>(field));
                case JVM_TYPE_SHORT:
                    return _value != static_cast<int64_t>(static_cast<jvm_short_t>(field));
                case JVM_TYPE_CHAR:
                    return _value != static_cast<int64_t>(static_cast<jvm_char_t>(field));
                case JVM_TYPE_INT:
                    return _value != static_cast<int64_t>(static_cast<jvm_int_t>(field));
                case JVM_TYPE_LONG:
                    return _value != static_cast<jvm_long_t>(field);
                case JVM_TYPE_FLOAT:
                    return _value != static_cast<jvm_float_t>(field);
                case JVM_TYPE_DOUBLE:
                    return _value != static_cast<jvm_double_t>(field);
                case JVM_TYPE_OBJECT: {
                    if (_value.type != filter_comp_value_t::TYPE_TEXT) {
                        return false;
                    }

                    id_t id = static_cast<id_t>(field);
                    if (id == 0) {
                        return false;
                    }

                    object_info_ptr_t value = helper.get_object_by_id(id);
                    if (!helper.types_helper().is_string(*value)) {
                        return false;
                    }

                    std::string text;
                    auto value_type = helper.types_helper().get_string_value(*static_cast<instance_info_t *>(value.get()), text);

                    switch (value_type) {
                        case types_helper_t::Null:
                            return _value.text_value != nullptr;
                        case types_helper_t::Value:
                            return text != _value.text_value;
                        case types_helper_t::NotString:
                        case types_helper_t::Invalid:
                        default:
                            return false;
                    }
                }
            }
        }
    };

    class filter_compare_less_field_t : public filter_compare_field_t {
    public:
        filter_compare_less_field_t(const char* field_name, const filter_comp_value_t& value) : filter_compare_field_t(field_name, value) {}
        virtual ~filter_compare_less_field_t() {}

    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
            switch (field.type()) {
                case JVM_TYPE_UNKNOWN:
                case JVM_TYPE_BOOL:
                case JVM_TYPE_OBJECT:
                    return false;
                case JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) < _value;
                case JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) < _value;
                case JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) < _value;
                case JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) < _value;
                case JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) < _value;
                case JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) < _value;
                case JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) < _value;
            }
        }
    };

    class filter_compare_less_or_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_less_or_equals_field_t(const char* field_name, const filter_comp_value_t& value) : filter_compare_field_t(field_name, value) {}
        virtual ~filter_compare_less_or_equals_field_t() {}

    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
            switch (field.type()) {
                case JVM_TYPE_UNKNOWN:
                case JVM_TYPE_BOOL:
                case JVM_TYPE_OBJECT:
                    return false;
                case JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) <= _value;
                case JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) <= _value;
                case JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) <= _value;
                case JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) <= _value;
                case JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) <= _value;
                case JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) <= _value;
                case JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) <= _value;
            }
        }
    };

    class filter_compare_greater_field_t : public filter_compare_field_t {
    public:
        filter_compare_greater_field_t(const char* field_name, const filter_comp_value_t& value) : filter_compare_field_t(field_name, value) {}
        virtual ~filter_compare_greater_field_t() {}

    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
            switch (field.type()) {
                case JVM_TYPE_UNKNOWN:
                case JVM_TYPE_BOOL:
                case JVM_TYPE_OBJECT:
                    return false;
                case JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) > _value;
                case JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) > _value;
                case JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) > _value;
                case JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) > _value;
                case JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) > _value;
                case JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) > _value;
                case JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) > _value;
            }
        }
    };

    class filter_compare_greater_or_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_greater_or_equals_field_t(const char* field_name, const filter_comp_value_t& value) : filter_compare_field_t(field_name, value) {}
        virtual ~filter_compare_greater_or_equals_field_t() {}

    protected:
        virtual bool match(const instance_info_t::field_t& field, const filter_helper_t& helper) const {
            switch (field.type()) {
                case JVM_TYPE_UNKNOWN:
                case JVM_TYPE_BOOL:
                case JVM_TYPE_OBJECT:
                    return false;
                case JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) >= _value;
                case JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) >= _value;
                case JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) >= _value;
                case JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) >= _value;
                case JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) >= _value;
                case JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) >= _value;
                case JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) >= _value;
            }
        }
    };
}

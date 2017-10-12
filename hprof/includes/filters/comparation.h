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

#include "filters/base.h"
#include "filters/filter_comp_value.h"

#include "types/string_instance.h"

namespace hprof {

    class filter_compare_field_t : public filter_by_field_t {
    public:
        filter_compare_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_by_field_t(fetcher), _value(value) {}
        virtual ~filter_compare_field_t() {}
    protected:
        filter_comp_value_t _value;
    };

    class filter_compare_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_equals_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_compare_field_t(fetcher, value) {}
        virtual ~filter_compare_equals_field_t() {}

    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
            switch (field.type()) {
                case jvm_type_t::JVM_TYPE_UNKNOWN:
                    return false;
                case jvm_type_t::JVM_TYPE_BOOL: {
                    if (_value.type != filter_comp_value_t::TYPE_BOOL) {
                        return false;
                    }
                    return _value.bool_value == static_cast<jvm_bool_t>(field);
                }
                case jvm_type_t::JVM_TYPE_BYTE:
                    return _value == static_cast<int64_t>(static_cast<jvm_byte_t>(field));
                case jvm_type_t::JVM_TYPE_SHORT:
                    return _value == static_cast<int64_t>(static_cast<jvm_short_t>(field));
                case jvm_type_t::JVM_TYPE_CHAR:
                    return _value == static_cast<int64_t>(static_cast<jvm_char_t>(field));
                case jvm_type_t::JVM_TYPE_INT:
                    return _value == static_cast<int64_t>(static_cast<jvm_int_t>(field));
                case jvm_type_t::JVM_TYPE_LONG:
                    return _value == static_cast<jvm_long_t>(field);
                case jvm_type_t::JVM_TYPE_FLOAT:
                    return _value == static_cast<jvm_float_t>(field);
                case jvm_type_t::JVM_TYPE_DOUBLE:
                    return _value == static_cast<jvm_double_t>(field);
                case jvm_type_t::JVM_TYPE_OBJECT: {
                    if (_value.type != filter_comp_value_t::TYPE_TEXT) {
                        return false;
                    }

                    jvm_id_t id = static_cast<jvm_id_t>(field);
                    if (id == 0) {
                        return false;
                    }

                    object_info_ptr_t value = objects.find_object(id);
                    if (value == nullptr) {
                        return _value.text_value == nullptr;
                    }

                    if (value->type() != object_info_t::TYPE_STRING) {
                        return false;
                    }

                    string_info_t* str = dynamic_cast<string_info_t*>(value.get());

                    return str->value() == _value.text_value;
                }
            }
        }
    };

    class filter_compare_not_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_not_equals_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_compare_field_t(fetcher, value) {}
        virtual ~filter_compare_not_equals_field_t() {}

    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
            switch (field.type()) {
                case jvm_type_t::JVM_TYPE_UNKNOWN:
                    return false;
                case jvm_type_t::JVM_TYPE_BOOL: {
                    if (_value.type != filter_comp_value_t::TYPE_BOOL) {
                        return false;
                    }
                    return _value.bool_value != static_cast<jvm_bool_t>(field);
                }
                case jvm_type_t::JVM_TYPE_BYTE:
                    return _value != static_cast<int64_t>(static_cast<jvm_byte_t>(field));
                case jvm_type_t::JVM_TYPE_SHORT:
                    return _value != static_cast<int64_t>(static_cast<jvm_short_t>(field));
                case jvm_type_t::JVM_TYPE_CHAR:
                    return _value != static_cast<int64_t>(static_cast<jvm_char_t>(field));
                case jvm_type_t::JVM_TYPE_INT:
                    return _value != static_cast<int64_t>(static_cast<jvm_int_t>(field));
                case jvm_type_t::JVM_TYPE_LONG:
                    return _value != static_cast<jvm_long_t>(field);
                case jvm_type_t::JVM_TYPE_FLOAT:
                    return _value != static_cast<jvm_float_t>(field);
                case jvm_type_t::JVM_TYPE_DOUBLE:
                    return _value != static_cast<jvm_double_t>(field);
                case jvm_type_t::JVM_TYPE_OBJECT: {
                    if (_value.type != filter_comp_value_t::TYPE_TEXT) {
                        return false;
                    }

                    jvm_id_t id = static_cast<jvm_id_t>(field);
                    if (id == 0) {
                        return false;
                    }

                    object_info_ptr_t value = objects.find_object(id);
                    if (value == nullptr) {
                        return _value.text_value != nullptr;
                    }

                    if (value->type() != object_info_t::TYPE_STRING) {
                        return false;
                    }

                    string_info_t* str = dynamic_cast<string_info_t*>(value.get());

                    return str->value() != _value.text_value;
                }
            }
        }
    };

    class filter_compare_less_field_t : public filter_compare_field_t {
    public:
        filter_compare_less_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_compare_field_t(fetcher, value) {}
        virtual ~filter_compare_less_field_t() {}

    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
            switch (field.type()) {
                case jvm_type_t::JVM_TYPE_UNKNOWN:
                case jvm_type_t::JVM_TYPE_BOOL:
                case jvm_type_t::JVM_TYPE_OBJECT:
                    return false;
                case jvm_type_t::JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) < _value;
                case jvm_type_t::JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) < _value;
                case jvm_type_t::JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) < _value;
                case jvm_type_t::JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) < _value;
                case jvm_type_t::JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) < _value;
                case jvm_type_t::JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) < _value;
                case jvm_type_t::JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) < _value;
            }
        }
    };

    class filter_compare_less_or_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_less_or_equals_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_compare_field_t(fetcher, value) {}
        virtual ~filter_compare_less_or_equals_field_t() {}

    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
            switch (field.type()) {
                case jvm_type_t::JVM_TYPE_UNKNOWN:
                case jvm_type_t::JVM_TYPE_BOOL:
                case jvm_type_t::JVM_TYPE_OBJECT:
                    return false;
                case jvm_type_t::JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) <= _value;
                case jvm_type_t::JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) <= _value;
                case jvm_type_t::JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) <= _value;
                case jvm_type_t::JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) <= _value;
                case jvm_type_t::JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) <= _value;
                case jvm_type_t::JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) <= _value;
                case jvm_type_t::JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) <= _value;
            }
        }
    };

    class filter_compare_greater_field_t : public filter_compare_field_t {
    public:
        filter_compare_greater_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_compare_field_t(fetcher, value) {}
        virtual ~filter_compare_greater_field_t() {}

    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
            switch (field.type()) {
                case jvm_type_t::JVM_TYPE_UNKNOWN:
                case jvm_type_t::JVM_TYPE_BOOL:
                case jvm_type_t::JVM_TYPE_OBJECT:
                    return false;
                case jvm_type_t::JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) > _value;
                case jvm_type_t::JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) > _value;
                case jvm_type_t::JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) > _value;
                case jvm_type_t::JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) > _value;
                case jvm_type_t::JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) > _value;
                case jvm_type_t::JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) > _value;
                case jvm_type_t::JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) > _value;
            }
        }
    };

    class filter_compare_greater_or_equals_field_t : public filter_compare_field_t {
    public:
        filter_compare_greater_or_equals_field_t(field_fetcher_t *fetcher, const filter_comp_value_t& value) : filter_compare_field_t(fetcher, value) {}
        virtual ~filter_compare_greater_or_equals_field_t() {}

    protected:
        virtual bool match(const field_value_t& field, const objects_index_t& objects) const {
            switch (field.type()) {
                case jvm_type_t::JVM_TYPE_UNKNOWN:
                case jvm_type_t::JVM_TYPE_BOOL:
                case jvm_type_t::JVM_TYPE_OBJECT:
                    return false;
                case jvm_type_t::JVM_TYPE_BYTE:
                    return static_cast<int64_t>(static_cast<jvm_byte_t>(field)) >= _value;
                case jvm_type_t::JVM_TYPE_SHORT:
                    return static_cast<int64_t>(static_cast<jvm_short_t>(field)) >= _value;
                case jvm_type_t::JVM_TYPE_CHAR:
                    return static_cast<int64_t>(static_cast<jvm_char_t>(field)) >= _value;
                case jvm_type_t::JVM_TYPE_INT:
                    return static_cast<int64_t>(static_cast<jvm_int_t>(field)) >= _value;
                case jvm_type_t::JVM_TYPE_LONG:
                    return static_cast<jvm_long_t>(field) >= _value;
                case jvm_type_t::JVM_TYPE_FLOAT:
                    return static_cast<jvm_float_t>(field) >= _value;
                case jvm_type_t::JVM_TYPE_DOUBLE:
                    return static_cast<jvm_double_t>(field) >= _value;
            }
        }
    };
}

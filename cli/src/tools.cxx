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
#include "tools.h"
#include "hprof_types_helper.h"

#include <iostream>
#include <type_traits>

using namespace hprof;

void print_object(const object_info_ptr_t& item, const dump_data_t& hprof, int level, int max_level);

inline void print_type(jvm_type_t type) {
    switch(type) {
        case JVM_TYPE_UNKNOWN:
            std::cout << "unknwon";
            break;
        case JVM_TYPE_BOOL:
            std::cout << "boolean";
            break;
        case JVM_TYPE_BYTE:
            std::cout << "byte";
            break;
        case JVM_TYPE_CHAR:
            std::cout << "char";
            break;
        case JVM_TYPE_SHORT:
            std::cout << "short";
            break;
        case JVM_TYPE_INT:
            std::cout << "int";
            break;
        case JVM_TYPE_LONG:
            std::cout << "long";
            break;
        case JVM_TYPE_FLOAT:
            std::cout << "float";
            break;
        case JVM_TYPE_DOUBLE:
            std::cout << "double";
            break;
        case JVM_TYPE_OBJECT:
            std::cout << "object";
            break;
    }
}

template <typename T>
void print_field_value(T field, const dump_data_t& hprof, int level, int max_level) {
    switch (field.type()) {
        case JVM_TYPE_UNKNOWN:
            std::cout << "unknwon";
            break;
        case JVM_TYPE_BOOL:
            std::cout << "boolean = " << (static_cast<jvm_bool_t>(field) ? "true" : "false");
            break;
        case JVM_TYPE_BYTE:
            std::cout << "byte = " << (int) static_cast<jvm_byte_t>(field);
            break;
        case JVM_TYPE_CHAR:
            std::cout << "char = " << static_cast<jvm_char_t>(field);
            break;
        case JVM_TYPE_SHORT:
            std::cout << "short = " << static_cast<jvm_short_t>(field);
            break;
        case JVM_TYPE_INT:
            std::cout << "int = " << static_cast<jvm_int_t>(field);
            break;
        case JVM_TYPE_LONG:
            std::cout << "long = " << static_cast<jvm_long_t>(field);
            break;
        case JVM_TYPE_FLOAT:
            std::cout << "float = " << static_cast<jvm_float_t>(field);
            break;
        case JVM_TYPE_DOUBLE:
            std::cout << "double = " << static_cast<jvm_double_t>(field);
            break;
        case JVM_TYPE_OBJECT: {
            hprof::id_t id = static_cast<hprof::id_t>(field);
            std::cout << "object = [0x" << std::hex << id << std::dec << "] ";

            if (id == 0) {
                std::cout << "null";
            } else {
                object_info_ptr_t obj = hprof.get_object_by_id(id);
                if (obj == nullptr) {
                    std::cout << "<not found>";
                } else {
                    print_object(obj, hprof, level + 1, max_level);
                }
            }

            break;
        }
    }
}

void print_static_fields(const class_info_t* const cls, const dump_data_t& hprof, int level, int max_level) {
    std::string field_ident;
    field_ident.assign((level + 1) * 4, ' ');

    for (int index = 0, count = cls->static_fields_count(); index < count; ++index) {
        auto& field = cls->static_field(index);
        std::cout << field_ident << "static " << hprof.get_string(field.name_id()) << " : ";
        print_field_value(field, hprof, level, max_level);
        std::cout << std::endl;
    }
}

void print_instance(const instance_info_t* item, const dump_data_t& hprof, int level, int max_level) {
    if (item == nullptr) {
        std::cout << "NULL";
    }

    std::cout << hprof.get_string(item->get_class()->name_id());
    hprof::id_t super_id = item->get_class()->super_id();
    if (super_id != 0) {
        auto super_class = hprof.get_class_by_id(super_id);
        std::cout << " : " << hprof.get_string(super_class->name_id());
    }

    if (level < max_level) {
        std::cout << " {" << std::endl;

        // item->dump();

        std::string field_ident;
        field_ident.assign((level + 1) * 4, ' ');

        auto& fields = item->fields();
        for (int index = 0, count = fields.count(); index < count; ++index) {
            auto field = fields[index];
            std::cout << field_ident << hprof.get_string(field.name_id()) << " : ";
            print_field_value(field, hprof, level, max_level);
            std::cout << std::endl;
        }

        print_static_fields(item->get_class().get(), hprof, level, max_level);

        for (int index = 0; index < level; ++index) {
                std::cout  << "    ";
        }
        std::cout  << "} ";
    }
}

void print_primitive_array(const primitive_array_info_t* const array, const dump_data_t& hprof, int level) {
    if (array == nullptr) {
        std::cout <<  "null";
        return;
    }

    print_type(array->value_type());

    std::cout << "(" << array->length() << ") = [";
    for (int index = 0, length = array->length(); index < length; ++index) {
        if (index) {
            std::cout << ", ";
        }

        switch (array->value_type()) {
            case JVM_TYPE_BOOL:
                std::cout << (array->item<jvm_bool_t>(index) ? "true" : "false");
                break;
            case JVM_TYPE_CHAR:
                std::wcout << "'"<< (wchar_t) array->item<jvm_char_t>(index) << "'";
                break;
            case JVM_TYPE_FLOAT:
                std::cout << array->item<jvm_float_t>(index);
                break;
            case JVM_TYPE_DOUBLE:
                std::cout << array->item<jvm_double_t>(index);
                break;
            case JVM_TYPE_BYTE:
                std::cout << (int) array->item<jvm_byte_t>(index);
                break;
            case JVM_TYPE_SHORT:
                std::cout << array->item<jvm_short_t>(index);
                break;
            case JVM_TYPE_INT:
                std::cout << array->item<jvm_int_t>(index);
                break;
            case JVM_TYPE_LONG:
                std::cout << array->item<jvm_long_t>(index);
                break;
            default:
                break;
        }
    }
    std::cout << "]";
}

void print_object_array(const object_array_info_t* const array, const dump_data_t& hprof, int level, int max_level) {
    if (array == nullptr) {
        std::cout <<  "null";
        return;
    }

    std::cout << "object (" << array->length() << ") = [";
    for (int index = 0, length = array->length(); index < length; ++index) {
        if (index) {
            std::cout << ", ";
        }

        auto object = hprof.get_object_by_id(array->item(index));
        if (object == nullptr) {
            std::cout << "null";
        }
        print_object(object, hprof, level + 1, max_level);
    }
    std::cout << "]";
}

void print_class(const class_info_t* const cls, const dump_data_t& hprof, int level, int max_level) {
    std::cout << "class "<< hprof.get_string(cls->name_id());
    hprof::id_t super_id = cls->super_id();
    if (super_id != 0) {
        auto super_class = hprof.get_class_by_id(super_id);
        std::cout << " : " << hprof.get_string(super_class->name_id());
    }

    if (level < max_level) {
        std::cout << " {" << std::endl;

        std::string field_ident;
        field_ident.assign((level + 1) * 4, ' ');

        for (int index = 0, count = cls->fields_count(); index < count; ++index) {
            auto field = cls->field(index);
            std::cout << field_ident << hprof.get_string(field.name_id()) << " : ";
            print_type(field.type());
            std::cout << std::endl;
        }

        print_static_fields(cls, hprof, level, max_level);

        for (int index = 0; index < level; ++index) {
                std::cout  << "    ";
        }
        std::cout  << "} ";
    }
}

void print_object(const object_info_ptr_t& item, const dump_data_t& hprof, int max_level) {
    print_object(item, hprof, 0, max_level);
    std::cout << std::endl;
}

void print_object(const object_info_ptr_t& item, const dump_data_t& hprof, int level, int max_level) {
    if (item == nullptr) {
        std::cout << "null";
        return;
    }

    switch (item->type()) {
        case object_info_t::TYPE_INSTANCE: {
            const instance_info_t* instance = dynamic_cast<const instance_info_t* const>(item.get());
            if (hprof.types_helper().is_string(*instance)) {
                std::string result;
                auto value_type = hprof.types_helper().get_string_value(*instance, result);

                switch (value_type) {
                    case types_helper_t::Null:
                        std::cout << "null";
                        break;
                    case types_helper_t::Value:
                        std::cout << "\"" << result << "\"";
                        break;
                    case types_helper_t::NotString:
                    case types_helper_t::Invalid:
                    default:
                        std::cout << "Invalid string";
                        break;
                }
                break;
            }
            print_instance(instance, hprof, level, max_level);
            break;
        }
        case object_info_t::TYPE_CLASS:
            print_class(dynamic_cast<const class_info_t* const>(item.get()), hprof, level, max_level);
            break;
        case object_info_t::TYPE_OBJECTS_ARRAY:
            print_object_array(dynamic_cast<const object_array_info_t* const>(item.get()), hprof, level, max_level);
            break;
        case object_info_t::TYPE_PRIMITIVES_ARRAY:
            print_primitive_array(dynamic_cast<const primitive_array_info_t* const>(item.get()), hprof, level);
            break;
    }
}

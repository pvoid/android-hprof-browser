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
#include "types.h"
#include "tools.h"

#include <iostream>
#include <type_traits>
#include <cassert>

using namespace hprof;

void print_object(const object_info_ptr_t& item, const objects_index_t& objects, int level, int max_level);

inline void print_type(jvm_type_t type) {
    switch(type) {
        case jvm_type_t::JVM_TYPE_UNKNOWN:
            std::cout << "unknwon";
            break;
        case jvm_type_t::JVM_TYPE_BOOL:
            std::cout << "boolean";
            break;
        case jvm_type_t::JVM_TYPE_BYTE:
            std::cout << "byte";
            break;
        case jvm_type_t::JVM_TYPE_CHAR:
            std::cout << "char";
            break;
        case jvm_type_t::JVM_TYPE_SHORT:
            std::cout << "short";
            break;
        case jvm_type_t::JVM_TYPE_INT:
            std::cout << "int";
            break;
        case jvm_type_t::JVM_TYPE_LONG:
            std::cout << "long";
            break;
        case jvm_type_t::JVM_TYPE_FLOAT:
            std::cout << "float";
            break;
        case jvm_type_t::JVM_TYPE_DOUBLE:
            std::cout << "double";
            break;
        case jvm_type_t::JVM_TYPE_OBJECT:
            std::cout << "object";
            break;
    }
}

void print_field_value(const field_value_t& field, const objects_index_t& objects, int level, int max_level) {
    print_type(field.type());

    std::cout << " = ";

    switch (field.type()) {
        case jvm_type_t::JVM_TYPE_UNKNOWN:
            break;
        case jvm_type_t::JVM_TYPE_BOOL:
            std::cout << (static_cast<jvm_bool_t>(field) ? "true" : "false");
            break;
        case jvm_type_t::JVM_TYPE_BYTE:
            std::cout << (int) static_cast<jvm_byte_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_CHAR:
            std::cout << static_cast<jvm_char_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_SHORT:
            std::cout << static_cast<jvm_short_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_INT:
            std::cout << static_cast<jvm_int_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_LONG:
            std::cout << static_cast<jvm_long_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_FLOAT:
            std::cout << static_cast<jvm_float_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_DOUBLE:
            std::cout << static_cast<jvm_double_t>(field);
            break;
        case jvm_type_t::JVM_TYPE_OBJECT: {
            jvm_id_t id = static_cast<jvm_id_t>(field);
            std::cout << "[0x" << std::hex << id << std::dec << "] ";

            if (id == 0) {
                std::cout << "null";
            } else {
                object_info_ptr_t obj = objects.find_object(id);
                if (obj == nullptr) {
                    std::cout << "<not found>";
                } else {
                    print_object(obj, objects, level + 1, max_level);
                }
            }

            break;
        }
    }
}

void print_instance(const instance_info_t* item, const objects_index_t& objects, int level, int max_level) {
    assert(item != nullptr);

    if (item->get_class() != nullptr) {
        std::cout << item->get_class()->name();
    }
    ;
    if (item->get_class()->super() != nullptr) {
        std::cout << " : " << item->get_class()->super()->name();
    }

    if (level < max_level) {
        std::cout << " {" << std::endl;

        std::string field_ident;
        field_ident.assign((level + 1) * 4, ' ');

        for (auto& field : item->fields()) {
            std::cout << field_ident << field.name() << " : ";
            print_field_value(field, objects, level, max_level);
            std::cout << std::endl;
        }

        const class_info_t *cls = item->get_class();
        while (cls != nullptr) { 
            for (auto& field : cls->static_fields()) {
                std::cout << field_ident << "static " << field.name() << " : ";
                print_field_value(field, objects, level, max_level);
                std::cout << std::endl;
            }

            cls = cls->super();
        }

        for (int index = 0; index < level; ++index) {
                std::cout  << "    ";
        }
        std::cout  << "} ";
    }
}

void print_primitive_array(const primitives_array_info_t* const array, const objects_index_t& objects, int level) {
    assert(array != nullptr);
    print_type(array->item_type());

    std::cout << "(" << array->length() << ") = [";
    bool first = true;

    for (auto& item : *array) {
        if (!first) std::cout << ", ";
        else first = false;


        switch (array->item_type()) {
            case jvm_type_t::JVM_TYPE_BOOL:
                std::cout << (static_cast<jvm_bool_t>(item) ? "true" : "false");
                break;
            case jvm_type_t::JVM_TYPE_CHAR:
                std::wcout << "'"<< (wchar_t) static_cast<jvm_char_t>(item) << "'";
                break;
            case jvm_type_t::JVM_TYPE_FLOAT:
                std::cout << static_cast<jvm_float_t>(item);
                break;
            case jvm_type_t::JVM_TYPE_DOUBLE:
                std::cout << static_cast<jvm_double_t>(item);
                break;
            case jvm_type_t::JVM_TYPE_BYTE:
                std::cout << (int) static_cast<jvm_byte_t>(item);
                break;
            case jvm_type_t::JVM_TYPE_SHORT:
                std::cout << static_cast<jvm_short_t>(item);
                break;
            case jvm_type_t::JVM_TYPE_INT:
                std::cout << static_cast<jvm_int_t>(item);
                break;
            case jvm_type_t::JVM_TYPE_LONG:
                std::cout << static_cast<jvm_long_t>(item);
                break;
            default:
                break;
        }
    }
    std::cout << "]";
}

void print_object_array(const objects_array_info_t* array, const objects_index_t& objects, int level, int max_level) {
    assert(array != nullptr);
    std::cout << "object (" << array->length() << ") = [";
    bool first = true;
    for (auto& id : *array) {
        if (!first) std::cout << ", ";
        else first = false;

        auto object = objects.find_object(id);
        if (object == nullptr) {
            std::cout << "null";
        }
        print_object(object, objects, level + 1, max_level);
    }
    std::cout << "]";
}

void print_class(const class_info_t* cls, const objects_index_t& objects, int level, int max_level) {
    std::cout << "class "<< cls->name();
    const class_info_t* super = cls->super();
    if (super != nullptr) {
        std::cout << " : " << super->name();
    }

    if (level < max_level) {
        std::cout << " {" << std::endl;

        std::string field_ident;
        field_ident.assign((level + 1) * 4, ' ');

        for (auto& field : cls->fields()) {
            std::cout << field_ident << field.name() << " : ";
            print_type(field.type());
            std::cout << std::endl;
        }

        for (auto& field : cls->static_fields()) {
            std::cout << field_ident << "static " << field.name() << " : ";
            print_field_value(field, objects, level, max_level);
            std::cout << std::endl;
        }

        for (int index = 0; index < level; ++index) {
                std::cout  << "    ";
        }
        std::cout  << "} ";
    }
}

void print_object(const hprof::object_info_ptr_t& item, const hprof::objects_index_t& objects, int max_level) {
    print_object(item, objects, 0, max_level);
    std::cout << std::endl;
}

void print_object(const object_info_ptr_t& item, const objects_index_t& objects, int level, int max_level) {
    if (item == nullptr) {
        std::cout << "null";
        return;
    }

    switch (item->type()) {
        case object_info_t::TYPE_STRING: {
            std::cout << "\"" << dynamic_cast<string_info_t*>(item.get())->value() << "\"";
            break;
        }
        case object_info_t::TYPE_INSTANCE: {
            print_instance(dynamic_cast<const instance_info_t*>(item.get()), objects, level, max_level);
            break;
        }
        case object_info_t::TYPE_CLASS:
            print_class(dynamic_cast<const class_info_t* const>(item.get()), objects, level, max_level);
            break;
        case object_info_t::TYPE_OBJECTS_ARRAY:
            print_object_array(dynamic_cast<const objects_array_info_t*>(item.get()), objects, level, max_level);
            break;
        case object_info_t::TYPE_PRIMITIVES_ARRAY:
            assert(item.get() != nullptr);
            assert(dynamic_cast<const primitives_array_info_t*>(item.get()) != nullptr);
            print_primitive_array(dynamic_cast<const primitives_array_info_t*>(item.get()), objects, level);
            break;
    }
}

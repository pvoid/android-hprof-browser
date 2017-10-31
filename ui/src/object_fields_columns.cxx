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
#include "object_fields_columns.h"

#include <codecvt>
#include <locale>
#include <sstream>
#include <iomanip>

using namespace hprof;

std::wstring_convert<std::codecvt_utf8_utf16<char16_t, 0x10ffff, std::little_endian>, char16_t> text_converter;

inline Glib::ustring get_type_name(jvm_type_t type) {
    switch (type) {
        case jvm_type_t::JVM_TYPE_OBJECT:
            return "object";
        case jvm_type_t::JVM_TYPE_BOOL:
            return "bool";
        case jvm_type_t::JVM_TYPE_CHAR:
            return "char";
        case jvm_type_t::JVM_TYPE_FLOAT:
            return "float";
        case jvm_type_t::JVM_TYPE_DOUBLE:
            return "double";
        case jvm_type_t::JVM_TYPE_BYTE:
            return "byte";
        case jvm_type_t::JVM_TYPE_SHORT:
            return "short";
        case jvm_type_t::JVM_TYPE_INT:
            return "int";
        case jvm_type_t::JVM_TYPE_LONG:
            return "long";
        case jvm_type_t::JVM_TYPE_UNKNOWN:
        default:
            return "unknown";
    }
}

inline Glib::ustring get_id_string(jvm_id_t id) {
    std::stringstream out;
    out << "0x";
    out.fill('0');
    out << std::setw(8) << std::hex << id;
    return out.str();
}

inline Glib::ustring get_field_value(const field_value_t& field) {
    switch (field.type()) {
        case jvm_type_t::JVM_TYPE_OBJECT: {
            jvm_id_t id = static_cast<jvm_id_t>(field);
            if (id == 0) return "null";
            else return get_id_string(id);
        }
        case jvm_type_t::JVM_TYPE_BOOL:
            return static_cast<jvm_bool_t>(field) ? "true" : "false";
        case jvm_type_t::JVM_TYPE_CHAR:
            return text_converter.to_bytes(static_cast<jvm_char_t>(field));
        case jvm_type_t::JVM_TYPE_FLOAT:
            return  std::to_string(static_cast<jvm_float_t>(field));
        case jvm_type_t::JVM_TYPE_DOUBLE:
            return  std::to_string(static_cast<jvm_double_t>(field));
        case jvm_type_t::JVM_TYPE_BYTE:
            return  std::to_string(static_cast<jvm_byte_t>(field));
        case jvm_type_t::JVM_TYPE_SHORT:
            return  std::to_string(static_cast<jvm_short_t>(field));
        case jvm_type_t::JVM_TYPE_INT:
            return  std::to_string(static_cast<jvm_int_t>(field));
        case jvm_type_t::JVM_TYPE_LONG:
            return  std::to_string(static_cast<jvm_long_t>(field));
        case jvm_type_t::JVM_TYPE_UNKNOWN:
        default:
            return "";
    }
}

void ObjectFieldsColumns::assign(Glib::RefPtr<Gtk::TreeStore> model, const heap_item_t& item) const {
    if (!model) {
        return;
    }

    switch (item.type()) {
        case heap_item_t::Object:
            assign(model, *model->append(), static_cast<const instance_info_t*>(item));
            break;
    }
}

void ObjectFieldsColumns::assign(Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, const instance_info_t* instance) const {
    row[_name] = instance->get_class()->name();
    if (instance->get_class()->super_id() != 0) {
        row[_type] = instance->get_class()->super()->name();
    }
    row[_value] = get_id_string(instance->id());

    append_fields(model, row, instance->fields());
}

void ObjectFieldsColumns::append_fields(Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, const fields_values_t& fields) const {
    for (auto& field : fields) {
        assign(*model->append(row.children()), field);
    }
}

void ObjectFieldsColumns::assign(const Gtk::TreeModel::Row& row, const field_value_t& field) const {
    row[_name] = field.name();
    row[_type] = get_type_name(field.type());
    row[_value] = get_field_value(field);
    if (field.type() == jvm_type_t::JVM_TYPE_OBJECT) {
        row[_object_id] = static_cast<jvm_id_t>(field);
        row[_data_fetched] = false;
    } else {
        row[_object_id] = 0;
        row[_data_fetched] = true;
    }
    row[_fetch_request_id] = 0;
}

void ObjectFieldsColumns::assign_value(const Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, u_int64_t request_id, const heap_item_t& item) {
    if (row[_fetch_request_id] != request_id) return;

    switch (item.type()) {
        case heap_item_t::Object: {
            auto instance = static_cast<const instance_info_t *>(item);
            row[_type] = instance->get_class()->name();
            append_fields(model, row, instance->fields());
            break;
        }
        case heap_item_t::String: {
            row[_value] = "\"" + static_cast<const string_info_t *>(item)->value() + "\"";
            row[_type] = "string";
            break;
        }
        case heap_item_t::PrimitivesArray: {
            auto array = static_cast<const primitives_array_info_t *>(item);
            row[_type] = get_type_name(array->item_type()) + "[" + std::to_string(array->length()) + "]" ;
            break;
        }
        case heap_item_t::ObjectsArray: {
            auto array = static_cast<const objects_array_info_t *>(item);
            row[_type] = "object[" + std::to_string(array->length()) + "]";
            break;
        }
    }
}
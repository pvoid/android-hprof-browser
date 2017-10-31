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

#include <gtkmm.h>
#include "hprof.h"

namespace hprof {
    class ObjectFieldsColumns : public Gtk::TreeModelColumnRecord {
    public:
        ObjectFieldsColumns() : _last_request_id(0) {
            add(_name);
            add(_type);
            add(_value);
            add(_object_id);
            add(_data_fetched);
            add(_fetch_request_id);
        }

        void add_columns(Gtk::TreeView& view) {
            view.append_column("Name", _name);
            view.append_column("Value", _value);
            view.append_column("Type", _type);

            view.get_column(0)->set_sort_column(_name);
            view.get_column(2)->set_sort_column(_type);
        }

        void assign(Glib::RefPtr<Gtk::TreeStore> model, const heap_item_t& item) const;

        void assign_value(const Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, u_int64_t request_id, const heap_item_t& item);

        template<typename Callback>
        void fetch_objects(const Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, const Callback& callback) {
            for (auto it = std::begin(row.children()); it != std::end(row.children()); ++it) {
                if ((*it)[_data_fetched] || (*it)[_fetch_request_id] != 0 || (*it)[_object_id] == 0) continue;
                
                auto request_id = ++_last_request_id;
                callback(model->get_path(it), request_id, (*it)[_object_id]);
                (*it)[_fetch_request_id] = request_id;
            }
        }

    private:
        void assign(Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, const instance_info_t* instance) const;
        void assign(const Gtk::TreeModel::Row& row, const field_value_t& field) const;
        void append_fields(Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, const fields_values_t& fields) const;
    private:
        u_int64_t _last_request_id;
        Gtk::TreeModelColumn<Glib::ustring> _name;
        Gtk::TreeModelColumn<Glib::ustring> _type;
        Gtk::TreeModelColumn<Glib::ustring> _value;
        Gtk::TreeModelColumn<jvm_id_t> _object_id;
        Gtk::TreeModelColumn<bool> _data_fetched;
        Gtk::TreeModelColumn<u_int64_t> _fetch_request_id;
    };
}

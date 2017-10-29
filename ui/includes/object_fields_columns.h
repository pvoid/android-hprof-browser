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
        ObjectFieldsColumns() {
            add(name);
            add(type);
            add(value);
        }

        void add_columns(Gtk::TreeView& view) {
            view.append_column("Name", name);
            view.append_column("Value", value);
            view.append_column("Type", type);
        }

        void assign(Glib::RefPtr<Gtk::TreeStore> model, const heap_item_t& item) const;
    private:
        void assign(Glib::RefPtr<Gtk::TreeStore> model, const Gtk::TreeModel::Row& row, const instance_info_t* instance) const;
        void assign(const Gtk::TreeModel::Row& row, const field_value_t& field) const;
    private:
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<Glib::ustring> type;
        Gtk::TreeModelColumn<Glib::ustring> value;
    };
}

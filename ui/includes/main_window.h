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

#include "dispatcher.h"
#include "hprof_storage.h"
#include "treeview_storage.h"
#include "object_fields_columns.h"

namespace hprof {
    class MainWindow : public Gtk::ApplicationWindow {
    public:
        MainWindow(EventsDisparcher& dispatcher, HprofStorage& hprof_storage, TreeViewStorage& treeview_storage);

        void on_open_hprof_file();
    private:
        void configure_header();
        void configure_query_screen(int32_t window_width, int32_t window_height);
        void configure_statusbar();

        void on_execute_query();
        void on_hprof_start_load(const std::string& file_name);
        void on_hprof_loading_progress(const std::string& action, double fraction);
        void on_hprof_stop_load();
        void on_query_result(const std::vector<heap_item_ptr_t>& result, u_int64_t seq_number);
        void on_query_failed(const std::vector<parse_error>& errors, u_int64_t seq_number);
        void on_object_fetch_result(u_int64_t request_id, const Gtk::TreeModel::Path& path, const heap_item_ptr_t& item);
        void on_treeview_fill_progress(double fraction);
        void on_treeview_filled();

        bool on_test_expand_row(const Gtk::TreeModel::iterator& row, const Gtk::TreeModel::Path& path);
        bool on_progress_bar_timeout(int timer_number);

        void set_status(const std::string& text);
        void clear_status();

        void show_pulse_progress();
        void set_progress_fraction(double fraction);
        void hide_pulse_progress();

        void clear_query_vew_state();
    private:
        EventsDisparcher& _dispatcher;
        HprofStorage& _hprof_storage;
        TreeViewStorage& _treeview_storage;
        
        // Header, Toolbar
        Gtk::HeaderBar _header_bar;
        Gtk::ToolButton _execute_query_button;

        // Status bar
        Gtk::Statusbar _statusbar;
        Gtk::ProgressBar _progress_bar;
        bool _progress_bar_in_pulse_mode;
        sigc::connection _timer_connection;

        // Query editor
        Gtk::Paned _query_box;
        Glib::RefPtr<Gtk::TextBuffer> _query_text_buffer;
        u_int64_t _query_seq_number;

        // Result view
        ObjectFieldsColumns _result_columns;
        Glib::RefPtr<Gtk::TreeStore> _result_model_store;
        Gtk::TreeView _results_view;
    };
}

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
#include "main_window.h"

using namespace hprof;

const std::string g_window_name { "Android Heap Profile Browser" };

MainWindow::MainWindow(EventsDisparcher& dispatcher, HprofStorage& hprof_storage) : 
    _dispatcher(dispatcher), _hprof_storage(hprof_storage), _progress_box(Gtk::ORIENTATION_VERTICAL), _query_box(Gtk::ORIENTATION_VERTICAL),
    _query_seq_number(0) {
  
    auto screen = Gdk::Screen::get_default();
    auto width =  static_cast<int32_t>(static_cast<double>(screen->get_width()) * 0.80);
    auto height =  static_cast<int32_t>(static_cast<double>(screen->get_height()) * 0.80);
    set_default_size(width, height);
    set_position(Gtk::WIN_POS_CENTER_ALWAYS);

    configure_header();
    configure_progress_screen();
    configure_query_screen(width, height);

    set_titlebar(_header_bar);

    auto root_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); 
    root_box->pack_start(_progress_box, Gtk::PACK_EXPAND_WIDGET);
    root_box->pack_start(_query_box, Gtk::PACK_EXPAND_WIDGET);
    root_box->show();
    
    add(*root_box);

    _hprof_storage.on_start_loading().connect(sigc::mem_fun1(*this, &MainWindow::on_hprof_start_load));
    _hprof_storage.on_progress_loading().connect(sigc::mem_fun2(*this, &MainWindow::on_hprof_loading_progress));
    _hprof_storage.on_stop_loading().connect(sigc::mem_fun(*this, &MainWindow::on_hprof_stop_load));
    _hprof_storage.on_query_succeed().connect(sigc::mem_fun(*this, &MainWindow::on_query_result));
}

void MainWindow::configure_header() {
    _header_bar.set_title(g_window_name);
    _header_bar.set_show_close_button(true);

    auto toolbar = Gtk::manage(new Gtk::Toolbar());
    toolbar->show();

    auto open_button = Gtk::manage(new Gtk::MenuToolButton());
    open_button->set_tooltip_text("Open heap profile");
    open_button->set_icon_name("document-open");
    open_button->set_visible(true);
    toolbar->append(*open_button, sigc::mem_fun(*this, &MainWindow::on_open_hprof_file));

    _execute_query_button.set_tooltip_text("Execute query");
    _execute_query_button.set_icon_name("media-playback-start");
    _execute_query_button.set_visible(true);
    _execute_query_button.set_sensitive(false);
    _execute_query_button.set_margin_start(20);
    toolbar->append(_execute_query_button, sigc::mem_fun(*this, &MainWindow::on_execute_query));

    _header_bar.pack_start(*toolbar);

    _header_bar.show();
}

void MainWindow::configure_progress_screen() {
    _progress_bar.set_text("Loading file...");
    _progress_bar.set_show_text();
    _progress_bar.set_halign(Gtk::ALIGN_CENTER);
    _progress_bar.set_valign(Gtk::ALIGN_CENTER);
    _progress_bar.show();

    _progress_box.pack_start(_progress_bar, Gtk::PACK_SHRINK);
    _progress_box.set_halign(Gtk::ALIGN_CENTER);
    _progress_box.set_valign(Gtk::ALIGN_CENTER);
}

void MainWindow::configure_query_screen(int32_t window_width, int32_t window_height) {
    _query_text_buffer = Gtk::TextBuffer::create();

    auto query_view = Gtk::manage(new Gtk::TextView());
    query_view->set_buffer(_query_text_buffer);
    query_view->set_monospace(true);
    query_view->set_border_width(10);
    query_view->set_wrap_mode(Gtk::WRAP_WORD);
    query_view->show();
    
    auto query_scroll_view = Gtk::manage(new Gtk::ScrolledWindow());
    query_scroll_view->set_size_request(-1, std::min(80, window_height / 4));
    query_scroll_view->add(*query_view);
    query_scroll_view->show();

    _query_box.add1(*query_scroll_view);

    _result_model_store = Gtk::TreeStore::create(_result_columns);

    auto results_view = Gtk::manage(new Gtk::TreeView());
    results_view->set_model(_result_model_store);
    _result_columns.add_columns(*results_view);
    results_view->show();

    auto results_scroll_view = Gtk::manage(new Gtk::ScrolledWindow());
    results_scroll_view->add(*results_view);
    results_scroll_view->show();

    _query_box.add2(*results_scroll_view);

    _query_box.set_border_width(5);
}

void MainWindow::on_hprof_start_load(const std::string& file_name) {
    // Extract file name
    auto name = file_name;
    auto pos = name.find_last_of("/");
    if (pos != std::string::npos) {
        name = name.substr(pos + 1);
    }

    _header_bar.set_title(name);
    _header_bar.set_subtitle(g_window_name);
 
    _query_seq_number = 0;
    _query_text_buffer->set_text("");
    _result_model_store->clear();
    _query_box.hide();

    _execute_query_button.set_sensitive(false);

    _progress_box.show();
}

void MainWindow::on_hprof_stop_load() {
    _progress_box.hide();
    
    _query_box.show();
    _execute_query_button.set_sensitive(true);
}

void MainWindow::on_hprof_loading_progress(const std::string& action, double fraction) {
    _progress_bar.set_text(action);
    _progress_bar.set_fraction(fraction);
}

void MainWindow::on_query_result(const std::vector<heap_item_ptr_t>& result, u_int64_t seq_number) {
    if (_query_seq_number != seq_number) return;

    _result_model_store->clear();
    for (auto& item : result) {
        _result_columns.assign(_result_model_store, *item);
    }
}

void MainWindow::on_open_hprof_file() {
    Gtk::FileChooserDialog dialog {"Please choose heap profile file", Gtk::FILE_CHOOSER_ACTION_OPEN};
    
    dialog.set_transient_for(*this);
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);
    
    auto filter_hprof = Gtk::FileFilter::create();
    filter_hprof->set_name("Heap profiles files");
    filter_hprof->add_pattern("*.hprof");
    dialog.add_filter(filter_hprof);

    auto filter_any = Gtk::FileFilter::create();
    filter_any->set_name("Any files");
    filter_any->add_pattern("*");
    dialog.add_filter(filter_any);

    int result = dialog.run();
    switch (result) {
        case Gtk::RESPONSE_OK: {
            auto action = OpenFileAction::create(dialog.get_filename());
            _dispatcher.emit(std::move(action));
            break;
        }
    }
}

void MainWindow::on_execute_query() {
    auto query_text = _query_text_buffer->get_text();
    if (query_text.empty()) return;

    _dispatcher.emit(ExecuteQueryAction::create(std::string { query_text.c_str() }, ++_query_seq_number));
}

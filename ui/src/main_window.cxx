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

MainWindow::MainWindow(EventsDisparcher& dispatcher, HprofStorage& hprof_storage, TreeViewStorage& treeview_storage) : 
    _dispatcher(dispatcher), _hprof_storage(hprof_storage), _treeview_storage(treeview_storage), _query_box(Gtk::ORIENTATION_VERTICAL),
    _query_seq_number(0), _progress_bar_in_pulse_mode(false) {
  
    auto screen = Gdk::Screen::get_default();
    auto width =  static_cast<int32_t>(static_cast<double>(screen->get_width()) * 0.80);
    auto height =  static_cast<int32_t>(static_cast<double>(screen->get_height()) * 0.80);
    set_default_size(width, height);
    set_position(Gtk::WIN_POS_CENTER_ALWAYS);

    configure_header();
    configure_query_screen(width, height);
    configure_statusbar();

    set_titlebar(_header_bar);

    auto root_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); 
    auto content_frame = Gtk::manage(new Gtk::Frame());
    content_frame->add(_query_box);
    content_frame->show();
    root_box->pack_start(*content_frame, Gtk::PACK_EXPAND_WIDGET);
    root_box->pack_start(_statusbar, Gtk::PACK_SHRINK);
    root_box->show();
    
    add(*root_box);

    _hprof_storage.on_start_loading().connect(sigc::mem_fun1(*this, &MainWindow::on_hprof_start_load));
    _hprof_storage.on_progress_loading().connect(sigc::mem_fun(*this, &MainWindow::on_hprof_loading_progress));
    _hprof_storage.on_stop_loading().connect(sigc::mem_fun(*this, &MainWindow::on_hprof_stop_load));
    _hprof_storage.on_query_succeed().connect(sigc::mem_fun(*this, &MainWindow::on_query_result));
    _hprof_storage.on_query_failed().connect(sigc::mem_fun(*this, &MainWindow::on_query_failed));
    _hprof_storage.on_fetch_object_result().connect(sigc::mem_fun(*this, &MainWindow::on_object_fetch_result));

    _treeview_storage.on_data_filled().connect(sigc::mem_fun(*this, &MainWindow::on_treeview_filled));
    _treeview_storage.on_data_fill_progress().connect(sigc::mem_fun(*this, &MainWindow::on_treeview_fill_progress));
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

    _results_view.signal_test_expand_row().connect(sigc::mem_fun(*this, &MainWindow::on_test_expand_row));
    _result_columns.add_columns(_results_view);
    _results_view.set_grid_lines(Gtk::TREE_VIEW_GRID_LINES_VERTICAL);
    _results_view.set_enable_tree_lines(true);
    _results_view.show();

    auto results_scroll_view = Gtk::manage(new Gtk::ScrolledWindow());
    results_scroll_view->add(_results_view);
    results_scroll_view->show();

    _query_box.add2(*results_scroll_view);

    _query_box.set_border_width(7);
}

void MainWindow::configure_statusbar() {
    _statusbar.pack_start(_progress_bar, Gtk::PACK_SHRINK);
    _statusbar.show();
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
    _query_box.hide();

    _execute_query_button.set_sensitive(false);

    set_status("Loading file: " + name);
    _progress_bar.show();
}

void MainWindow::on_hprof_stop_load() {
    clear_status();
    _progress_bar.hide();
    _query_box.show();
    _execute_query_button.set_sensitive(true);
    _results_view.unset_model();
}

void MainWindow::on_hprof_loading_progress(const std::string& action, double fraction) {
    set_status(action);
    set_progress_fraction(fraction);
}

void MainWindow::on_query_result(const std::vector<heap_item_ptr_t>& result, u_int64_t seq_number) {
    if (_query_seq_number != seq_number) return;

    std::cout << "Query results: " << result.size() << std::endl;

    set_status("Building result list");
    _dispatcher.emit(FillTreeViewAction::create(_result_model_store, &_result_columns, result));
}

void MainWindow::on_query_failed(const std::vector<parse_error>& errors, u_int64_t seq_number) {
    if (_query_seq_number != seq_number) return;

    hide_pulse_progress();
    // Build and show error message
    assert(errors.size() > 0);
    auto& error = errors[0];
    std::stringstream message;
    message << error.location.begin.column << ":" << error.location.begin.line << " " << error.message;
    set_status(message.str());

    // Place cursor on error start
    auto it = _query_text_buffer->begin();
    it.forward_chars(error.location.begin.column - 1);
    _query_text_buffer->place_cursor(it);
}

void MainWindow::on_treeview_fill_progress(double fraction) {
    set_progress_fraction(fraction);
}

void MainWindow::on_treeview_filled() {
    _results_view.set_model(_result_model_store);
    clear_status();
    hide_pulse_progress();
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
            clear_query_vew_state();
            break;
        }
    }
}

void MainWindow::on_execute_query() {
    auto query_text = _query_text_buffer->get_text();
    if (query_text.empty()) return;

    _dispatcher.emit(ExecuteQueryAction::create(std::string { query_text.c_str() }, ++_query_seq_number));
    
    set_status("Running query...");
    show_pulse_progress();
}

bool MainWindow::on_test_expand_row(const Gtk::TreeModel::iterator& row, const Gtk::TreeModel::Path& path) {
    EventsDisparcher* dispatcher = &_dispatcher;
    _result_columns.populate_instance_row(_result_model_store, *row, [dispatcher] (auto path, auto request_id, auto object_id) {
        dispatcher->emit(FetchObjectAction::create(object_id, request_id, path));
    });
    return false;
}

void MainWindow::on_object_fetch_result(u_int64_t request_id, const Gtk::TreeModel::Path& path, const heap_item_ptr_t& item) {
    auto it = _result_model_store->get_iter(path);
    if (!it || item == nullptr) return;
    _result_columns.assign_value(_result_model_store, *it, request_id, item);
}

void MainWindow::set_status(const std::string& text) {
    clear_status();
    _statusbar.push(text);
}

void MainWindow::clear_status() {
    _statusbar.pop();
}

void MainWindow::show_pulse_progress() {
    _progress_bar.show();
    _progress_bar.pulse();
    _progress_bar_in_pulse_mode = true;
    Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(*this, &MainWindow::on_progress_bar_timeout), 0), 40);
}

void MainWindow::hide_pulse_progress() {
    _progress_bar.hide();
}

void MainWindow::set_progress_fraction(double fraction) {
    _progress_bar_in_pulse_mode = false;
    _progress_bar.set_fraction(fraction);
}

bool MainWindow::on_progress_bar_timeout(int timer_number) {
    if (!_progress_bar_in_pulse_mode || !_progress_bar.is_visible()) return false;
    _progress_bar.pulse();
    return true;
}

void MainWindow::clear_query_vew_state() {
    _results_view.unset_model();
    if (_result_model_store->children().size()) {
        _result_model_store->clear();
    }
    _query_text_buffer->erase(_query_text_buffer->begin(), _query_text_buffer->end());
}

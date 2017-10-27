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
    _dispatcher(dispatcher), _hprof_storage(hprof_storage), _progress_box(Gtk::ORIENTATION_VERTICAL) {
    
    auto root_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL)); 
    add(*root_box);
    root_box->show();
    
    _header_bar.set_title(g_window_name);
    _header_bar.set_show_close_button(true);
    set_titlebar(_header_bar);
    _header_bar.show();

    _progress_bar.set_text("Loading file...");
    _progress_bar.set_show_text();
    _progress_bar.set_halign(Gtk::ALIGN_CENTER);
    _progress_bar.set_valign(Gtk::ALIGN_CENTER);
    _progress_bar.show();

    _progress_box.pack_start(_progress_bar, Gtk::PACK_SHRINK);
    _progress_box.set_halign(Gtk::ALIGN_CENTER);
    _progress_box.set_valign(Gtk::ALIGN_CENTER);

    root_box->pack_start(_progress_box, Gtk::PACK_EXPAND_WIDGET);

    _hprof_storage.on_start_loading().connect(sigc::mem_fun1(*this, &MainWindow::on_hprof_start_load));
    _hprof_storage.on_progress_loading().connect(sigc::mem_fun2(*this, &MainWindow::on_hprof_loading_progress));
    _hprof_storage.on_stop_loading().connect(sigc::mem_fun(*this, &MainWindow::on_hprof_stop_load));
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
    _progress_box.show();
}

void MainWindow::on_hprof_stop_load() {
    _progress_box.hide();
}

void MainWindow::on_hprof_loading_progress(const std::string& action, double fraction) {
    _progress_bar.set_text(action);
    _progress_bar.set_fraction(fraction);
}
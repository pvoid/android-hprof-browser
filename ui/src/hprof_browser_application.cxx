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
#include "hprof_browser_application.h"

#include "actions.h"

#include <iostream>

using namespace hprof;

HprofBrowserApplication::HprofBrowserApplication() : 
    Application("com.github.pvoid.android-hprof-browser"), _dispatcher(EventsDisparcher::create()), 
    _hprof_storage(data_reader_factory_t::create()), _main_window(*_dispatcher, _hprof_storage) {
}

void HprofBrowserApplication::on_startup() {
    Application::on_startup();

    add_action("file_open", sigc::mem_fun(*this, &HprofBrowserApplication::on_open_file));
    add_action("quit", sigc::mem_fun(*this, &HprofBrowserApplication::on_quit));

    auto menu_bar = Gio::Menu::create();
    menu_bar->append("Open heap file", "app.file_open");
    menu_bar->append("Quit", "app.quit");
    set_app_menu(menu_bar);
}

void HprofBrowserApplication::on_activate() {
    _dispatcher->subscribe(&_hprof_storage);
    _dispatcher->start();

    add_window(_main_window);
    _main_window.show();
}

void HprofBrowserApplication::on_quit() {
    quit();

    auto windows = get_windows();
    for(auto it = std::begin(windows); it != std::end(windows); ++it) {
        (*it)->hide();
    }
}

void HprofBrowserApplication::on_open_file() {
    Gtk::FileChooserDialog dialog {"Please choose heap profile file", Gtk::FILE_CHOOSER_ACTION_OPEN};

    dialog.set_transient_for(_main_window);
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
            _dispatcher->emit(std::move(action));
            break;
        }
    }
}

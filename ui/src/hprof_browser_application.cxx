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

using namespace hprof;

HprofBrowserApplication::HprofBrowserApplication() : Application("com.github.pvoid.android-hprof-browser") {
}

void HprofBrowserApplication::on_startup() {
    Application::on_startup();

    add_action("quit", sigc::mem_fun(*this, &HprofBrowserApplication::on_quit));

    auto menu_bar = Gio::Menu::create();
    menu_bar->append("Quit", "app.quit");
    set_app_menu(menu_bar);
}

void HprofBrowserApplication::on_activate() {
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
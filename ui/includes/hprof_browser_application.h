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
#include "hprof_storage.h"
#include "treeview_storage.h"
#include "main_window.h"

namespace hprof {
    class HprofBrowserApplication : public Gtk::Application {
    public:
        HprofBrowserApplication();
    protected:
        void on_startup() override;
        void on_activate() override;
    private:
        void on_open_file();
        void on_quit();
    private:
        std::unique_ptr<EventsDisparcher> _dispatcher;
        HprofStorage _hprof_storage;
        TreeViewStorage _treeview_storage;

        MainWindow _main_window;
    };
}
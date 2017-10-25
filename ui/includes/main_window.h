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

namespace hprof {
    class MainWindow : public Gtk::ApplicationWindow {
    public:
        MainWindow(EventsDisparcher& dispatcher, HprofStorage& hprof_storage);
    private:
        void on_hprof_start_load(const std::string& file_name);
        void on_hprof_stop_load();
    private:
        EventsDisparcher& _dispatcher;
        HprofStorage& _hprof_storage;

        Gtk::HeaderBar _header_bar;
        Gtk::Box _progress_box;
        Gtk::ProgressBar _progress_bar;
        // Gtk::Spinner _spinner;
    };
}
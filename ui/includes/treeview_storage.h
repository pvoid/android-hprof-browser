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

#include "storage.h"

#include <gtkmm.h>

namespace hprof {

    class TreeViewStorage : public Storage, protected SignalsHelper {
    public:
        using type_signal_data_filled = sigc::signal<void>;
        using type_signal_data_fill_progress = sigc::signal<void, double>;
    public:
        TreeViewStorage() {}
        virtual ~TreeViewStorage() {}

        void emit(const Action& action);

        type_signal_data_filled on_data_filled() { return _signal_data_filled; }
        type_signal_data_fill_progress on_data_fill_progress() { return _signal_data_fill_progress; }
    protected:
        void on_process_signal(const StorageSignal* signal);
    private:
        void on_fill_tree_view(const FillTreeViewAction* action);
    private:
        type_signal_data_filled _signal_data_filled;
        type_signal_data_fill_progress _signal_data_fill_progress;
    };
}
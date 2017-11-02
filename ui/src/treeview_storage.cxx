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
#include "treeview_storage.h"

using namespace hprof;

enum : int32_t {
    SIGNAL_TREEVIEW_FILLED = 0,
    SIGNAL_TREEVIEW_FILL_PROGRESS = 1,
};

struct TreeViewFilledSignal : public StorageSignal {
    TreeViewFilledSignal() : StorageSignal(SIGNAL_TREEVIEW_FILLED) {}
};

struct TreeViewFillProgressSignal : public StorageSignal {
    TreeViewFillProgressSignal(double fraction) : StorageSignal(SIGNAL_TREEVIEW_FILL_PROGRESS), fraction(fraction) {}
    double fraction;
};

void TreeViewStorage::emit(const Action& action) {
    switch (action.type) {
        case Action::FillTreeView:
            on_fill_tree_view(reinterpret_cast<const FillTreeViewAction*>(&action));
            break;
        default:
            break;
    }
}

void TreeViewStorage::on_process_signal(const StorageSignal* signal) {
    switch (signal->signal) {
        case SIGNAL_TREEVIEW_FILLED:
            _signal_data_filled.emit();
            break;
        case SIGNAL_TREEVIEW_FILL_PROGRESS:
            _signal_data_fill_progress.emit(static_cast<const TreeViewFillProgressSignal*>(signal)->fraction);
            break;
    }
}

void TreeViewStorage::on_fill_tree_view(const FillTreeViewAction* action) {
    int32_t ready = 0;
    double last_fraction = 0;
    action->store->clear();
    for (auto& item : action->items) {
        action->columns->assign(action->store, item);
        double fraction =  static_cast<double>((++ready * 100.) / action->items.size()) / 100;
        if (last_fraction != fraction) {
            send_signal(std::make_unique<TreeViewFillProgressSignal>(fraction));
            last_fraction = fraction;
        }
    }
    send_signal(std::make_unique<TreeViewFilledSignal>());
}

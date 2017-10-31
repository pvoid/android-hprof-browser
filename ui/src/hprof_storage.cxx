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
#include "hprof_storage.h"
#include <iostream>

#include <functional>
#include <chrono>

using namespace hprof;

using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::duration_cast;

enum : int32_t {
    SIGNAL_START_LOADING = 1,
    SIGNAL_PROGRESS_LOADING = 2,
    SIGNAL_STOP_LOADING = 3,
    SIGNAL_QUERY_RESULT = 4,
    SIGNAL_FETCH_OBJECT_RESULT = 5,
};

struct HprofFileLoadStartedSignal : public StorageSignal {
    std::string file_name;
    HprofFileLoadStartedSignal(const std::string& file_name) : StorageSignal(SIGNAL_START_LOADING), file_name(file_name) {
    }
};

struct HprofFileLoadProgressSignal : public StorageSignal {
    std::string action;
    double fraction;
    HprofFileLoadProgressSignal(const std::string& action, double fraction) : StorageSignal(SIGNAL_PROGRESS_LOADING), action(action), fraction(fraction) {
    }
};

struct HprofFileLoadFinishedSignal : public StorageSignal {
    HprofFileLoadFinishedSignal() : StorageSignal(SIGNAL_STOP_LOADING) {}
};

struct QueryResultSignal : public StorageSignal {
    QueryResultSignal(std::unique_ptr<std::vector<heap_item_ptr_t>>&& result, u_int64_t seq_number) : 
        StorageSignal(SIGNAL_QUERY_RESULT), result(std::move(result)), seq_number(seq_number) {}

    std::unique_ptr<std::vector<heap_item_ptr_t>> result;
    u_int64_t seq_number;
};

struct FetchObjectResultSignal : public StorageSignal {
    FetchObjectResultSignal(u_int64_t seq_number, const Gtk::TreeModel::Path& path, const heap_item_ptr_t& item) :
        StorageSignal(SIGNAL_FETCH_OBJECT_RESULT), item(item), seq_number(seq_number), path(path) {}
    
    heap_item_ptr_t item;
    u_int64_t seq_number;
    Gtk::TreeModel::Path path;
};

HprofStorage::HprofStorage(std::unique_ptr<data_reader_factory_t>&& factory) : _reader_factory(std::move(factory)) {}

HprofStorage::~HprofStorage() {}

void HprofStorage::emit(const Action& action) {
    switch (action.type) {
        case Action::OpenFile:
            load_hprof(reinterpret_cast<const OpenFileAction *>(&action));
            break;
        case Action::ExecuteQuery:
            execute_query(reinterpret_cast<const ExecuteQueryAction *>(&action));
            break;
        case Action::FetchObject:
            fetch_object(reinterpret_cast<const FetchObjectAction *>(&action));
            break;
    }
}

void HprofStorage::load_hprof(const OpenFileAction* action) {
    send_signal(std::make_unique<HprofFileLoadStartedSignal>(action->file_name));

    auto start = steady_clock::now();

    file_t file { action->file_name };
    auto dump = file.read_dump(*_reader_factory, std::bind(&HprofStorage::on_loading_progress, this, std::placeholders::_1, std::placeholders::_2));
    if (dump != nullptr) {
        _heap_profile = std::move(dump);
    }

    send_signal(std::make_unique<HprofFileLoadFinishedSignal>());
    auto spent_time = steady_clock::now() - start;
    
    std::cout << "Heap dump lodaded in " << duration_cast<seconds>(spent_time).count() << "s "
                << (duration_cast<milliseconds>(spent_time).count() % 1000) << "ms " << std::endl << std::endl;
}

void HprofStorage::on_process_signal(const StorageSignal* signal) {
    switch (signal->signal) {
        case SIGNAL_START_LOADING:
            _signal_start_loading.emit(static_cast<const HprofFileLoadStartedSignal*>(signal)->file_name);
            break;
        case SIGNAL_PROGRESS_LOADING:
            _signal_progress_loading.emit(static_cast<const HprofFileLoadProgressSignal*>(signal)->action, static_cast<const HprofFileLoadProgressSignal*>(signal)->fraction);
            break;
        case SIGNAL_STOP_LOADING:
            _signal_stop_loading.emit();
            break;
        case SIGNAL_QUERY_RESULT: {
            auto s = static_cast<const QueryResultSignal*>(signal);
            _signal_query_succeed.emit(*(s->result), s->seq_number);
            break;
        }
        case SIGNAL_FETCH_OBJECT_RESULT: {
            auto s = static_cast<const FetchObjectResultSignal*>(signal);
            _signal_fetch_object_result.emit(s->seq_number, s->path, s->item);
        }
    }
}

void HprofStorage::on_loading_progress(file_t::phase_t phase, u_int32_t progress) {
    std::string action;
    switch (phase) {
        case file_t::PHASE_READ:
            action = "Reading file...";
            break;
        case file_t::PHASE_PREPARE:
            action = "Preparing dump data...";
            break;
        case file_t::PHASE_ANALYZE:
            action = "Analyzing dump data...";
            break;
    }

    send_signal(std::make_unique<HprofFileLoadProgressSignal>(action, static_cast<double>(progress) / 100.));
}

void HprofStorage::execute_query(const ExecuteQueryAction* action) {
    if (_heap_profile != nullptr && _query_parser.parse(action->query_text)) {
        auto result = std::make_unique<std::vector<heap_item_ptr_t>>();
        _heap_profile->query(_query_parser.query(), *result);
        send_signal(std::make_unique<QueryResultSignal>(std::move(result), action->seq_number));
    }
    // TODO: send errors back to ui
}

void HprofStorage::fetch_object(const FetchObjectAction* action) {
    if (_heap_profile == nullptr) return;

    auto item = _heap_profile->objects_index().find_object(action->object_id);
    if (item == nullptr) return;

    send_signal(std::make_unique<FetchObjectResultSignal>(action->seq_number, action->path, item));
}
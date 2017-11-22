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
#include "hprof.h"
#include "hprof_file.h"
#include "language_driver.h"

#include <gtkmm.h>
#include <memory>
#include <mutex>
#include <vector>
#include <condition_variable>

namespace hprof {
    class HprofStorage : public Storage, protected SignalsHelper {
    public:
        using type_signal_start_loading = sigc::signal<void, const std::string&>;
        using type_signal_progress_loading = sigc::signal<void, const std::string&, double>;
        using type_signal_stop_loading = sigc::signal<void>;
        using type_signal_query_succeed = sigc::signal<void, const std::vector<heap_item_ptr_t>&, u_int64_t>;
        using type_signal_query_failed = sigc::signal<void, const std::vector<parse_error>&, u_int64_t>;
        using type_signal_fetch_object_result = sigc::signal<void, u_int64_t, const Gtk::TreeModel::Path&, const heap_item_ptr_t&>;
    public:
        HprofStorage(std::unique_ptr<data_reader_factory_t>&& factory);
        virtual ~HprofStorage();
        void emit(const Action& action);
        
        type_signal_start_loading& on_start_loading() { return _signal_start_loading; }
        type_signal_progress_loading& on_progress_loading() { return _signal_progress_loading; }
        type_signal_stop_loading& on_stop_loading() { return _signal_stop_loading;  }
        type_signal_query_succeed& on_query_succeed() { return _signal_query_succeed; }
        type_signal_query_failed& on_query_failed() { return _signal_query_failed; }
        type_signal_fetch_object_result& on_fetch_object_result() { return _signal_fetch_object_result; }
    protected:
        void on_process_signal(const StorageSignal* signal);
    private:
        void load_hprof(const OpenFileAction* action);
        void execute_query(const ExecuteQueryAction* action);
        void fetch_object(const FetchObjectAction* action);
        void on_loading_progress(file_t::phase_t phase, u_int32_t progress);
    private:
        std::unique_ptr<data_reader_factory_t> _reader_factory;
        std::unique_ptr<heap_profile_t> _heap_profile;
        language_driver _query_parser;

        // signals
        type_signal_start_loading _signal_start_loading;
        type_signal_progress_loading _signal_progress_loading;
        type_signal_stop_loading _signal_stop_loading;
        type_signal_query_succeed _signal_query_succeed;
        type_signal_query_failed _signal_query_failed;
        type_signal_fetch_object_result _signal_fetch_object_result;
    };
}
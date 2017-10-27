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

#include <gtkmm.h>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace hprof {
    class HprofStorage : public Storage, protected SignalsHelper {
    public:
        using type_signal_start_loading = sigc::signal<void, const std::string&>;
        using type_signal_progress_loading = sigc::signal<void, const std::string&, double>;
        using type_signal_stop_loading = sigc::signal<void>;
    public:
        HprofStorage(std::unique_ptr<data_reader_factory_t>&& factory);
        virtual ~HprofStorage();
        void emit(const Action& action);
        
        type_signal_start_loading on_start_loading() { return _signal_start_loading; }
        type_signal_progress_loading on_progress_loading() { return _signal_progress_loading; }
        type_signal_stop_loading on_stop_loading() { return _signal_stop_loading;  }
    protected:
        void on_process_signal(const StorageSignal* signal);
    private:
        void load_hprof(const OpenFileAction* action);
        void on_loading_progress(file_t::phase_t phase, u_int32_t progress);
    private:
        // std::mutex _mutex;
        // std::condition_variable _condition;
        std::unique_ptr<data_reader_factory_t> _reader_factory;
        std::unique_ptr<heap_profile_t> _heap_profile;
        type_signal_start_loading _signal_start_loading;
        type_signal_progress_loading _signal_progress_loading;
        type_signal_stop_loading _signal_stop_loading;
    };
}
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

using namespace hprof;

enum : int32_t {
    SIGNAL_START_LOADING = 1,
    SIGNAL_PROGRESS_LOADING = 2,
    SIGNAL_STOP_LOADING = 3
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

HprofStorage::HprofStorage(std::unique_ptr<data_reader_factory_t>&& factory) : _reader_factory(std::move(factory)) {
}

HprofStorage::~HprofStorage() {}

void HprofStorage::emit(const Action& action) {
    switch (action.type) {
        case Action::OpenFile:
            load_hprof(reinterpret_cast<const OpenFileAction *>(&action));
            break;
    }
}

void HprofStorage::load_hprof(const OpenFileAction* action) {
    send_signal(std::make_unique<HprofFileLoadStartedSignal>(action->file_name));

    file_t file { action->file_name };
    auto dump = file.read_dump(*_reader_factory, std::bind(&HprofStorage::on_loading_progress, this, std::placeholders::_1, std::placeholders::_2));
    if (dump != nullptr) {
        _heap_profile = std::move(dump);
    }

    send_signal(std::make_unique<HprofFileLoadFinishedSignal>());
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

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
#include "hprof_file.h"
#include <iostream>

using namespace hprof;

enum : int32_t {
    SIGNAL_START_LOADING = 1,
    SIGNAL_STOP_LOADING = 2
};

struct LoadHprofFileSignal : public StorageSignal {
    std::string file_name;

    LoadHprofFileSignal(const std::string& file_name) : StorageSignal(SIGNAL_START_LOADING), file_name(file_name) {
    }
};

struct HprofFileLoadedSignal : public StorageSignal {
    HprofFileLoadedSignal() : StorageSignal(SIGNAL_STOP_LOADING) {}
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
    file_t file { action->file_name };
    if (!file.open(*_reader_factory)) {  
        return;
    }

    send_signal(std::make_unique<LoadHprofFileSignal>(action->file_name));

    auto dump = file.read_dump();
    if (dump != nullptr) {
        _heap_profile = std::move(dump);
    }

    send_signal(std::make_unique<HprofFileLoadedSignal>());
}

void HprofStorage::on_process_signal(const StorageSignal* signal) {
    switch (signal->signal) {
        case SIGNAL_START_LOADING:
            _signal_start_loading.emit(static_cast<const LoadHprofFileSignal*>(signal)->file_name);
            break;
        case SIGNAL_STOP_LOADING:
            _signal_stop_loading.emit();
            break;
    }
}
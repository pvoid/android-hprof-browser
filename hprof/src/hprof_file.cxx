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
#include "hprof_file.h"
#include <limits>

using namespace hprof;

file_t::file_t(const std::string& name) : _file_name(name) {
}

file_t::~file_t() {
}

std::unique_ptr<heap_profile_t> file_t::read_dump(const data_reader_factory_t& factory, const progress_callback& callback) const {
    auto in = std::ifstream { _file_name, std::ios::binary };
    if (!in.is_open()) {
        return nullptr;
    }

    auto file_magic = factory.read_magic(in);
    if (file_magic.empty()) {
        return nullptr;
    }

    size_t read_progress = std::numeric_limits<size_t>::max();
    auto stream = hprof_istream_t { std::move(in), [&callback, &read_progress] (auto done, auto total) { 
        auto progress = done * 100 / total;
        if (read_progress != progress) {
            read_progress = progress;
            callback(PHASE_READ, read_progress);
        }
    }};
    auto reader = factory.reader(file_magic);

    if (reader == nullptr) {
        return nullptr;
    }

    u_int32_t prepare_progress = std::numeric_limits<u_int32_t>::max();
    return reader->build(stream, [&callback, &prepare_progress] (auto done, auto total) {
        auto progress = done * 100 / total;
        if (prepare_progress != progress) {
            prepare_progress = progress;
            callback(PHASE_PREPARE, prepare_progress);
        }
    });
}

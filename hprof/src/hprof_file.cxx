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

using namespace hprof;

file_t::file_t(const std::string& name, std::unique_ptr<data_reader_factory_t>&& factory) : _file_name(name), _factory(std::move(factory)) {
}

file_t::~file_t() {
}

bool file_t::open() {
    auto in = std::ifstream { _file_name, std::ios::binary };
    if (!in.is_open()) {
        return false;
    }

    _file_magic = _factory->read_magic(in);
    if (_file_magic.empty()) {
        return false;
    }

    _stream = std::move(hprof_istream { std::move(in) });
    return true;
}

std::unique_ptr<dump_data_t> file_t::read_dump() const {
    if (!is_open() || _file_magic.empty()) {
        return std::unique_ptr<dump_data_t>();
    }

    const data_reader_t* reader = _factory->reader(_file_magic);
    if (reader == nullptr) {
        return std::unique_ptr<dump_data_t>();
    }

    return reader->build(_stream);
}

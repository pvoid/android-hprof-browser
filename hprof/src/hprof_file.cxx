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

file_t::file_t(const std::string& name) : _file_name(name), _reader(nullptr) {
}

file_t::~file_t() {
}

bool file_t::open(data_reader_factory_t& factory) {
    auto in = std::ifstream { _file_name, std::ios::binary };
    if (!in.is_open()) {
        return false;
    }

    _file_magic = factory.read_magic(in);
    if (_file_magic.empty()) {
        return false;
    }

    _stream = hprof_istream_t { std::move(in) };
    _reader = factory.reader(_file_magic);
    return _reader != nullptr;
}

std::unique_ptr<heap_profile_t> file_t::read_dump() const {
    if (!is_open() || _file_magic.empty() || _reader == nullptr) {
        return std::unique_ptr<heap_profile_t>();
    }

    return _reader->build(_stream);
}

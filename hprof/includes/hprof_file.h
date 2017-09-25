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

#include "hprof_data_reader_factory.h"

#include <memory>
#include <string>
#include <iostream>

namespace hprof {
    class file_t {
    public:
        explicit file_t(const std::string& name, std::unique_ptr<data_reader_factory_t>&& factory);
        virtual ~file_t();

        bool open();
        bool is_open() const { return _stream.is_open(); }
        void close() { _stream.close(); }
        std::unique_ptr<dump_data_t> read_dump() const;
    private:
        std::string _file_name;
        std::string _file_magic;
        std::unique_ptr<data_reader_factory_t> _factory;
        mutable hprof_istream _stream;
    };
}

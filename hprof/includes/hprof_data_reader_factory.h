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

#include "hprof_data_reader.h"

#include <memory>
#include <iostream>
#include <string>

namespace hprof {
    class data_reader_factory_t {
    public:
        virtual std::string read_magic(std::istream& in) const = 0;
        virtual const data_reader_t* reader(const std::string& signature) const = 0;
        static std::unique_ptr<data_reader_factory_t> create();
    };
}

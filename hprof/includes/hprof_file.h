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

#include "hprof.h"

#include <memory>
#include <string>
#include <iostream>
#include <functional>

namespace hprof {
    class file_t {
    public:
        enum phase_t {
            PHASE_READ,
            PHASE_PREPARE,
            PHASE_ANALYZE
        };

        using progress_callback = std::function<void (phase_t, u_int32_t)>;
    public:
        explicit file_t(const std::string& name);
        virtual ~file_t();

        std::unique_ptr<heap_profile_t> read_dump(const data_reader_factory_t&, const progress_callback&) const;
    private:
        std::string _file_name;
    };
}

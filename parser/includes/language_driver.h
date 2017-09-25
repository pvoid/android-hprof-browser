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

#include "location.hh"
#include "language_parser.h"

namespace hprof {
    class language_driver {
    public:
        language_driver() {}
        virtual ~language_driver() {}

        int parse(const std::string& text);

        void error(const hprof::location& loc, const std::string& msg);
        void error(const std::string& msg);
    };
}

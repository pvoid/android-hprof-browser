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

#include "language_scanner.h"
#include "language_driver.h"

#include <iostream>
#include <sstream>

using namespace hprof;

bool language_driver::parse (const std::string& text) {
    std::istringstream in { text};
    language_scanner scanner { &in };
    language_parser parser(*this, scanner);
    return parser.parse() == 0;
}

void language_driver::action(query_t::action_t action) {
    _query.action = action;
}

void language_driver::source(query_t::source_t source) {
    _query.source = source;
}

void language_driver::filter(filter_t* filter) {
    _query.filter.reset(filter);
}

void language_driver::error (const hprof::location& loc, const std::string& msg) {
    std::cerr << loc << ": " << msg << std::endl;
}

void language_driver::error (const std::string& msg) {
    std::cerr << msg << std::endl;
}

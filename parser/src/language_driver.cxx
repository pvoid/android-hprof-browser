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

int language_driver::parse (const std::string& text) {
    std::istringstream in { text};
    language_scanner scanner { &in };
    language_parser parser(*this, scanner);

    return parser.parse();
}

void language_driver::error (const hprof::location& loc, const std::string& msg) {
    std::cerr << loc << ": " << msg << std::endl;
}

void language_driver::error (const std::string& msg) {
    std::cerr << msg << std::endl;
}

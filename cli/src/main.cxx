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
#include <hprof_file.h>

#include <iostream>
#include <chrono>
#include <algorithm>

#include "language_driver.h"
#include "tools.h"

using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::milliseconds;

using namespace hprof;

int main(int argc, char* argv[]) {

    if (argc == 1) {
        std::cerr << "Specify hprof file name" << std::endl;
        return -1;
    }

    std::cout << "Loading heap dump from: " << argv[1] << std::endl;
    file_t file { argv[1], data_reader_factory_t::create() };
    if (!file.open()) {
        return -1;
    }

    auto start = steady_clock::now();

    auto hprof = file.read_dump();
    file.close();

    auto spent_time = steady_clock::now() - start;

    std::cout << "Heap dump lodaded in " << duration_cast<seconds>(spent_time).count() << "s "
              << (duration_cast<milliseconds>(spent_time).count() % 1000) << "ms " << std::endl << std::endl;

    if (hprof == nullptr) {
        std::cout << "Error reading heap profile file" << std::endl;
        return -1;
    }

    if (hprof->has_errors()) {
        std::cout << hprof->error_message() << std::endl;
        return -1;
    }

    language_driver driver {};
    std::vector<heap_item_ptr_t> result;
    do {
        std::cout << ">> ";
        std::string query_text;
        std::getline(std::cin, query_text);

        if (query_text == "exit") {
            break;
        }

        if (!driver.parse(query_text)) {
            continue;
        }

        start = steady_clock::now();
        if (hprof->query(driver.query(), result)) {
            std::cout << "Results: " << result.size() << std::endl << std::endl;

            for (auto& item : result) {
                print_object(item, hprof->objects_index(), 3);
            }
        } else {
            std::cout << "Failed";
        }

        auto spent_time = steady_clock::now() - start;
        std::cout << std::endl
                  << "Execution time " << duration_cast<seconds>(spent_time).count() << "s "
                  << (duration_cast<milliseconds>(spent_time).count() % 1000) << "ms " << std::endl;

        result.clear();
    } while (true);



    return 0;
}

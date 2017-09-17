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

    file_t file { argv[1], data_reader_factory_t::create() };
    if (!file.open()) {
        return -1;
    }

    const auto start = steady_clock::now();

    auto hprof = file.read_dump();
    file.close();

    hprof->analyze();

    std::vector<class_info_ptr_t> classes;

    hprof->get_classes(classes, filter_class_name_t { "android.view.View" });

    std::cout << "Classes: " << classes.size() << std::endl << std::endl;

    std::sort(std::begin(classes), std::end(classes),
        [] (auto& left, auto& right) -> bool { return left->instances.size() > right->instances.size(); });

    for (auto& item : classes) {
        std::cout << hprof->get_string(item->name_id) << " instances: " << item->instances.size() << std::endl;
    }

    auto spent_time = steady_clock::now() - start;

    std::cout << std::endl
              << "Time spent: " << duration_cast<seconds>(spent_time).count() << "s "
              << (duration_cast<milliseconds>(spent_time).count() % 1000) << "ms " << std::endl;
}

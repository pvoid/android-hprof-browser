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

#include "hprof.h"
#include "reader/data_reader_v103.h"

using std::string;
using std::istream;
using namespace hprof;

data_reader_v103_t g_reader_103 {};

constexpr size_t MAX_MAGIC_LEN = 18;

class data_reader_factory_impl : public data_reader_factory_t {
    public:
        data_reader_factory_impl() {}
        virtual ~data_reader_factory_impl() {}

        string read_magic(istream& in) const override;
        const data_reader_t* reader(const string& signature) const override;
};

std::unique_ptr<data_reader_factory_t> data_reader_factory_t::create() {
    return std::make_unique<data_reader_factory_impl>();
}

string data_reader_factory_impl::read_magic(istream& in) const {
    string result {};
    result.reserve(MAX_MAGIC_LEN);
    char ch;

    for (size_t index = 0; index <= MAX_MAGIC_LEN; ++index) {
        in.get(ch);

        if (ch == '\0') {
            return result;
        }

        if (in.eof()) {
            break;
        }

        result += ch;
    }

    return string {};
}

const data_reader_t* data_reader_factory_impl::reader(const string& signature) const {
    if ("JAVA PROFILE 1.0.3" == signature) {
        return &g_reader_103;
    }
    return nullptr;
}

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
#include <gtest/gtest.h>
#include <istream>

#include "hprof_data_reader_factory.h"
#include "membuf.h"

using namespace hprof;

TEST(ReaderFactoryTests, Signature) {
    auto impl = data_reader_factory_t::create();
    ASSERT_NE(nullptr, impl->reader("JAVA PROFILE 1.0.3"));
    ASSERT_EQ(nullptr, impl->reader("JAVA PROFILE 1.0.4"));
    ASSERT_EQ(nullptr, impl->reader("JAVA PROFILE 1.0.0"));
    ASSERT_EQ(nullptr, impl->reader("RANDOM STRING"));
    ASSERT_EQ(nullptr, impl->reader(""));
}

TEST(ReaderFactoryTests, MagicKnown) {
    auto impl = data_reader_factory_t::create();
    char magic[] = "JAVA PROFILE 1.0.3";
    tests::membuf buff { magic, sizeof(magic) };
    std::istream in { &buff };

    ASSERT_EQ(magic, impl->read_magic(in));
}

TEST(ReaderFactoryTests, MagicUnknwon) {
    auto impl = data_reader_factory_t::create();
    char magic[] = "JAVA PROFILE 1.0.4";
    tests::membuf buff { magic, sizeof(magic) };
    std::istream in { &buff };

    ASSERT_EQ(magic, impl->read_magic(in));
}

TEST(ReaderFactoryTests, MagicTooLong) {
    auto impl = data_reader_factory_t::create();
    char magic[] = "JAVA PROFILE 1.0.4AAAAAZZZZ";
    tests::membuf buff { magic, sizeof(magic) };
    std::istream in { &buff };

    ASSERT_EQ("", impl->read_magic(in));
}

TEST(ReaderFactoryTests, MagicNoZeroChar) {
    auto impl = data_reader_factory_t::create();
    char magic[] = "JAVA PROFILE 1.0.3";
    tests::membuf buff { magic, sizeof(magic) - 1 };
    std::istream in { &buff };

    ASSERT_EQ("", impl->read_magic(in));
}

TEST(ReaderFactoryTests, MagicEmpty) {
    auto impl = data_reader_factory_t::create();
    char magic[] = "";
    tests::membuf buff { magic, sizeof(magic) };
    std::istream in { &buff };

    ASSERT_EQ("", impl->read_magic(in));
}

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
#include "hprof_istream.h"

TEST(HprofIStream, Char) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x10, in.read_char());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0a, in.read_char());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x1f, in.read_char());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0, in.read_char());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, Byte) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x10, in.read_byte());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0a, in.read_byte());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x1f, in.read_byte());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0, in.read_byte());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, Short) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x100a, in.read_int16());
    ASSERT_FALSE(in.eof());

    // Not enough data
    ASSERT_EQ(0, in.read_int16());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, IntNotEnoughData) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x0, in.read_int32());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, IntSmall) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-small-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x18, in.read_int32());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int32());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, Int) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0xffffff04, in.read_int32());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int32());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, LongNotEnoughData) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x0, in.read_int64());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, LongSmall) {
    std::ifstream data { TEST_DATA_DIR "/istream-long-small-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0x12, in.read_int64());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int64());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, Long) {
    std::ifstream data { TEST_DATA_DIR "/istream-long-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    ASSERT_EQ(0xffffffffffffffc0, in.read_int64());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int64());
    ASSERT_TRUE(in.eof());
}

TEST(HprofIStream, BytesExact) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    u_int8_t bytes[3] = {0};

    ASSERT_EQ(3, in.read_bytes(bytes, 3));

    ASSERT_EQ(0x10, bytes[0]);
    ASSERT_EQ(0x0a, bytes[1]);
    ASSERT_EQ(0x1f, bytes[2]);
}

TEST(HprofIStream, BytesExactTemplate) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    u_int8_t bytes[3] = {0};

    ASSERT_EQ(3, in.read_bytes(bytes));

    ASSERT_EQ(0x10, bytes[0]);
    ASSERT_EQ(0x0a, bytes[1]);
    ASSERT_EQ(0x1f, bytes[2]);
}


TEST(HprofIStream, BytesNotEnough) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream in { std::move(data) };

    u_int8_t bytes[4] = {0};

    ASSERT_EQ(0, in.read_bytes(bytes, 4));
}

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

auto g_empty_callback = [] (auto, auto) {};

TEST(hprof_istream_t, When_ReadChar_Expect_NextByteValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };
    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x10, in.read_char());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0a, in.read_char());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x1f, in.read_char());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0, in.read_char());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_ReadChar_Expect_OneByteCounted) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };
    size_t total = 0;
    size_t read = 0;
    hprof_istream_t in { std::move(data), [&total, &read] (auto done, auto size) { total = size; read = done; } };

    in.read_char();

    ASSERT_EQ(1, read);
    ASSERT_EQ(3, total);
}

TEST(hprof_istream_t, When_ReadByte_Expect_NextByteValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x10, in.read_byte());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0a, in.read_byte());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x1f, in.read_byte());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0, in.read_byte());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_ReadByte_Expect_OneByteCounted) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };
    size_t total = 0;
    size_t read = 0;
    hprof_istream_t in { std::move(data), [&total, &read] (auto done, auto size) { total = size; read = done; } };

    in.read_byte();

    ASSERT_EQ(1, read);
    ASSERT_EQ(3, total);
}

TEST(hprof_istream_t, When_ReadShort_Expect_NextTwoBytesValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x100a, in.read_int16());
    ASSERT_FALSE(in.eof());

    // Not enough data
    ASSERT_EQ(0, in.read_int16());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_ReadInt16_Expect_TwoByteCounted) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };
    size_t total = 0;
    size_t read = 0;
    hprof_istream_t in { std::move(data), [&total, &read] (auto done, auto size) { total = size; read = done; } };

    in.read_int16();

    ASSERT_EQ(2, read);
    ASSERT_EQ(3, total);
}

TEST(hprof_istream_t, When_NotEnoughDataForInt_Expect_Zero) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x0, in.read_int32());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_OneByteIntValue_Expect_NextIntValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-small-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x18, in.read_int32());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int32());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_FourBytesIntValue_Expect_NextIntValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0xffffff04, in.read_int32());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int32());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_ReadInt32_Expect_FourByteCounted) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-data.bin", std::ios::binary };
    size_t total = 0;
    size_t read = 0;
    hprof_istream_t in { std::move(data), [&total, &read] (auto done, auto size) { total = size; read = done; } };

    in.read_int32();

    ASSERT_EQ(4, read);
    ASSERT_EQ(4, total);
}

TEST(hprof_istream_t, When_NotEnoughDataForLong_Expect_Zero) {
    std::ifstream data { TEST_DATA_DIR "/istream-int-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x0, in.read_int64());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_OneByteLongValue_Expect_NextLongValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-long-small-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0x12, in.read_int64());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int64());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_FourBytesLongValue_Expect_NextLongValue) {
    std::ifstream data { TEST_DATA_DIR "/istream-long-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    ASSERT_EQ(0xffffffffffffffc0, in.read_int64());
    ASSERT_FALSE(in.eof());

    ASSERT_EQ(0x0, in.read_int64());
    ASSERT_TRUE(in.eof());
}

TEST(hprof_istream_t, When_ReadInt64_Expect_EightByteCounted) {
    std::ifstream data { TEST_DATA_DIR "/istream-long-data.bin", std::ios::binary };
    size_t total = 0;
    size_t read = 0;
    hprof_istream_t in { std::move(data), [&total, &read] (auto done, auto size) { total = size; read = done; } };

    in.read_int64();

    ASSERT_EQ(8, read);
    ASSERT_EQ(8, total);
}


TEST(hprof_istream_t, When_ReadSpecifiedBytesAmount_Expect_FillArrayByData) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    u_int8_t bytes[3] = {0};

    ASSERT_EQ(3, in.read_bytes(bytes, 3));

    ASSERT_EQ(0x10, bytes[0]);
    ASSERT_EQ(0x0a, bytes[1]);
    ASSERT_EQ(0x1f, bytes[2]);
}

TEST(hprof_istream_t, When_ReadNotSpecifiedBytesAmount_Expect_FillArrayByData) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };

    hprof_istream_t in { std::move(data), g_empty_callback };

    u_int8_t bytes[3] = {0};

    ASSERT_EQ(3, in.read_bytes(bytes));

    ASSERT_EQ(0x10, bytes[0]);
    ASSERT_EQ(0x0a, bytes[1]);
    ASSERT_EQ(0x1f, bytes[2]);
}


TEST(hprof_istream_t, When_NotEnoughData_Expect_ZeroBytesRead) {
    std::ifstream data { TEST_DATA_DIR "/istream-char-data.bin", std::ios::binary };
    hprof_istream_t in { std::move(data), g_empty_callback };

    u_int8_t bytes[4] = {0};

    ASSERT_EQ(0, in.read_bytes(bytes, 4));
}

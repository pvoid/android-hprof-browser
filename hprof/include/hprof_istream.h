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

#include <fstream>

class hprof_istream {
public:
    hprof_istream() {}

    explicit hprof_istream(std::ifstream&& in) : _stream(std::move(in)) {}
    ~hprof_istream() {
        _stream.close();
    }

    hprof_istream(const hprof_istream&) = delete;
    hprof_istream(hprof_istream&&) = default;

    hprof_istream& operator=(const hprof_istream&) = delete;
    hprof_istream& operator=(hprof_istream&&) = default;

    bool is_open() const { return _stream.is_open(); }

    bool eof() const { return _stream.eof(); }

    void close() {
        _stream.close();
    }

    u_int8_t read_byte() {
        char data;
        _stream.get(data);
        return static_cast<u_int8_t>(data);
    }

    char read_char() {
        char data;
        _stream.get(data);
        return data;
    }

    int32_t read_int16() {
        int16_t result = 0;
        for (int offset = 8; offset >= 0; offset -= 8) {
            int val = read_byte();
            if (eof()) return 0;
            result |= val << offset;
        }
        return result;
    }

    int32_t read_int32() {
        int32_t result = 0;
        for (int offset = 24; offset >= 0; offset -= 8) {
            int val = read_byte();
            if (eof()) return 0;
            result |= val << offset;
        }
        return result;
    }

    int64_t read_int64() {
        int64_t result = 0;
        for (int offset = 56; offset >= 0; offset -= 8) {
            int64_t val = read_byte();
            if (eof()) return 0;
            result |= val << offset;
        }
        return result;
    }

    template<size_t SIZE>
    size_t read_bytes(u_int8_t (&buff)[SIZE]) {
        return read_bytes(buff, SIZE);
    }

    size_t read_bytes(u_int8_t* buff, size_t size) {
        _stream.read(reinterpret_cast<char *>(buff), size);
        return _stream.good() ? size : 0;
    }

private:
    std::ifstream _stream;
};

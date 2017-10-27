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
#include <functional>

class hprof_istream_t {
public:
    using progress_listener = std::function<void(size_t, size_t)>;
public:
    hprof_istream_t(std::ifstream&& in, progress_listener&& listener) : 
                        _stream(std::move(in)), _listener(std::move(listener)), _file_size(0), _read(0) {
        _read = _stream.tellg();
        _stream.seekg(0, std::ios_base::end);
        _file_size = _stream.tellg();
        _stream.seekg(_read);
    }

    ~hprof_istream_t() {
        _stream.close();
    }

    hprof_istream_t(const hprof_istream_t&) = delete;
    hprof_istream_t(hprof_istream_t&&) = default;

    hprof_istream_t& operator=(const hprof_istream_t&) = delete;
    hprof_istream_t& operator=(hprof_istream_t&&) = default;

    bool is_open() const { return _stream.is_open(); }

    bool eof() const { return _stream.eof(); }

    void close() {
        _stream.close();
    }

    u_int8_t read_byte() {
        char data = 0;
        _stream.get(data);
        change_read_count(1);
        return static_cast<u_int8_t>(data);
    }

    char read_char() {
        char data = 0;
        change_read_count(1);
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
        auto count = _stream.good() ? size : 0;
        change_read_count(count);
        return count;
    }

    size_t stream_size() const {
        return _file_size;
    }

    size_t stream_read() const {
        return _read;
    }
private:
    void change_read_count(size_t size) {
        _read += size;
        _listener(_read, _file_size);
    }
private:
    std::ifstream _stream;
    progress_listener _listener;
    size_t _file_size;
    size_t _read;
};

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

#include "types.h"
#include "hprof_istream.h"
#include "filters/base.h"
#include "filters/apply_to_field.h"
#include "filters/classname.h"
#include "filters/instance_of.h"
#include "filters/logical.h"
#include "filters/comparation.h"

#include <chrono>
#include <functional>

namespace hprof {

    struct query_t {
        enum action_t {
            ACTION_SHOW
        } action;
        enum source_t {
            SOURCE_OBJECTS,
            SOURCE_CLASSES
        } source;
        std::unique_ptr<filter_t> filter;
    };

    class heap_profile_t {
    public:
        virtual ~heap_profile_t() {}
        virtual bool has_errors() const = 0;
        virtual const std::string& error_message() const = 0;
        virtual bool query(const query_t& query, std::vector<heap_item_ptr_t>& result) const = 0;
        virtual const objects_index_t& objects_index() const = 0;
        virtual const classes_index_t& classes_index() const = 0;
    };

    class data_reader_t {
    public:
        using progress_callback = std::function<void(u_int32_t done, u_int32_t total)>;
    public:
        virtual ~data_reader_t() {}
        virtual std::unique_ptr<heap_profile_t> build(hprof_istream_t&, const progress_callback& callback) const = 0;
    };

    class data_reader_factory_t {
    public:
        virtual std::string read_magic(std::istream& in) const = 0;
        virtual const data_reader_t* reader(const std::string& signature) const = 0;
        static std::unique_ptr<data_reader_factory_t> create();
    };
}

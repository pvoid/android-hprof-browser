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

namespace hprof {
    class objects_index_t {
    public:
        virtual ~objects_index_t() {}
        // FIXME: Probably it is possible to find more convinient result type
        virtual heap_item_ptr_t find_object(jvm_id_t id) const = 0;
    };

    class classes_index_t {
    public:
        virtual ~classes_index_t() {}
        // FIXME: Probably it is possible to find more convinient result type
        virtual heap_item_ptr_t find_class(jvm_id_t id) const = 0;
    };
}

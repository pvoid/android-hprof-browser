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

#include <unordered_map>
#include <memory>
#include <vector>
#include <string>
#include <set>
#include <string.h>

#include "hprof_types_base.h"
#include "hprof_types_class.h"
#include "hprof_types_instance.h"
#include "hprof_types_array.h"

namespace hprof {
    using strings_map_t = std::unordered_map<id_t, std::string>;

    struct heap_info_t {
        int32_t type;
        id_t name;
    };
}

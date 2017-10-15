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
#include "types.h"

using namespace hprof;

gc_root_t::~gc_root_t() {}

object_info_t::~object_info_t() {}

field_spec_t::~field_spec_t() {}

fields_spec_t::~fields_spec_t() {}

field_value_t::~field_value_t() {}

fields_values_t::~fields_values_t() {}

class_info_t::~class_info_t() {}

instance_info_t::~instance_info_t() {}

string_info_t::~string_info_t() {}

primitives_array_info_t::~primitives_array_info_t() {}

primitives_array_info_t::array_item_t::~array_item_t() {}

objects_array_info_t::~objects_array_info_t() {}

heap_item_t::~heap_item_t() {}

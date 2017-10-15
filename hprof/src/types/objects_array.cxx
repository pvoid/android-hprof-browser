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
#include "types/objects_array.h"

using namespace hprof;

void objects_array_info_impl_t_deleter::operator()(objects_array_info_impl_t* ptr) const {
    ptr->~objects_array_info_impl_t();
    delete[] reinterpret_cast<u_int8_t*>(ptr);
}

objects_array_info_impl_t::~objects_array_info_impl_t() {}

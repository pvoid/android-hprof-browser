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

#include "test_name_tokenizer.h"
#include "test_hprof_istream.h"
#include "test_data_reader_factory.h"
// Test types
#include "types/test_object.h"
#include "types/test_fields.h"
#include "types/test_class.h"
#include "types/test_gc_root.h"
#include "types/test_instance.h"
#include "types/test_string_instance.h"
#include "types/test_objects_array.h"
#include "types/test_primitives_array.h"
#include "test_types.h"
// Test filters
#include "filters/test_classname.h"
#include "filters/test_logical.h"
#include "filters/test_instance_of.h"

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

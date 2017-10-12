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

#include "language_driver.h"

using namespace hprof;

TEST(Parser, SourceObjects) {
    language_driver driver;
    auto result = driver.parse("show objects");
    ASSERT_TRUE(result);
    ASSERT_EQ(query_t::SOURCE_OBJECTS, driver.query().source);
    ASSERT_EQ(nullptr, driver.query().filter);
}

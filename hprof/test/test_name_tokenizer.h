///
/// Copyright 2017 Dmitry `PVOID` Petukhov
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///      http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
#pragma once

#include <gtest/gtest.h>
#include <iostream>

#include "name_tokenizer.h"

using namespace hprof;

TEST(class_name_tokenizer, When_SaveString_Expect_BeginEquals) {
    const char *name = "com.android.ui.View";
    class_name_tokenizer tokenizer_1(name, std::strlen(name));
    class_name_tokenizer tokenizer_2(name, std::strlen(name));
    ASSERT_EQ(tokenizer_1, tokenizer_2);
}

TEST(class_name_tokenizer, When_SaveString_Expect_EndEquals) {
    const char *name = "com.android.ui.View";
    const size_t len = std::strlen(name);
    class_name_tokenizer tokenizer_1 { name + len, len };
    class_name_tokenizer tokenizer_2 { name + len, len };
    ASSERT_EQ(tokenizer_1, tokenizer_2);
}

TEST(class_name_tokenizer, When_SaveValue_Expect_BeginEquals) {
    const char *name1 = "com.android.ui.View";
    const char *name2 = "com.android.ui.View";
    class_name_tokenizer tokenizer_1(name1, std::strlen(name1));
    class_name_tokenizer tokenizer_2(name2, std::strlen(name2));
    ASSERT_EQ(tokenizer_1, tokenizer_2);
}

TEST(class_name_tokenizer, When_SaveValue_Expect_EndEquals) {
    const char *name1 = "com.android.ui.View";
    const char *name2 = "com.android.ui.View";
    const size_t len1 = std::strlen(name1);
    const size_t len2 = std::strlen(name2);
    class_name_tokenizer tokenizer_1 { name1 + len1, len1 };
    class_name_tokenizer tokenizer_2 { name2 + len2, len2 };
    ASSERT_EQ(tokenizer_1, tokenizer_2);
}

TEST(class_name_tokenizer, When_FullQualifiedName_Expect_BeginEqualsFirstSegment) {
    const char *name = "com.android.ui.View";
    class_name_tokenizer tokenizer(name, std::strlen(name));
    ASSERT_EQ("com", *tokenizer);
}

TEST(class_name_tokenizer, When_FullQualifiedName_Expect_PreIncrementIteratesOverAllSegments) {
    const char *name = "com.android.ui.View";
    size_t len = std::strlen(name);
    class_name_tokenizer begin { name, len };
    class_name_tokenizer end { name + len, 0 };

    ASSERT_NE(end, begin);
    ASSERT_EQ("com", *begin);

    ASSERT_NE(end, ++begin);
    ASSERT_EQ("android", *begin);

    ASSERT_NE(end, ++begin);
    ASSERT_EQ("ui", *begin);

    ASSERT_NE(end, ++begin);
    ASSERT_EQ("View", *begin);

    ASSERT_EQ(end, ++begin);
}

TEST(class_name_tokenizer, When_FullQualifiedName_Expect_PostIncrementIteratesOverAllSegments) {
    const char *name = "com.android.ui.View";
    size_t len = std::strlen(name);
    class_name_tokenizer begin { name, len };
    class_name_tokenizer end { name + len, 0 };

    ASSERT_NE(end, begin);
    ASSERT_EQ("com", *begin);

    ASSERT_NE(end, begin++);
    ASSERT_EQ("android", *begin);

    ASSERT_NE(end, begin++);
    ASSERT_EQ("ui", *begin);

    ASSERT_NE(end, begin++);
    ASSERT_EQ("View", *begin);

    ASSERT_NE(end, begin++);
    ASSERT_EQ(end, begin);
}

TEST(class_name_tokenizer, When_SimpleName_Expect_PreIncrementIteratesOverAllSegments) {
    const char *name = "View";
    size_t len = std::strlen(name);
    class_name_tokenizer begin { name, len };
    class_name_tokenizer end { name + len, 0 };

    ASSERT_NE(end, begin);
    ASSERT_EQ("View", *begin);

    ASSERT_EQ(end, ++begin);
}

TEST(class_name_tokenizer, When_SimpleName_Expect_PostIncrementIteratesOverAllSegments) {
    const char *name = "View";
    size_t len = std::strlen(name);
    class_name_tokenizer begin { name, len };
    class_name_tokenizer end { name + len, 0 };

    ASSERT_NE(end, begin);
    ASSERT_EQ("View", *begin);

    ASSERT_NE(end, begin++);
    ASSERT_EQ(end, begin);
}

TEST(name_tokens, When_NotNamePart_Expect_MatchDoeesNotEqualZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_NE(0, class_name.match("some"));
}

TEST(name_tokens, When_FullSegmentName_Expect_MatchEqualsZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_EQ(0, class_name.match("ui"));
}

TEST(name_tokens,  When_SegmentNamePrefix_Expect_MatchEqualsZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_EQ(0, class_name.match("andr"));
}

TEST(name_tokens, When_FullSegmentNameWrongCase_Expect_MatchEqualsZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_EQ(0, class_name.match("uI"));
}

TEST(name_tokens, When_SegmentNamePrefixWrongCase_Expect_MatchEqualsZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_EQ(0, class_name.match("aNdR"));
}

TEST(name_tokens, When_NameIsSearchPrefix_Expect_MatchDoesNotEqualZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_NE(0, class_name.match("com.android.ui.ViewGroup"));
}

TEST(name_tokens, When_FullQualifiedNamePrefix_Expect_MatchEqualsZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_EQ(0, class_name.match("com.android"));
}

TEST(name_tokens, When_FullQualifiedNamePrefixWrongCase_Expect_MatchEqualsZero) {
    name_tokens class_name = "com.android.ui.View";
    ASSERT_EQ(0, class_name.match("com.anDroid"));
}

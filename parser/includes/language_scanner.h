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

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif


#include <iostream>
#include "language_driver.h"
#include "language_parser.h"


namespace hprof {
    using location_type = hprof::language_parser::location_type;
    using semantic_type = hprof::language_parser::semantic_type;

    class language_scanner : public yyFlexLexer {
    public:
        explicit language_scanner(std::istream* in) : yyFlexLexer(in) {}
        virtual ~language_scanner() {}

        using yyFlexLexer::yylex;
        virtual hprof::language_parser::token::yytokentype yylex(semantic_type * const lval, location_type *location,
            hprof::language_driver& driver, hprof::language_scanner& scanner);
    };
}

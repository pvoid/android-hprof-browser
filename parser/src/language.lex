%{
    #include "language_scanner.h"
    #include "language_parser.h"

    #define yyterminate() return hprof::language_parser::token::END

    using token_type = hprof::language_parser::token::yytokentype;
    using semantic_type = hprof::language_parser::semantic_type;
    using location_type = hprof::language_parser::location_type;

    #undef YY_DECL
    #define YY_DECL token_type hprof::language_scanner::yylex(semantic_type * const lval, location_type *location, \
        hprof::language_driver& driver, hprof::language_scanner& scanner)

    #undef yywrap
    #define yywrap() 1

    using token = hprof::language_parser::token;
%}

%option c++
%option yyclass="hprof::language_scanner"
%option noyywrap nounput nodefault batch debug nounput case-insensitive

%{
  #define YY_USER_ACTION  _location.columns(yyleng);
%}

%%%

%{
  _location.step();
%}

[ \t]+         { _location.step(); }
[\n]+          { _location.lines(yyleng); _location.step(); }

\'[^\n\']+\'   |
\"[^\n\"]+\"    { lval->strval = strdup(yytext); return token::STRING; }

[0-9]+          { lval->intval = atoi(yytext); return token::INT; }

[0-9]+\.[0-9]* |
\.[0-9]+        { lval->floatval = atof(yytext); return token::FLOAT;}

SHOW            { return token::SHOW; }
OBJECTS         { return token::OBJECTS; }
CLASSES         { return token::CLASSES; }
HAVING          { return token::HAVING; }
OBJECT          { return token::OBJECT; }
CLASS           { return token::CLASSES; }

INSTANCEOF      { return token::INSTANCEOF; }

AND             { return token::AND; }
OR              { return token::OR; }
NOT             { return token::NOT; }

[A-Za-z][A-Za-z0-9_]* { lval->strval = strdup(yytext); return token::NAME; }

"="             { return token::COMPARISON; }
"<"             { return token::COMPARISON; }
"<="            { return token::COMPARISON; }
">"             { return token::COMPARISON; }
">="            { return token::COMPARISON; }
"!="            { return token::COMPARISON; }

%%

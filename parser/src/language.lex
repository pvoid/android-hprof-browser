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
  #define YY_USER_ACTION  location->columns(yyleng);
%}

%%%

%{
  location->step();
%}

[ \t]+          { location->step(); }
[\n]+           { location->lines(yyleng); location->step(); }

\'[^\n\']*\' |
\"[^\n\"]*\"  { lval->strval = strdup(yytext + 1); lval->strval[yyleng - 2] = '\0'; return token::STRING; }

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

"."             { return token::FIELD_ACCESS; }
"="             { return token::EQUALS; }
"!="            { return token::NOT_EQUALS; }
"<"             { return token::LESS; }
"<="            { return token::LESS_OR_EQUALS; }
">"             { return token::GREATER; }
">="            { return token::GREATER_OR_EQUALS; }

"("             { return token::LPARENT; }
")"             { return token::RPARENT; }

%%

%skeleton "lalr1.cc"
%require "3.0.4"

%defines
%define api.namespace {hprof}
%define parser_class_name {language_parser}
/*%define parse.assert*/

%code requires
{
#include <string>

namespace hprof {
    class language_driver;
    class language_scanner;
}
}

%param { hprof::language_driver& driver }
%param { hprof::language_scanner& scanner }

%locations

/*%define parse.trace*/
/*%define parse.error verbose*/

%code
{
#include "language_scanner.h"
#include "language_driver.h"

#undef yylex
#define yylex scanner.yylex
}

%union {
	int intval;
	double floatval;
	char *strval;
	int subtok;
}

%token <strval> STRING
%token <strval> NAME
%token <intval> INT
%token <intval> BOOL
%token <floatval> FLOAT

%token END 0 "end of query"
%token SHOW
%token OBJECTS
%token CLASSES
%token HAVING
%token OBJECT
%token CLASS
%token INSTANCEOF
%token AND
%token OR
%token NOT
%token COMPARISON

%%
%start query;
query: show_stmt END;

show_stmt: SHOW show_src having_stmt;

show_src: OBJECTS | CLASSES;

having_stmt:
    | HAVING filters_stmt;

filters_stmt: filter_stmt

filter_stmt: OBJECT INSTANCEOF STRING
    | OBJECT '.' NAME;

%%

void hprof::language_parser::error (const location_type& loc, const std::string& msg) {
  driver.error (loc, msg);
}

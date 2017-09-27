%skeleton "lalr1.cc"
%require "3.0.4"

%defines
%define api.namespace {hprof}
%define parser_class_name {language_parser}
/*%define parse.assert*/

%code requires
{
#include <string>
#include "filters.h"

namespace hprof {
    class language_driver;
    class language_scanner;
}
}

%param { hprof::language_driver& driver }
%param { hprof::language_scanner& scanner }

%locations

%define parse.trace
%define parse.error verbose

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
    hprof::filter_comp_value_t *compareval;
    hprof::filter_t *filterval;
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
%token EQUALS "="
%token NOT_EQUALS "!="
%token LESS "<"
%token LESS_OR_EQUALS "<="
%token GREATER ">"
%token GREATER_OR_EQUALS ">="
%token FIELD_ACCESS "."
%token LPARENT "("
%token RPARENT ")"

%type <compareval> field_value
%type <filterval> filter_stmt

%left <filterval> AND
%left <filterval> OR

%%
%start query;
query: show_stmt END;

show_stmt: SHOW show_src having_stmt { driver.action(query_t::ACTION_SHOW); };

show_src: OBJECTS { driver.source(query_t::SOURCE_OBJECTS); }
    | CLASSES { driver.source(query_t::SOURCE_CLASSES); };

having_stmt:
    | HAVING filter_stmt { driver.filter($2); };

filter_stmt:
     "(" filter_stmt ")" { $$ = $2; }
    | filter_stmt AND filter_stmt { $$ = new (std::nothrow) filter_and_t($1, $3); }
    | filter_stmt OR filter_stmt { $$ = new (std::nothrow) filter_or_t($1, $3); };
    | OBJECT INSTANCEOF STRING { $$ = new (std::nothrow) filter_instance_of_t($3); delete[] $3; }
    | OBJECT FIELD_ACCESS NAME EQUALS field_value { $$ = new (std::nothrow) filter_compare_equals_field_t($3, *$5); delete[] $3; delete $5; }
    | OBJECT FIELD_ACCESS NAME NOT_EQUALS field_value { $$ = new (std::nothrow) filter_compare_not_equals_field_t($3, *$5); delete[] $3; delete $5; }
    | OBJECT FIELD_ACCESS NAME LESS field_value { $$ = new (std::nothrow) filter_compare_less_field_t($3, *$5); delete[] $3; delete $5; }
    | OBJECT FIELD_ACCESS NAME LESS_OR_EQUALS field_value { $$ = new (std::nothrow) filter_compare_less_or_equals_field_t($3, *$5); delete[] $3; delete $5; }
    | OBJECT FIELD_ACCESS NAME GREATER field_value { $$ = new (std::nothrow) filter_compare_greater_field_t($3, *$5); delete[] $3; delete $5; }
    | OBJECT FIELD_ACCESS NAME GREATER_OR_EQUALS field_value { $$ = new (std::nothrow) filter_compare_greater_or_equals_field_t($3, *$5); delete[] $3; delete $5; }
    | OBJECT FIELD_ACCESS NAME INSTANCEOF STRING { $$ = new (std::nothrow) filter_apply_filter_t($3, std::make_unique<filter_instance_of_t>($5)); delete[] $3; delete[] $5; };

field_value: STRING { $$ = new (std::nothrow) filter_comp_value_t($1); delete[] $1;}
    | INT { $$ = new (std::nothrow) filter_comp_value_t($1); }
    | BOOL { $$ = new (std::nothrow) filter_comp_value_t($1); }
    | FLOAT { $$ = new (std::nothrow) filter_comp_value_t($1); };
%%

void hprof::language_parser::error (const location_type& loc, const std::string& msg) {
  driver.error (loc, msg);
}

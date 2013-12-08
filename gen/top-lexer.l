%option prefix="Top"
%option header-file="top-lexer.h"
%option outfile="top-lexer.cpp"
%option noyywrap
%option batch debug

%{
#include <stdlib.h>
#include "top-parser.hpp"
typedef top::TopParser::token token;
%}

num [1-9][0-9]+
dep ->
sep ,
space [ \t\n]*

%%
{num} { yylval->num = atoi(yytext); return token::NUMBER; }
{dep} { return token::DEP; }
{sep} { return token::SEP; }
{space}
.
%%

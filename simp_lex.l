%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h" // generated via yacc -d
%}

%option noyywrap

%%
"var"                                   { return VAR; }
"if"                                    { return IF; }
"else"                                  { return ELSE; }
"while"                                 { return WHILE; }
"do"                                    { return DO; }
"end"                                   { return END; }
"function"                              { return FUNCTION; }
"return"                                { return RETURN; }
"print"                                 { return PRINT; }

"=="                                    { return EQUALS; }
"!="                                    { return NOT_EQUALS; }
"||"                                    { return OR; }
"&&"                                    { return AND; }

[0-9]+(\.[0-9]+)?([eE][0-9]+)?          { return NUM; }
[a-z|A-Z]([a-z|A-Z]|[[:digit:]]|_)*     { return ID; }
[!-*/+-><=&|\(\)]                       { return yytext[0]; }
[ \t\f\v\n]                             { ; }
#.*\n                                   { ; }
%%
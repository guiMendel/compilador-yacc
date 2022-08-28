%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "list.h"

#include "y.tab.h" // generated via yacc -d

#define TOKEN(t) (yylval.token = t)
%}

%option noyywrap
%option nounput
%option noinput
%option yylineno

%%
"var"                                   { return TOKEN(VAR); }
"if"                                    { return TOKEN(IF); }
"else"                                  { return TOKEN(ELSE); }
"while"                                 { return TOKEN(WHILE); }
"do"                                    { return TOKEN(DO); }
"end"                                   { return TOKEN(END); }
"for"                                   { return TOKEN(FOR); }
"each"                                   { return TOKEN(EACH); }
"in"                                    { return TOKEN(IN); }
"function"                              { return TOKEN(FUNCTION); }
"return"                                { return TOKEN(RETURN); }
"read"                                  { return TOKEN(READ); }

"\.\."                                  { return TOKEN(RANGE); }

"=="                                    { return TOKEN(EQUALS); }
"!="                                    { return TOKEN(NOT_EQUALS); }
"or"                                    { return TOKEN(OR); }
"and"                                    { return TOKEN(AND); }

[0-9]+(\.[0-9]+)?([eE][0-9]+)?          { yylval.number = atoi(yytext); return NUM; }
[a-z|A-Z]([a-z|A-Z]|[[:digit:]]|_)*     { yylval.string = strdup(yytext); return ID; }
[;,!-*/+-><=&|\(\)\[\]]                     { return TOKEN(yytext[0]); }
[ \t\f\v\n]                             { ; }
#.*\n                                   { ; }
%%

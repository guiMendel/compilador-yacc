%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h" // generated via yacc -d

#include "scanner.h"

char tokenString[MAXTOKENLEN+1];
#define SAVE_TOKEN strncpy(tokenString, yytext, yyleng); tokenString[yyleng] = '\0';
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
"write"                                 { return WRITE; }
"read"                                 { return READ; }

"=="                                    { return EQUALS; }
"!="                                    { return NOT_EQUALS; }
"||"                                    { return OR; }
"&&"                                    { return AND; }

[0-9]+(\.[0-9]+)?([eE][0-9]+)?          { SAVE_TOKEN; return NUM; }
[a-z|A-Z]([a-z|A-Z]|[[:digit:]]|_)*     { SAVE_TOKEN; return ID; }
[;,!-*/+-><=&|\(\)]                     { return yytext[0]; }
[ \t\f\v\n]                             { ; }
#.*\n                                   { ; }
%%

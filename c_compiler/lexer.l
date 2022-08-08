%{
#include <sdtio.h>
#include <stdlib.h>
#include "y.tab.h" // generated via yacc -d
%}

%option noyywrap;

/*** Definition section ***/
/* https://www2.cs.arizona.edu/~debray/Teaching/CSc453/DOCS/cminusminusspec.html */

_ID           [a-z|A-Z] ([a-z|A-Z] | [[:digit:]] | _)*
_INTCON       [[:digit:]]+
_CHARCON      '[[:print:]]' | '\n' | '\0'
_STRINGCON    \"[[:print:]]*\"
_COMMENT      /\* [[:print:]]* /\*

/** Rules Section **/
%%
"=="                 { return EQUALS; }
"!="                 { return NOT_EQUALS; }
"<="                 { return LESS_EQUAL; }
">="                 { return BIGGER_EQUAL; }
"||"                 { return OR; }
"&&"                 { return AND; }

/* Pass operands right through */
[!-*/+-><=&|]        { return yytext[0]; }

/* Keywords */
"extern"             { return EXTERN; }
"void"               { return VOID; }
"char"               { return CHAR; }
"int"                { return INT; }
"if"                 { return IF; }
"else"               { return ELSE; }
"while"              { return WHILE; }
"for"                { return FOR; }
"return"             { return RETURN; }

/* Whitespace */
[ \t\f\v\n]          { ; }

{_ID}                 { yylval.s = yytext; return ID; }
{_INTCON}             { yylval.i = atoi(yytext); return INTCON; }
{_CHARCON}            { yylval.c = yytext[0]; return CHARCON; }
{_STRINGCON}          { yylval.s = yytext; return STRINGCON; }
{_COMMENT}            { ; }


%%

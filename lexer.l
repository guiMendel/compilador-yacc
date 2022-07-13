%{
#include <stdio.h>
FILE *out ;
%}

/*** Definition section ***/
KEYWORD      "auto"|"if"|"else"|"for"|"while"|"do"|"switch"|"include"|"case"|"break"|"struct"|"union"|"int"|"char"|"float"|"double"|"long"
NUMCONST    (-?[[:digit:]])+([\.]([[:digit:]]*))?
IDENTIFIER   [_|a-z|A-Z][a-z|A-Z|0-9|_]*
STRINGCONST   \"[[:alnum:]]*\"
SYM          [""|#]

/** Rules Section **/
%%
"="         { fprintf(out, "<attribution op, '%s' >\n", yytext); }
"+"         { fprintf(out, "<arith op, '%s' >\n", yytext); }
"-"         { fprintf(out, "<arith op, '%s' >\n", yytext); }
"*"         { fprintf(out, "<arith op, '%s' >\n", yytext); }
"/"         { fprintf(out, "<arith op, '%s' >\n", yytext); }
"%"         { fprintf(out, "<arith op, '%s' >\n", yytext); }
"++"        { fprintf(out, "<arith op, '%s' >\n", yytext); }
"--"        { fprintf(out, "<arith op, '%s' >\n", yytext); }
"<"         { fprintf(out, "<logical op, '%s' >\n", yytext); }
">"         { fprintf(out, "<logical op, '%s' >\n", yytext); }
"=="        { fprintf(out, "<logical op, '%s' >\n", yytext); }
"!="        { fprintf(out, "<logical op, '%s' >\n", yytext); }
">="        { fprintf(out, "<logical op, '%s' >\n", yytext); }
"<="        { fprintf(out, "<logical op, '%s' >\n", yytext); }
"||"        { fprintf(out, "<logical op, '%s' >\n", yytext); }
"&&"        { fprintf(out, "<logical op, '%s' >\n", yytext); }
"//"        { fprintf(out, "<inline comment, '%s' >\n", yytext); }
"/*"        { fprintf(out, "<block comment open, '%s' >\n", yytext); }
"*/"        { fprintf(out, "<block comment close, '%s' >\n", yytext); }
";"         { fprintf(out, "<end-of-satement, '%s' >\n", yytext); }
"["         { fprintf(out, "<open square bracket, '%s' >\n", yytext); }
"]"         { fprintf(out, "<close square bracket, '%s' >\n", yytext); }
"("         { fprintf(out, "<open parenthesis, '%s' >\n", yytext); }
")"         { fprintf(out, "<open parenthesis, '%s' >\n", yytext); }
"{"         { fprintf(out, "<open curly brace, '%s' >\n", yytext); }
"}"         { fprintf(out, "<open curly brace, '%s' >\n", yytext); }
","         { fprintf(out, "<comma, '%s' >\n", yytext); }
"."         { fprintf(out, "<period, '%s' >\n", yytext); }
":"         { fprintf(out, "<colon, '%s' >\n", yytext); }
"&"         { fprintf(out, "<ampersand, '%s' >\n", yytext); }

{SYM}          { fprintf(out, "<symbol, '%s' >\n", yytext); }
{KEYWORD}      { fprintf(out, "<keyword, '%s' >\n", yytext); } 
{IDENTIFIER}   { fprintf(out, "<id, '%s' >\n", yytext); }
{NUMCONST}     { fprintf(out, "<num_const, '%s' >\n", yytext); }
{STRINGCONST}  { fprintf(out, "<string_const, '%s' >\n", yytext); }


%%

int yywrap();

int main(int argc, char *argv[]){
    
    FILE *f = fopen(argv[1],"r");

    if (!f) {
      printf("error: file not found in path");
      return -1;
    }

    yyin = f;
    out = fopen(argv[2],"w");
    yylex();

    return 0;
}

int yywrap(){
    return 1;
}
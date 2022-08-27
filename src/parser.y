%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
void yyerror(char *message);

#include "ast.h"
#include "list.h"
#include "compiler.h"
#include "printer.h"
#include "scanner.h"

Function *fn;

#define YYSTYPE AstNode *
%}

%token VAR
%token ID
%token NUM
%token IF
%token ELSE
%token WHILE
%token DO
%token END
%token FUNCTION
%token RETURN
%token WRITE
%token READ

%right THEN ELSE

%right '='
%left OR
%left AND
%left EQUALS NOT_EQUALS
%left '<' '>'
%left '+' '-'
%left '*' '/'
%right '!'

%%
program : statements { printNode($1, 0); }
        ;

statements : empty { AstNode block; ast_block_init(&block); $$ = &block; }
           | statement statements { list_push(&$2->as_block.stmts, $1); $$ = $2; }
           | function.declaration statements
           ;

function.declaration : FUNCTION ID '(' parameters ')' DO statements END
                     ;

parameters : empty | ID more.parameters;

more.parameters : empty
               | ',' ID more.parameters
               ;

statement : expression ';' { $$ = $1; }
          | declaration ';'
          | IF '(' expression ')' statement                   %prec THEN
          | IF '(' expression ')' statement ELSE statement
          | WHILE '(' expression ')' statement
          | DO statements END
          | RETURN ';'
          | RETURN expression ';'
          | READ ID ';'
          | WRITE expression ';'
          | ';'
          ;

declaration : VAR ID
            ;

expression : '(' expression ')'
           | ID '=' expression
           | operation { $$ = $1; }
           | function.call
           ;

function.call : ID '(' arguments ')'
              ;

arguments : empty | expression more.arguments;

more.arguments : empty
               | ',' expression more.arguments
               ;

operation : operation OR operation { AstNode node; ast_binop_init(&node, BINOP_OR, $1, $3); $$ = &node; }
          | operation2 { $$ = $1; }
          ;

operation2 : operation2 AND operation2 { AstNode node; ast_binop_init(&node, BINOP_AND, $1, $3); $$ = &node; }
           | operation3 { $$ = $1; }
           ;

operation3 : operation3 EQUALS operation3 { AstNode node; ast_binop_init(&node, BINOP_EQ, $1, $3); $$ = &node; }
           | operation3 NOT_EQUALS operation3 { AstNode node; ast_binop_init(&node, BINOP_NEQ, $1, $3); $$ = &node; }
           | operation4 { $$ = $1; }
           ;

operation4 : operation4 '<' operation4 { AstNode node; ast_binop_init(&node, BINOP_LT, $1, $3); $$ = &node; }
           | operation4 '>' operation4 { AstNode node; ast_binop_init(&node, BINOP_GT, $1, $3); $$ = &node; }
           | operation5 { $$ = $1; }
           ;

operation5 : operation5 '+' operation5 { AstNode node; ast_binop_init(&node, BINOP_ADD, $1, $3); $$ = &node; }
           | operation5 '-' operation5 { AstNode node; ast_binop_init(&node, BINOP_SUB, $1, $3); $$ = &node; }
           | operation6 { $$ = $1; }
           ;

operation6 : operation6 '*' operation6 { AstNode node; ast_binop_init(&node, BINOP_MUL, $1, $3); $$ = &node; }
           | operation6 '/' operation6 { AstNode node; ast_binop_init(&node, BINOP_DIV, $1, $3); $$ = &node; }
           | operation7 { $$ = $1; }
           ;

operation7 : '-' operation7 { AstNode node; ast_unop_init(&node, UNOP_NEG, $2); $$ = &node; }
           | '!' operation7 { AstNode node; ast_unop_init(&node, UNOP_NOT, $2); $$ = &node; }
           | NUM { AstNode node; ast_number_init(&node, atoi(tokenString)); $$ = &node; }
           | ID { AstNode node; ast_ident_init(&node, tokenString); $$ = &node; }
           ;

empty : /* empty */;
%%


void yyerror(char *message) {
  fprintf(stderr, "Error: %s\n", message);
  exit(1);
}

int main() {
  Function f;
  function_init(&f, "main");

  int yydebug = 1;
  
  yyparse();

  printFunction(&f);

  return 0;
}

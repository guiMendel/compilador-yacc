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
program : statements { fn->code = $1; }
        ;

statements : empty {$$ = new_block_node(); }
           | statement statements { list_push(&$2->as_block.stmts, $1); $$ = $2; }
           | function.declaration statements
           ;

function.declaration : FUNCTION ID '(' parameters ')' DO statements END
                     ;

parameters : empty | ID more.parameters;

more.parameters : empty
               | ',' ID more.parameters
               ;

statement : expression ';'
          | declaration ';'
          | IF '(' expression ')' statement                   %prec THEN { $$ = new_if_node($3, $5, NULL); }
          | IF '(' expression ')' statement ELSE statement   { $$ = new_if_node($3, $5, $7); }
          | WHILE '(' expression ')' statement { $$ = new_while_node($3, $5); }
          | DO statements END { $$ = $2; }
          | RETURN ';' { $$ = new_return_node(NULL); }
          | RETURN expression ';' { $$ = new_return_node($2); }
          | READ ID ';'
          | WRITE expression ';'
          | ';'
          ;

declaration : VAR ID
            ;

expression : '(' expression ')' { $$ = $2; }
           | ID '=' expression { $$ = new_assign_node(tokenString, $3); }
           | operation { $$ = $1; }
           | function.call
           ;

function.call : ID '(' arguments ')'
              ;

arguments : empty | expression more.arguments;

more.arguments : empty
               | ',' expression more.arguments
               ;

operation : operation OR operation { $$ = new_binop_node(BINOP_OR, $1, $3); }
          | operation2 { $$ = $1; }
          ;

operation2 : operation2 AND operation2 { $$ = new_binop_node(BINOP_AND, $1, $3); }
           | operation3 { $$ = $1; }
           ;

operation3 : operation3 EQUALS operation3 { $$ = new_binop_node(BINOP_EQ, $1, $3); }
           | operation3 NOT_EQUALS operation3 { $$ = new_binop_node(BINOP_NEQ, $1, $3); }
           | operation4 { $$ = $1; }
           ;

operation4 : operation4 '<' operation4 { $$ = new_binop_node(BINOP_LT, $1, $3); }
           | operation4 '>' operation4 { $$ = new_binop_node(BINOP_GT, $1, $3); }
           | operation5 { $$ = $1; }
           ;

operation5 : operation5 '+' operation5 { $$ = new_binop_node(BINOP_ADD, $1, $3); }
           | operation5 '-' operation5 { $$ = new_binop_node(BINOP_SUB, $1, $3); }
           | operation6 { $$ = $1; }
           ;

operation6 : operation6 '*' operation6 { $$ = new_binop_node(BINOP_MUL, $1, $3); }
           | operation6 '/' operation6 { $$ = new_binop_node(BINOP_DIV, $1, $3); }
           | operation7 { $$ = $1; }
           ;

operation7 : '-' operation7 { $$ = new_unop_node(UNOP_NEG, $2); }
           | '!' operation7 { $$ = new_unop_node(UNOP_NOT, $2); }
           | NUM { $$ = new_number_node(atoi(tokenString)); }
           | ID { $$ = new_ident_node(tokenString); }
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
  fn = &f;

  int yydebug = 1;
  
  yyparse();

  unsigned char chunk[1024];
  compile(&f, chunk);

  return 0;
}
